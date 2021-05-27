#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define REQUEST_PIPE "SERVER_REQUEST"
#define CLIENT_PIPE "SERVER_ANSWER"


#define BUFFER_SIZE 2048
int main(int argc, int *argv[]) {

  int fd, bytes_read;
  char buffer[BUFFER_SIZE];
  char command[BUFFER_SIZE+50];

  if ((fd = open("REQUEST_PIPE", O_WRONLY)) < 0)
  {
    perror("error fifo write");
  }
  else
  {
    printf("[debug] fifo file opened for writing\n");
  }

  dup2(fd, STDOUT_FILENO);
  close(fd);

  while((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
    buffer[bytes_read] = '\0';
    sprintf(command,"%d %s",getpid(),buffer);
    write(STDOUT_FILENO, command, strlen(command));
  }
  return 0;
}
