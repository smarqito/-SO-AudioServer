#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include "pool.h"
#include "queue.h"
#include "dup_aux.h"
#include "config.h"

typedef struct pool *Pool;

#define BUFFER 4096

static Pool POOL;

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

void fork_finished(int signal)
{
    int status;
    int pid = wait(&status);
    if (WEXITSTATUS(status) == 0)
    {
        pid = atoi(get_task_pid(get_executer_task(POOL->queue, pid)));
        set_status_task(POOL->queue, pid, FINISHED);
        remove_pid_task(POOL->queue, pid);
    }
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
    int pid;
    if (t)
    {
        set_task_status(t, PROCESSING);
        switch ((pid = fork()))
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
            kill(getppid(), SIGUSR1);
            _exit(0);
        default:
            set_task_executer(t, pid);
            break;
        }
    }
}

int thread_pool(char *config_file, char *filter_folder)
{
    POOL = init_pool();
    load_config(POOL, config_file, filter_folder);
    char buffer[BUFFER];
    int bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER)) > 0)
    {
        buffer[bytes_read] = '\0';
        add_task(POOL->queue, buffer);
        handle_pool(POOL);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, fork_finished);
    if (argc == 3)
    {
        thread_pool(argv[1], argv[2]);
    }
    return 0;
}
