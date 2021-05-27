#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "dup_aux.h"


void create_fifo()
{
  if (mkfifo(REQUEST_PIPE, 0666) == -1)
  {
    perror("error creating fifo.");
  }
  else
  {
    printf("[debug] fifo file created\n");
  }
}

void open_dup(char *name, int flag, mode_t mode, int dt)
{
  int fd;
  if ((fd = open(name, flag, mode)) < 0)
  {
    perror("error reading fifo");
  }
  else
  {
    printf("[debug] fifo file opened for reading\n");
  }
  dup2(fd, dt);
  close(fd);
}
