/**
 * @file queue.c
 * @author Marco Sousa (a62608@alumni.uminho.pt)
 * @brief Módulo para gestão de tarefas sob a forma de queue (FCFS)
 * @version 0.1
 * @date 2021-06-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "queue.h"

struct queue
{
    Task *tasks;
    int current;
    int pending;
    int total;
    int size;
} NQueue;

Queue init_queue()
{
    Queue new = malloc(sizeof(NQueue));
    new->total = 0;
    new->current = 0;
    new->pending = 0;
    new->size = MAX_TASKS;
    new->tasks = malloc(sizeof(Task *) * MAX_TASKS);
    for (int i = 0; i < MAX_TASKS; i++)
    {
        new->tasks[i] = NULL;
    }
    return new;
}

int add_task(Queue q, Task t)
{
    if (q && q->total < q->size)
    {
        int i = (q->current + q->pending) % q->size;
        Task tmp = q->tasks[i];
        for (int j = 0; tmp && j < q->size; j++)
            tmp = q->tasks[++i % q->size];

        //Task t = init_task(request);

        q->tasks[i % q->size] = t;
        q->pending++;
        q->total++;
        return 1;
    }
    return 0;
}

Task get_next_task(Queue q)
{
    if (q && q->pending != 0)
    {
        for (int i = q->current, j = 0, k = 0; j < q->total; i = (i + 1) % q->size, k++)
        {
            Task t = q->tasks[i];
            if (t)
            {
                k++;
                Status ts = get_task_status(t);
                if (ts == PENDING || ts == WAITING)
                {
                    set_task_status(t, PROCESSING);
                    q->current = (i + 1) % q->size;
                    q->pending--;
                    return t;
                }
            }
        }
    }
    return NULL;
}

int set_status_task(Queue q, int pid, Status status)
{
    if (q)
    {
        Task t;
        get_pid_task(q, pid, &t);
        if (t && get_task_status(t) != status)
        {
            set_task_status(t, status);
            switch (status)
            {
            case WAITING:
            case PENDING:
                q->pending++;
                break;
            default:
                break;
            }
            return 1;
        }
    }
    return 0;
}

int get_pid_task(Queue q, int pid, Task *t)
{
    char tmp[30];
    sprintf(tmp, "%d", pid);
    *t = NULL;
    for (int i = 0, j = 0; j < q->total && !(*t); i++)
    {
        char *taskpid = get_task_pid(q->tasks[i % q->size]);
        if (taskpid)
        {
            j++;
            if (strcmp(taskpid, tmp) == 0)
            {
                *t = q->tasks[i % q->size];
                return i % q->size;
            }
        }
    }
    return -1;
}

Task get_executer_task(Queue q, int pid)
{
    Task t = NULL;
    for (int i = 0, j = 0; j < q->total && !t; i++)
    {
        int executer_pid = get_task_executer(q->tasks[i % q->size]);
        if (executer_pid == pid)
        {
            t = q->tasks[i % q->size];
            return t;
        }
    }
    return NULL;
}

int remove_pid_task(Queue q, int pid)
{
    Task t;
    int i = get_pid_task(q, pid, &t);
    if (t)
    {
        q->total--;
        free(q->tasks[i]);
        q->tasks[i] = NULL;
        return 1;
    }
    return 0;
}

int get_pending_tasks(Queue q)
{
    if (q)
    {
        return q->pending;
    }
    return 0;
}

void add_pending_tasks(Queue q)
{
    if (q)
    {
        q->pending++;
    }
}

void show_queue(Queue q)
{
    if (q)
    {
        for (int i = 0; i < q->size; i++)
        {
            if (q->tasks[i])
            {
                char tmp[20];
                sprintf(tmp, "Task #%d: ", i + 1);
                write(STDOUT_FILENO, tmp, strlen(tmp));
                show_task(q->tasks[i]);
                write(STDOUT_FILENO, "\n", 1);
            }
        }
    }
}

/**
 * @brief Debug area
 * 
 */
// int main(int argc, char const *argv[])
// {
//     char teste[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste1[1000] = "5870 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste2[1000] = "5872 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste3[1000] = "5873 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste4[1000] = "5874 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste5[1000] = "5875 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste6[1000] = "5876 transform <input_file> <output_file> filter1 filter2 filter3";
//     Queue q = init_queue();
//     add_task(q, teste);
//     add_task(q, teste1);
//     add_task(q, teste2);
//     add_task(q, teste3);
//     add_task(q, teste4);
//     Task t = get_next_task(q);
//     while (get_next_task(q))
//         ;
//     set_status_task(q, atoi(get_task_pid(t)), WAITING);
//     show_queue(q);
//     printf("\n");
//     remove_pid_task(q, 5872);
//     add_task(q, teste5);
//     t = get_next_task(q);
//     set_status_task(q, 5872, WAITING);
//     t = get_next_task(q);
//     set_status_task(q, 5872, FINISHED);
//     remove_pid_task(q, 5872);

//     add_task(q, teste6);

//     int i = get_pid_task(q, 5876, &t);
//     printf("posição 5876: %d\n", i);
//     //    write(STDOUT_FILENO, "\n", 1);
//     show_queue(q);
//     return 0;
// }
