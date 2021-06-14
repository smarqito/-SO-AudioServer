#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "dup_aux.h"


void create_fifo(char *filename)
{
  if (mkfifo(filename, 0666) == -1)
  {
    perror("error creating fifo.");
  }
  else
  {
    printf("[debug] fifo %s file created\n", filename);
  }
}

void open_dup(char *name, int flag, mode_t mode, int dt)
{
  int fd;
  if ((fd = open(name, flag, mode)) < 0)
  {
    perror("error");
  }
  else
  {
    printf("[debug] opened file %s\n", name);
  }
  dup2(fd, dt);
  close(fd);
}
