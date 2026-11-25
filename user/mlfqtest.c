// usage: mlfqtest [long_ticks] [work_ticks]
#include "kernel/types.h"
#include "kernel/pinfo.h"
#include "user/user.h"

#define MAXP 64

static char *states[] = { "UNUSED", "USED", "SLEEPING", 
                          "RUNNABLE", "RUNNING", "ZOMBIE" };

static struct pinfo list[MAXP];

static void
show(char *title)
{
  int n = getprocs(list, MAXP);
  
  printf("---- %s (tick %d) ----\n", title, uptime());
  printf("PID\tPPID\tSTATE     Q\tWAIT\tBURST\tCONF\tNAME\n");
  for (int i = 0; i < n; i++) {
    char *s = states[list[i].state];
    printf("%d\t%d\t%s", list[i].pid, list[i].ppid, s);
    for (int k = strlen(s); k < 10; k++)
      printf(" ");
    printf("%d\t%d\t%d\t%d\t%s\n", list[i].queue, list[i].wait,
           list[i].burst, list[i].confidence, list[i].name);
  }
}

// busy loop for `run` ticks; c0 prints a queue table every `step` ticks
static void
worker(int id, int run, int step)
{
  int t0 = uptime();
  int next = t0 + step;
  long count = 0;
  
  while (uptime() - t0 < run) {
    count++;
    
    if (step > 0 && uptime() >= next) {
      show("snapshot taken by child 0");
      next += step;
    }
  }
  
  printf("child %d (pid %d) done, counter = %ld\n", id, getpid(), count);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int longrun = 1000, work = 300;
  int bursts[4] = {2, 3, 6, 12};
  
  if (argc > 1)
    longrun = atoi(argv[1]);
    
  if (argc > 2)
    work = atoi(argv[2]);
    
  for (int i = 0; i < 4; i++) {
    int pid = fork();
    
    if (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }
    
    if (pid == 0) {
      if (i == 0)
        worker(0, longrun, longrun / 8);
      else
        worker(i, work, 0);
    }
    
    if (i > 0)
      setsched(pid, bursts[i], 80); // burst 3, 6, 12 with confidence 80
  }
  
  show("right after fork: children start in queue 3, sh is in queue 1");
  
  for (int i = 0; i < 4; i++)
    wait(0);
    
  printf("mlfqtest finished\n");
  exit(0);
}