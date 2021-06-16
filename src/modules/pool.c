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

void set_inuse_filters(Task t)
{
    char **filters = get_task_filter_set(t);
    int *counter = get_task_filter_counter(t);
    int total = get_task_filter_size(t);

    for (int i = 0; i < total; i++)
    {
        update_inuse_process_size(POOL->config, filters[i], counter[i]);
    }
}

void release_inuse_filters(Task t)
{
    char **filters = get_task_filter_set(t);
    int *counter = get_task_filter_counter(t);
    int total = get_task_filter_size(t);
    for (int i = 0; i < total; i++)
    {
        update_inuse_process_size(POOL->config, filters[i], -counter[i]);
    }
}

int has_available_resources(Config_Server cs, Task t)
{
    return 1;
}

void full_execution(Task t)
{
    int pid, status = 0;
    switch ((pid = fork()))
    {
    case -1:
        perror("fork error");
        break;
    case 0:
        dup2(get_task_client_fd(t), STDOUT_FILENO);
        char *type = get_task_command(t);
        if (strcmp(type, "status") == 0)
        {
            char tmp[40];
            show_queue(POOL->queue);
            show_config_status(POOL->config);
            sprintf(tmp, "pid: %d\n", getppid());
            write(STDOUT_FILENO, tmp, strlen(tmp));
        }
        else if (strcmp(type, "transform") == 0)
        {
            status = exec_full(POOL->config, t);
        }
        kill(getppid(), SIGUSR1);
        _exit(status);
    default:
        set_inuse_filters(t);
        set_task_executer(t, pid);
        break;
    }
}

void partial_execution(Task t, char *filter)
{
    int pid, status = 0;
    switch ((pid = fork()))
    {
    case -1:
        perror("fork error");
        break;
    case 0:
        dup2(get_task_client_fd(t), STDOUT_FILENO);
        status = exec_partial(POOL->config, t);
        kill(getppid(), SIGUSR1);
        _exit(status);
    default:
        increment_filter(t);
        update_inuse_process_size(POOL->config, filter, 1);
        set_task_executer(t, pid);
        break;
    }
}

void handle_pool()
{
    Task t; // = get_next_task(pool->queue);
    int pending_tasks = get_pending_tasks(POOL->queue);
    int keep = 0;
    char *filter;
    for (int i = 0; i < pending_tasks && !keep; i++)
    {
        if ((t = get_next_task(POOL->queue)))
        {
            switch (get_task_execution_type(t))
            {
            case FULL:
                if (can_execute(t))
                {
                    keep++;
                    full_execution(t);
                }
                else
                {
                    set_task_status(t, WAITING);
                    add_pending_tasks(POOL->queue);
                }
                break;
            case PARTIAL:
                get_current_filter(t, &filter);
                if (is_filter_available(POOL->config, filter, 1))
                {
                    keep++;
                    partial_execution(t, filter);
                }
                else
                {
                    set_task_status(t, WAITING);
                    add_pending_tasks(POOL->queue);
                }
                break;
            }
        }
    }
}

int set_type_of_execution(Task t)
{
    char **filters = get_task_filter_set(t);
    int *counter = get_task_filter_counter(t);
    int total = get_task_filter_size(t);

    for (int i = 0; i < total; i++)
    {
        if (get_max_filter(POOL->config, filters[i]) < counter[i])
        {
            set_task_execution_type(t, PARTIAL);
            return 0;
        }
    }
    set_task_execution_type(t, FULL);

    return 1;
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
        if (t)
        {
            if (strcmp(get_task_command(t), "status") == 0 || get_input_file(t))
            {
                set_type_of_execution(t);
                open_task_client_fd(t);
                add_task(POOL->queue, t);
                handle_pool();
            }
            else
            {
                res = open(get_task_pid(t), O_WRONLY);
                sprintf(tmp, "input não existe!\n");
                write(res, tmp, strlen(tmp));
                close(res);
            }
        }
        else
        {
        }
    }

    return 0;
}

/**
 * @brief SIGUSR1 handler
 * 
 * @param signal signal identifier
 */
void fork_finished(int signal)
{
    int status, i;
    int pid = wait(&status);
    char *filter;
    if (WEXITSTATUS(status) == 0)
    {
        Task t = get_executer_task(POOL->queue, pid);
        pid = atoi(get_task_pid(t));
        switch (get_task_execution_type(t))
        {
        case FULL:
            release_inuse_filters(t);
            set_status_task(POOL->queue, pid, FINISHED);
            remove_pid_task(POOL->queue, pid);
            close(get_task_client_fd(t));
            break;
        case PARTIAL:
            i = get_previous_filter(t, &filter);
            if (i >= 0)
            {
                if (i < (get_task_total_filters(t) - 1))
                {
                    set_status_task(POOL->queue, pid, WAITING);
                }
                else
                {
                    set_status_task(POOL->queue, pid, FINISHED);
                    remove_pid_task(POOL->queue, pid);
                    close(get_task_client_fd(t));
                }
                update_inuse_process_size(POOL->config, filter, -1);
            }
            break;
        default:
            break;
        }
        handle_pool();
    }
}

/**
 * @brief Gestão de encerramento do servidor
 * 
 * @param signal 
 */
void close_pool(int signal)
{
    close(STDIN_FILENO); // fecho descritor que mantém thread_pool como listener
    char resp[100];
    while (get_total_tasks(POOL->queue) > 0)
    {
        handle_pool();
        pause();
        sprintf(resp, "Faltam %d tarefas", get_total_tasks(POOL->queue));
    }
    _exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, fork_finished);
    signal(SIGUSR2, close_pool);
    if (argc == 3)
    {
        thread_pool(argv[1], argv[2]);
        return 0;
    }
    else
    {
        return 1;
    }
}
