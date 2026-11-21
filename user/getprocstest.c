#include "kernel/types.h"
#include "kernel/pinfo.h"
#include "user/user.h"

static struct pinfo list[64];
static char *states[] = { "UNUSED", "USED", "SLEEPING",
                          "RUNNABLE", "RUNNING", "ZOMBIE" };

int
main(void)
{
  int pids[3];
  int p[2];
  pipe(p);

  for(int i = 0; i < 3; i++){
    int pid = fork();
    if(pid == 0){
      if(i < 2){ char c; read(p[0], &c, 1); exit(0); }
      for(;;);
    }
    pids[i] = pid;
  }
  pause(10);

  
  int c = fork();
  if(c == 0){
    char *args[] = { "ps", 0 };
    exec("ps", args);
    exit(1);
  }
  wait(0);

  int n = getprocs(list, 64);
  printf("parent pid=%d, total=%d\n", getpid(), n);
  for(int i = 0; i < n; i++)
    printf("  pid=%d ppid=%d state=%s name=%s\n", list[i].pid,
           list[i].ppid, states[list[i].state], list[i].name);

  printf("max=2    -> %d (expected 2)\n", getprocs(list, 2));
  printf("max=1000 -> %d (expected %d)\n", getprocs(list, 1000), n);
  printf("max=0    -> %d (expected -1)\n", getprocs(list, 0));
  printf("buf=NULL -> %d (expected -1)\n", getprocs(0, 10));
  printf("bad addr -> %d (expected -1)\n",
         getprocs((struct pinfo*)0x80000000, 10)); 

  for(int i = 0; i < 3; i++) kill(pids[i]);
  for(int i = 0; i < 3; i++) wait(0);
  exit(0);
}