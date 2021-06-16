/**
 * @file task.c
 * @author Marco Sousa (a62608@alumni.uminho.pt)
 * @brief Módulo para apoio na gestão de tarefas
 * @version 0.1
 * @date 2021-06-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "task.h"
#include "dup_aux.h"

typedef struct filters
{
    char **filters; // filtros com a ordem pedida pelo utilizador
    int size;       // size of array
    int num_filters;
    int current_filter;
    char **filters_set; // set de filtros (sem repetições)
    int *filters_count; // lista de contadores (relacionado com filtro)
    int num_filters_set;
} * Filters, FNode;

struct task
{
    char *request;
    char *command;
    char *pid;
    int client_fd;
    ExecuteType execution;
    int executer_pid;
    Status status;
    char *input_file;
    char *output_file;
    Filters filters;
} TNode;

Filters init_filters()
{
    Filters f = malloc(sizeof(FNode));
    f->size = 1;
    f->num_filters = 0;
    f->current_filter = 0;
    f->filters = malloc(sizeof(char *));
    f->filters[0] = NULL;
    return f;
}

void resizeFilters(Filters f)
{
    char **new_filters = malloc(sizeof(char *) * f->size * 2);
    char **old_filters = f->filters;
    int i;
    for (i = 0; i < f->size; i++)
    {
        new_filters[i] = old_filters[i];
    }
    for (; i < (f->size * 2); i++)
    {
        new_filters[i] = NULL;
    }
    f->filters = new_filters;
    f->size *= 2;
    free(old_filters);
}

void add_task_filter(Filters f, char *filter)
{
    if (f->num_filters == f->size)
    {
        resizeFilters(f);
    }
    f->filters[f->num_filters++] = strdup(filter);
}

void set_task_filter_counter(Filters f)
{
    int j, k, i;
    int num_filters = f->num_filters;
    char **filters_set = malloc(sizeof(char *) * num_filters);
    int *filters_counter = malloc(sizeof(int) * num_filters);
    for (i = 0, j = 0; i < num_filters; i++)
    {
        for (k = 0; k < j && strcmp(filters_set[k], f->filters[i]) != 0; k++)
            ;

        if (k == j)
        {
            filters_set[k] = f->filters[i];
            filters_counter[k] = 1;
            j++;
        }
        else
        {
            filters_counter[k]++;
        }
    }
    f->filters_set = filters_set;
    f->filters_count = filters_counter;
    f->num_filters_set = j;
}

char **get_task_filter_set(Task t)
{
    if (t)
    {
        return t->filters->filters_set;
    }
    return NULL;
}

int *get_task_filter_counter(Task t)
{
    if (t)
    {
        return t->filters->filters_count;
    }
    return NULL;
}

int get_task_filter_size(Task t)
{
    if (t)
    {
        return t->filters->num_filters_set;
    }
    return 0;
}

Task init_task(char *request)
{
    Task new = malloc(sizeof(TNode));
    new->filters = init_filters();
    new->request = strdup(request);
    new->status = PENDING;
    int i;
    char *str, *token;
    for (i = 0, str = request;; i++, str = NULL)
    {
        token = strtok(str, " ");
        if (token == NULL)
            break;
        switch (i)
        {
        case 0:
            new->pid = strdup(token);
            break;
        case 1:
            new->command = strdup(token);

            break;
        case 2:
            if (!check_file_exists(token))
            {
                new->input_file = NULL;
            }
            else
            {
                new->input_file = strdup(token);
            }
            break;
        case 3:
            new->output_file = strdup(token);
            break;
        default:
            add_task_filter(new->filters, token);
            break;
        }
    }

    set_task_filter_counter(new->filters);

    return new;
}

ExecuteType get_task_execution_type(Task t)
{
    return t->execution;
}

void set_task_execution_type(Task t, ExecuteType type)
{
    if (t)
    {
        t->execution = type;
    }
}

int get_task_executer(Task t)
{
    if (t)
    {
        return t->executer_pid;
    }
    return 0;
}

void set_task_executer(Task t, int pid)
{
    if (t)
    {
        t->executer_pid = pid;
    }
}

char *get_task_command(Task t)
{
    if (t)
    {
        return strdup(t->command);
    }
    return NULL;
}

char *get_task_pid(Task t)
{
    if (t)
    {
        return strdup(t->pid);
    }
    return NULL;
}

char *get_input_file(Task t)
{
    if (t && t->input_file)
    {
        return strdup(t->input_file);
    }
    return NULL;
}

char *get_output_file(Task t)
{
    if (t)
    {
        return strdup(t->output_file);
    }
    return NULL;
}

int get_current_filter(Task t, char **current)
{
    if (t)
    {
        Filters f = t->filters;
        if (f->current_filter != f->num_filters)
        {
            (*current) = strdup(f->filters[f->current_filter]);
            return f->current_filter;
        }
    }
    return -1;
}

int get_next_filter(Task t, char **next)
{
    if (t)
    {
        Filters f = t->filters;
        if (f->current_filter != f->num_filters)
        {
            (*next) = strdup(f->filters[f->current_filter++]);
            return f->current_filter - 1;
        }
    }
    return -1;
}

int get_previous_filter(Task t, char **next)
{
    if (t)
    {
        Filters f = t->filters;
        if ((f->current_filter - 1) >= 0)
        {
            (*next) = strdup(f->filters[f->current_filter - 1]);
            return f->current_filter - 1;
        }
    }
    return -1;
}

void increment_filter(Task t)
{
    if (t)
    {
        Filters f = t->filters;
        f->current_filter++;
    }
}

int get_task_total_filters(Task t)
{
    if (t)
    {
        return t->filters->num_filters;
    }
    return 0;
}

void open_task_client_fd(Task t)
{
    if (t)
    {
        t->client_fd = open(t->pid, O_WRONLY);
    }
}

int get_task_client_fd(Task t)
{
    if (t)
    {
        return t->client_fd;
    }
}

char **get_task_filters(Task t)
{
    if (t)
    {
        return t->filters->filters;
    }
    return NULL;
}

Status get_task_status(Task t)
{
    if (t)
    {
        return t->status;
    }
    return ERROR;
}

void set_task_status(Task t, Status status)
{
    if (t)
    {
        t->status = status;
    }
}

void show_filters(Filters f)
{
    for (int i = 0; i < f->num_filters; i++)
    {
        write(STDOUT_FILENO, f->filters[i], strlen(f->filters[i]));
    }
    write(STDOUT_FILENO, "\n", 1);
}

void show_task(Task t)
{
    if (t)
    {
        char tmp[50];
        int pid_size = strlen(get_task_pid(t)) + 1;
        write(STDOUT_FILENO, t->request + pid_size, strlen(t->request + pid_size));
        switch (get_task_status(t))
        {
        case PENDING:
            sprintf(tmp, " %s", "PENDING");
            break;
        case WAITING:
            sprintf(tmp, " %s", "WAITING");
            break;
        case PROCESSING:
            sprintf(tmp, " %s", "PROCESSING");
            break;
        case FINISHED:
            sprintf(tmp, " %s", "FINISHED");
            break;

        default:
            break;
        }
        write(STDOUT_FILENO, tmp, strlen(tmp));

        // fflush(NULL);
        // printf("pid: %d; input: %s; output: %s; filters: ",atoi(t->pid), get_input_file(t), get_output_file(t));
        // show_filters(t->filters);
    }
}

/**
 * @brief Debug area
 * 
 */
// int main(int argc, char const *argv[])
// {
//     char teste[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3 filter1";
//     Task t = init_task(teste);
//     show_task(t);
//     char *task_t;
//     for (int j = get_next_filter(t, &task_t); j >= 0; j = get_next_filter(t, &task_t))
//     {
//         printf("indice atual: %d\nfiltro: %s\n", j, task_t);
//     }

//     return 0;
// }
