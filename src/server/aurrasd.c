#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include "dup_aux.h"
#include "readln.h"

#define BUFFER_SIZE 4096 // 4 KB

static char CONFIG[5][4][30];

void term_sig(int signal)
{
  printf("good bye\n");
  _exit(0);
}

void parseConfigLines(int fd)
{
  char buffer[BUFFER_SIZE];
  int bytes_read;
  char *helper, *token;
  int config_line = 0;
  int i;
  while ((bytes_read = readln(fd, buffer, BUFFER_SIZE)) > 0)
  {
    printf("cá dentro \n");
    write(1, buffer, bytes_read);
    for (i = 0, helper = buffer; i < 3; i++, helper = NULL)
    {
      token = strtok(helper, " \n");
      if (token == NULL)
      {
        break;
      }
      strcpy(CONFIG[config_line][i], token);
    }
    config_line++;
  }
}

int main(int argc, int *argv[])
{
  signal(SIGTERM, term_sig);

  int fd_s, fd_c;
  int log;
  int err;
  int config;
  int bytes_read;
  char buffer[BUFFER_SIZE];
  // create_fifo(CLIENT_PIPE);

  open_dup(LOG_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666, STDOUT_FILENO); // ficheiro de log
  open_dup(ERR_FILE, O_CREAT | O_APPEND | O_WRONLY, 0666, STDERR_FILENO); // ficheiro de erros

  create_fifo(REQUEST_PIPE);
  if ((config = open("../etc/aurrasd.conf", O_RDONLY)) < 0)
  {
    perror("config file");
  }
  else
  {
    parseConfigLines(config);
  }

  for (int i = 0; i < 5; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      printf("%s cenas \n", CONFIG[i][j]);
    }
  }

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
    buffer[bytes_read - 1] = '\0';
    printf("%s\n", buffer + 6);
    if (strcmp("exit", buffer + 6) == 0)
      close(fd_c);
    write(STDOUT_FILENO, buffer, bytes_read);
  }

  return 0;
}
