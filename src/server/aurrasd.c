#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "dup_aux.h"

#define BUFFER_SIZE 4096 // 4 KB

int POOL_PID;
int KEEP_RUN;

void term_sig(int signal)
{
  close(KEEP_RUN);
  printf("[debug] A fechar a pool\n");
  fflush(NULL);
  //kill(POOL_PID, SIGUSR2);
  waitpid(POOL_PID, NULL, 0);
  printf("[debug] goodbye\n");
  _exit(0);
}

void term_config_error(int signal)
{
  char config_error[100];
  sprintf(config_error, "Erro a carregar o ficheiro de configuração. A fechar servidor.\n");
  write(STDOUT_FILENO, config_error, strlen(config_error));
  _exit(1);
}

int main(int argc, char *argv[])
{
  signal(SIGTERM, term_sig);
  signal(SIGUSR1, term_config_error);

  int fd_c;
  int bytes_read;
  char buffer[BUFFER_SIZE];

  if (argc == 3) // execute server
  {

    create_fifo(POOL_PIPE);

    if ((POOL_PID = fork()) == 0) // create thread pool
    {
      int status = execl("./pool", "pool", argv[1], argv[2], NULL);
      _exit(status);
    }
    else
    {
      //open_dup(LOG_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666, STDOUT_FILENO); // ficheiro de log
      open_dup(ERR_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666, STDERR_FILENO); // ficheiro de erros

      create_fifo(REQUEST_PIPE);

      open_dup(REQUEST_PIPE, O_RDONLY, 0, STDIN_FILENO); // STDIN passa a ser o PIPE

      if ((KEEP_RUN = open(REQUEST_PIPE, O_WRONLY)) < 0) // bloqueia o descritor de escrita; servidor sempre a correr
      {
        perror("error fifo write");
      }
      else
      {
        printf("[debug] fifo file opened for writing\n");
      }

      open_dup(POOL_PIPE, O_WRONLY, 0666, STDOUT_FILENO);

      char exit_[5];
      while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0)
      {
        buffer[bytes_read - 1] = '\0';
        snprintf(exit_, 4, "%s", buffer);
        write(STDOUT_FILENO, buffer, bytes_read);
      }
    }
  }
  else
  {
    char error[150];
    sprintf(error, "helper:\n> aurrasd <config_file> <filters_folder>\n");
    write(STDOUT_FILENO, error, strlen(error));
  }

  return 0;
}
