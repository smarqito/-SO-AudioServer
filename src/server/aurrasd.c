#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dup_aux.h"


#define BUFFER_SIZE 4096 // 4 KB



int main(int argc, int *argv[])
{
  int fd_s, fd_c;
  int log;
  int err;
  int bytes_read;
  char buffer[BUFFER_SIZE];

  create_fifo();

  //open_dup(LOG_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666, STDOUT_FILENO); // ficheiro de log
  open_dup(ERR_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666, STDERR_FILENO); // ficheiro de erros


  open_dup(REQUEST_PIPE, O_RDONLY, 0, STDIN_FILENO); // STDIN passa a ser o PIPE


  if ((fd_c = open(REQUEST_PIPE, O_WRONLY)) < 0)
  {
    perror("error fifo write");
  }
  else
  {
    printf("[debug] fifo file opened for writing\n");
  }

  while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0)
  {
    write(STDOUT_FILENO, buffer, bytes_read);
  }

  return 0;
}
