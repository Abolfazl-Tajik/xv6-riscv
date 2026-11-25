#include "kernel/types.h"
#include "user/user.h"

#define RUN_TICKS 300

struct result {
  int nice;
  long count;
};

static int
weight_of(int nice)
{
  if (nice == 0) return 1024;
  if (nice == 5) return 335;
  return 110;
}

// print x100 as a decimal with two digits
static void
ratio(int x100)
{
  printf("%d.%d%d", x100 / 100, (x100 / 10) % 10, x100 % 10);
}

int
main(void)
{
  int nices[3] = {0, 5, 10};
  struct result r[3];
  int fd[2];

  pipe(fd);
  for (int i = 0; i < 3; i++) {
    if (fork() == 0) {
      struct result me;
      setnice(getpid(), nices[i]);
      me.nice = nices[i];
      me.count = 0;
      int t0 = uptime();
      while (uptime() - t0 < RUN_TICKS)
        me.count++;
      write(fd[1], &me, sizeof(me));
      exit(0);
    }
  }

  for (int i = 0; i < 3; i++)
    read(fd[0], &r[i], sizeof(r[i]));
  for (int i = 0; i < 3; i++)
    wait(0);

  long c0 = 1;
  for (int i = 0; i < 3; i++) {
    printf("nice=%d weight=%d counter=%ld\n", r[i].nice,
           weight_of(r[i].nice), r[i].count);
    if (r[i].nice == 0)
      c0 = r[i].count;
  }
  for (int i = 0; i < 3; i++) {
    if (r[i].nice == 0)
      continue;
    printf("nice 0 / nice %d: measured ", r[i].nice);
    ratio((int)(c0 * 100 / r[i].count));
    printf(", expected ");
    ratio(1024 * 100 / weight_of(r[i].nice));
    printf("\n");
  }
  exit(0);
}