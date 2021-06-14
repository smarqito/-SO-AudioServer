#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 4096
char read_buffer[BUFFER_SIZE];
int read_buffer_pos = 0;
int read_buffer_end = 0;



int readc(int fd, char *c)
{
    if (read_buffer_pos == read_buffer_end)
    {
        read_buffer_end = read(fd, read_buffer, BUFFER_SIZE);
        switch (read_buffer_end)
        {
        case -1:
            perror("read_line");
            break;
        case 0:
            return 0;
            break;
        default:
            read_buffer_pos = 0;
        }
    }
    *c = read_buffer[read_buffer_pos++];
    return 1;
}
ssize_t readln(int fd, char *line, size_t size)
{
    int res = 0;
    int i = 0;
    while (i < size && (res = readc(fd, line + i) > 0))
    {
        i++;
        if ((line)[i - 1] == '\n') {
            line[i-1] = '\0';
            return i;
        }
    }
    return 0;
}

/**
 * @brief Debug area
 * 
 */
// void parseConfigLines(int fd)
// {
//     char buffer[BUFFER_SIZE];
//     int bytes_read;
//     char *helper, *token;
//     int config_line = 0;
//     int i;
//     while ((bytes_read = readln(fd, buffer, BUFFER_SIZE)) > 0)
//     {
//         write(1, buffer, bytes_read);
//         for (i = 0, helper = buffer;; i++, helper = NULL)
//         {
//             token = strtok(helper, " \n");
//             printf("token: %s \n", token);
//             if (token == NULL)
//             {
//                 break;
//             }
//             strcpy(CONFIG[config_line][i], token);
//         }
//         config_line++;
//     }
// }

// int main(int argc, char const *argv[])
// {
//     int config;
//     if ((config = open("aurrasd.conf", O_RDONLY)) < 0)
//     {
//         perror("config file");
//     }
//     else
//     {
//         parseConfigLines(config);
//     }

//     for (int i = 0; i < 5; i++)
//     {
//         for (int j = 0; j < 4; j++)
//         {
//             printf("%s\n", CONFIG[i][j]);
//         }
//     }
//     return 0;
// }
