struct pinfo {
  int pid;
  int ppid;
  int state;
  uint64 sz;
  char name[16];

  // (MLFQ)
  int queue;
  int wait;
  int burst;
  int confidence;
};