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
#include "exec_helper.h"

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

int can_execute(Task t)
{
    Config_Server cs = POOL->config;
    char **filtros = get_task_filter_set(t);
    int *counter = get_task_filter_counter(t);
    int num = get_task_filter_size(t);
    int i;
    for (i = 0; i < num && is_filter_available(cs, filtros[i], counter[i]); i++)
        ;

    return i == num;
}

void load_config(Pool p, char *config_file, char *filters_folder)
{
    int fd = open(config_file, O_RDONLY);
    if (fd < 0)
    {
        perror("config_file");
        kill(getppid(), SIGUSR1);
        _exit(1);
    }
    set_filters_folder(p->config, filters_folder);
    parseConfigLines(fd, p->config);
}

int has_available_resources(Config_Server cs, Task t)
{
}

void handle_pool(Pool pool)
{
    Task t = get_next_task(pool->queue);
    int pid;
    int status = 0;
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
                status = exec_full(POOL->config, t);
            }
            kill(getppid(), SIGUSR1);
            _exit(status);
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
    open_dup(POOL_PIPE, O_RDONLY, 0666, STDIN_FILENO);

    char buffer[BUFFER];
    int bytes_read;
    int res;
    char tmp[100];
    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER)) > 0)
    {
        buffer[bytes_read] = '\0';
        Task t = init_task(buffer);
        if (t && get_input_file(t))
        {
            add_task(POOL->queue, t);
            handle_pool(POOL);
        }
        else
        {
            res = open(get_task_pid(t), O_WRONLY);
            sprintf(tmp, "input ou output não existem!\n");

            write(res, tmp, strlen(tmp));
            close(res);
        }
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
