#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include "pool.h"
#include "queue.h"
#include "dup_aux.h"
#include "config.h"

typedef struct pool *Pool;

#define BUFFER 4096

struct pool
{
    Queue queue;
    Config_Server config;
} PNode;

Pool init_pool()
{
    Pool new = malloc(sizeof(PNode));
    new->queue = init_queue();
    new->config = init_config_server();
    return new;
}

void load_config(Pool p, char *config_file, char *filters_folder)
{
    int fd = open(config_file, O_RDONLY);
    if (fd < 0)
    {
        perror("config_file");
    }
    set_filters_folder(p->config, filters_folder);
    parseConfigLines(fd, p->config);
}

void handle_pool(Pool pool)
{
    Task t = get_next_task(pool->queue);
    if (t)
    {
        set_task_status(t, PROCESSING);
        switch (fork())
        {
        case -1:
            perror("fork error");
            break;
        case 0:
            open_dup(get_task_pid(t), O_WRONLY, 0666, 1);
            char *type = get_task_command(t);
            if (strcmp(type, "status") == 0)
            {
                show_queue(pool->queue);
                show_config_status(pool->config);
            }
            else if (strcmp(type, "transform") == 0)
            {
                show_task(t);
            }
            _exit(0);
        default:
            wait(NULL);
            set_task_status(t, FINISHED);
            remove_pid_task(pool->queue, atoi(get_task_pid(t)));
            break;
        }
    }
}

int pool(char *config_file, char *filter_folder)
{
    Pool pool = init_pool();
    load_config(pool, config_file, filter_folder);
    char buffer[BUFFER];
    int bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER)) > 0)
    {
        buffer[bytes_read] = '\0';
        add_task(pool->queue, buffer);
        handle_pool(pool);
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc == 3)
    {
        pool(argv[1], argv[2]);
    }
    return 0;
}
