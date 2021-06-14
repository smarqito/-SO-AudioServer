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

typedef struct filters
{
    char **filters;
    int size; // size of array
    int num_filters;
    int current_filter;
} * Filters, FNode;

struct task
{
    char *request;
    char *pid;
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
    char **new = malloc(sizeof(char *) * f->size * 2);
    char **filters = f->filters;
    int i;
    for (i = 0; i < f->size; i++)
    {
        new[i] = filters[i];
    }
    for (; i < (f->size * 2); i++)
    {
        new[i] = NULL;
    }
    f->filters = new;
    f->size *= 2;
    free(filters);
}

void add_task_filter(Filters f, char *filter)
{
    if (f->num_filters == f->size)
    {
        resizeFilters(f);
    }
    f->filters[f->num_filters++] = strdup(filter);
}

Task init_task(char *request)
{
    Task new = malloc(sizeof(TNode));
    new->filters = init_filters();
    new->request = strdup(request);
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
            break;
        case 2:
            new->input_file = strdup(token);
            break;
        case 3:
            new->output_file = strdup(token);
            break;
        default:
            add_task_filter(new->filters, token);
            break;
        }
    }
    return new;
}

char *get_input_file(Task t)
{
    if (t)
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

char *get_current_filter(Task t)
{
    if (t)
    {
        Filters f = t->filters;
        if (f->current_filter != f->num_filters)
        {
            return strdup(f->filters[f->current_filter]);
        }
    }
    return NULL;
}

char *get_next_filter(Task t)
{
    if (t)
    {
        Filters f = t->filters;
        if (f->current_filter != f->num_filters)
        {
            return strdup(f->filters[f->current_filter++]);
        }
    }
    return NULL;
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
        write(STDOUT_FILENO, t->request, strlen(t->request));
        fflush(NULL);
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
//     char teste[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3";
//     Task t = init_task(teste);
//     show_task(t);
//     return 0;
// }
