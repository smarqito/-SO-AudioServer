#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <string.h>
#include "aurras.h"
#include "dup_aux.h"

#define REQUEST_PIPE "SERVER_REQUEST"

#define BUFFER_SIZE 4096

char self_pid[10];

void handle_term(int status)
{
  unlink(self_pid);
  _exit(status);
}

int main(int argc, char *argv[])
{
  signal(SIGINT, handle_term);
  signal(SIGTERM, handle_term);
  if (argc > 1)
  {
    int fd, bytes_read;
    char buffer[BUFFER_SIZE];

    char self_pid[10];
    sprintf(self_pid, "%d", getpid());
    create_fifo(self_pid);

    if ((fd = open(REQUEST_PIPE, O_WRONLY)) < 0)
    {
      perror("error fifo write");
    }

    strcat(buffer, self_pid);
    strcat(buffer, " ");

    for (int i = 1; i < argc; i++)
    {
      strcat(buffer, argv[i]);
      strcat(buffer, " ");
    }

    write(fd, buffer, strlen(buffer)); // faz request ao server

    close(fd);

    open_dup(self_pid, O_RDONLY, 0666, 0); // abre o fifo para leitura
    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0)
    {
      write(STDOUT_FILENO, buffer, bytes_read); // faz request ao server
    }

    unlink(self_pid);
  }
  else
  {
    printf("helper:\n");
    printf("aurras status\n");
    printf("aurras transform <input_file> <output_file> <filter_1> ... <filter_n>\n");
    fflush(NULL);
  }

  return 0;
}
