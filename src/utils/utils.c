#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

char *get_pid_based_name(pid_t __pid) {
  int pid = (int) __pid;
  size_t strSize = (size_t) snprintf(NULL, 0, "%d", pid);
  char *str = malloc(strSize + 1);
  snprintf(str, strSize, "%d", pid);
  return str;
}
