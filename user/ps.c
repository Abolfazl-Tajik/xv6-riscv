#include "kernel/types.h"
#include "kernel/pinfo.h"
#include "user/user.h"

#define MAXP 64

static struct pinfo list[MAXP];

static char *states[] = { "UNUSED", "USED", "SLEEPING",
                          "RUNNABLE", "RUNNING", "ZOMBIE" };

int
main(int argc, char *argv[])
{
  int n = getprocs(list, MAXP);
  if(n < 0){
    fprintf(2, "ps: getprocs failed\n");
    exit(1);
  }

  printf("PID\tPPID\tSTATE     SIZE\tNAME\n");
  for(int i = 0; i < n; i++){
    char *s = "???";
    if(list[i].state >= 0 && list[i].state <= 5)
      s = states[list[i].state];
    printf("%d\t%d\t%s", list[i].pid, list[i].ppid, s);
    for(int k = strlen(s); k < 10; k++)
      printf(" ");
    printf("%d\t%s\n", (int)list[i].sz, list[i].name);
  }
  exit(0);
}