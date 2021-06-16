#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "dup_aux.h"

void create_fifo(char *filename)
{
  if (mkfifo(filename, 0666) == -1)
  {
    perror(filename);
  }
  // else
  // {
  //   printf("[debug] fifo %s file created\n", filename);
  // }
}

void open_dup(char *name, int flag, mode_t mode, int dt)
{
  int fd;
  if ((fd = open(name, flag, mode)) < 0)
  {
    perror(name);
  }
  // else
  // {
  //   printf("[debug] opened file %s\n", name);
  // }
  dup2(fd, dt);
  close(fd);
}

int check_file_exists(char *name)
{
  int fd;
  if ((fd = open(name, O_RDONLY)) < 0)
  {
    perror(name);
    return 0;
  }
  else
  {
    close(fd);
    return 1;
  }
}
