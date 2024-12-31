#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include "segment.h"
#include "workera.h"
#include "utils.h"

#define PRINT {fprintf(stderr, "%s: %d\n", __FILE__, __LINE__);}

#define MAIN_REQUEST_PIPE_PATH "server_request_pipe"
#define BUFFER_SIZE (sizeof(pid_t) + sizeof(int) + 4 * sizeof(size_t))

int open_requests_pipe(int *fd);
void write_log(const char *format, ...);
int open_response_pipe(pid_t pid);
void gestionnaire(int sig);
void setup_signal_handlers();
int main(void) {
  int requests;
  if (open_requests_pipe(&requests) != 0) {
    return EXIT_FAILURE;
  }
  char buffer[BUFFER_SIZE];
  size_t buffer_size = BUFFER_SIZE;
  size_t bytesToRead = 0;
  ssize_t r;
  while (1) {
    bytesToRead = buffer_size;
    pid_t request_pid = -1;
    int maxValue = 0;
    size_t matrixes[4] = {0};
    {
      char *ptr = buffer;
      while (bytesToRead != 0
            && (r = read(requests,
              (void *) ptr, bytesToRead)) != (ssize_t) bytesToRead) {
        if (r == (ssize_t) -1) {
          goto error_read;
        }
        bytesToRead -= (size_t) r;
        ptr += r;
      }
      ptr = buffer;
      request_pid = *(pid_t *) ptr;
      ptr = (ptr + sizeof(pid_t));
      maxValue = *(int *) ptr;
      ptr = (ptr + sizeof(int));
      matrixes[4] = 0;
      for (int i = 0; i < 4; ++i) {
        matrixes[i] = *(size_t *) ptr;
        ptr = (ptr + sizeof(size_t));
      }
    }
    pid_t pid = fork();
    if (pid == (pid_t) -1) {
      goto error_fork;
    }
    {
      if (pid == 0) {
        if (matrixes[1] != matrixes[2]) {
          exit(EXIT_SUCCESS);
        }
        segment_t *s = worker_a(matrixes[0], matrixes[1], matrixes[3], maxValue);
        if (s == NULL) {
          goto error;
        }
#if defined DEBUG && DEBUG != 0
        fprintf(stderr, "request pid: %d\n", (int) request_pid);
        debug_segment(s);
#else
        int fd_response = open_response_pipe(request_pid);
        if (fd_response == -1) {
          goto error;
        }
        if (segment_write_matrixes(fd_response, s) != 0) {
          goto error;
        }
        if (close(fd_response) == -1) {
          perror("close");
          goto error;
        }
        if (munmap(s, segment_get_size(s)) != 0) {
          perror("unmap");
          goto error;
        }
#endif
        return EXIT_SUCCESS;
      }
    }
    if (wait(NULL) == -1) {
      goto error;
    }
  }
  return EXIT_SUCCESS;

error_read:
  perror("read");
  goto error;

error_fork:
  perror("fork");

error:
  write_log("exit from main server\n");
  wait(NULL);
  return EXIT_FAILURE;
}

void gestionnaire([[maybe_unused]]int sig) {
  pid_t pgrp = getpgrp();
  if (pgrp != -1) {
    // Send SIGTERM signal to all processes in the process group
    if (kill(-pgrp, SIGTERM) == -1) {
      perror("kill");
    }
  } else {
    perror("getpgrp");
  }
}

void setup_signal_handlers() {
    struct sigaction action;
    action.sa_handler = gestionnaire;
    action.sa_flags = 0;
    sigfillset(&action.sa_mask);
    int signals[] = {SIGINT, SIGTERM, SIGHUP, SIGQUIT};
    int nbSignals = sizeof(signals) / sizeof(signals[0]);
    for (int i = 0; i < nbSignals; ++i) {
        if (sigaction(i, &action, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
    }
}

int open_requests_pipe(int *fd) {
  int requests;
  if ((requests = open(MAIN_REQUEST_PIPE_PATH, O_RDONLY)) == -1) {
    if (errno == ENOENT) {
     if (mkfifo(MAIN_REQUEST_PIPE_PATH, 0666) != 0) {
        perror("mkfifo");
        return -1;
      }
      return open_requests_pipe(fd);
    }
    perror("open");
    return -1;
  }
  *fd = requests;
  return 0;
}

int open_response_pipe(pid_t pid) {
  char *pipe_name = get_pid_based_name(pid);
  int response_fd;
  if ((response_fd = open(pipe_name, O_WRONLY)) == -1) {
    perror("open");
  }
  free(pipe_name);
  return response_fd;
}

void write_log([[maybe_unused]]const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}
