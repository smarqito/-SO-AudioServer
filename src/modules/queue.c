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
#include "queue.h"
#include "task.h"

typedef struct queue *Queue;

struct queue
{
    Task *tasks;
    int current;
    int total;
    int size;
} NQueue;

Queue init_queue()
{
    Queue new = malloc(sizeof(NQueue));
    new->total = 0;
    new->current = 0;
    new->size = 100;
    new->tasks = malloc(sizeof(Task *) * 100);
    for (int i = 0; i < 100; i++)
    {
        new->tasks[i] = NULL;
    }
    return new;
}

int add_task(Queue q, char *request)
{
    if (q && q->total < q->size)
    {
        Task t = init_task(request);
        q->tasks[(q->current + (q->total++)) % q->size] = t;
        return 1;
    }
    return 0;
}

Task get_next_task(Queue q)
{
    if (q && q->total != 0)
    {
        Task t = q->tasks[q->current % q->size];
        q->tasks[q->current++ % q->size] = NULL;
        q->total--;
        return t;
    }
    return NULL;
}

void show_queue(Queue q)
{
    if (q)
    {
        for (int i = 0; i < q->size; i++)
        {
            if (q->tasks[i])
            {
                printf("Task #%d: ", i + 1);
                fflush(NULL);
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
//     char teste1[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste2[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste3[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste4[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste5[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3";
//     char teste6[1000] = "5869 transform <input_file> <output_file> filter1 filter2 filter3";
//     Queue q = init_queue();
//     add_task(q, teste);
//     add_task(q, teste1);
//     add_task(q, teste2);
//     add_task(q, teste3);
//     add_task(q, teste4);
//     add_task(q, teste5);
//     add_task(q, teste6);
//     show_queue(q);
//     show_task(get_next_task(q));
//     write(STDOUT_FILENO, "\n", 1);
//     show_queue(q);
//     return 0;
// }
