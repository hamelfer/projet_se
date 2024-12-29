#include "utils.h"
#include <stddef.h>
#include <stdio.h>

char *get_pid_based_name() {
  int pid = (int) getpid();
  size_t strSize = (size_t) snprintf(NULL, 0, "%d", pid);
  char *str = malloc(strSize + 1);
  snprintf(str, strSize, "%d", pid);
  return str;
}
