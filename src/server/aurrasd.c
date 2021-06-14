#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include "dup_aux.h"
#include "readln.h"
#include "pool.h"

#define BUFFER_SIZE 4096 // 4 KB

void term_sig(int signal)
{
  printf("good bye\n");
  _exit(0);
}

int main(int argc, char *argv[])
{
  signal(SIGTERM, term_sig);

  int fd_c;
  int bytes_read;
  int p[2];
  char buffer[BUFFER_SIZE];
  // create_fifo(CLIENT_PIPE);
  if (argc == 3) // execute server
  {
    if (pipe(p) < 0)
    {
      perror("pipe error");
    }

    if (fork() == 0) // create thread pool
    {
      close(p[1]);
      dup2(p[0], STDIN_FILENO);
      pool(argv[1], argv[2]);
    }
    else
    {
      open_dup(LOG_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666, STDOUT_FILENO); // ficheiro de log
      open_dup(ERR_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666, STDERR_FILENO); // ficheiro de erros

      create_fifo(REQUEST_PIPE);

      open_dup(REQUEST_PIPE, O_RDONLY, 0, STDIN_FILENO); // STDIN passa a ser o PIPE

      if ((fd_c = open(REQUEST_PIPE, O_WRONLY)) < 0) // bloqueia o descritor de escrita; servidor sempre a correr
      {
        perror("error fifo write");
      }
      else
      {
        printf("[debug] fifo file opened for writing\n");
      }
      close(p[0]);
      dup2(p[1], STDOUT_FILENO);
      
      char exit_[5];
      while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0)
      {
        buffer[bytes_read - 1] = '\0';
        snprintf(exit_, 4, "%s", buffer);
        if (strcmp("exit", exit_) == 0)
          close(fd_c);
        write(STDOUT_FILENO, buffer, bytes_read);
      }
    }
  }
  else
  {
    char error[150];
    sprintf(error, "erro!\nargumentos: aurrasd <config_file> <filters_folder>\n");
    write(STDOUT_FILENO, error, strlen(error));
  }

  return 0;
}
