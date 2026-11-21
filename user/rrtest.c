// 3 CPU-bound children; run with: make qemu CPUS=1 DEBUG=1
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  for (int i = 0; i < 3; i++) {
    if (fork() == 0) {
      int t0 = uptime();
      
      while (uptime() - t0 < 30)
        ;
        
      exit(0);
    }
  }
  
  for (int i = 0; i < 3; i++)
    wait(0);
    
  exit(0);
}