#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "exec_helper.h"
#include "dup_aux.h"

int exec_full(Config_Server cs, Task t)
{
    if (t)
    {
        int t_f = get_task_total_filters(t);
        char *filter;
        int status;
        int p[t_f - 1][2];
        char resp[100];
        sprintf(resp, "Processing %d filters\n", t_f);
        write(STDOUT_FILENO, resp, strlen(resp));
        for (int i = get_next_filter(t, &filter); i >= 0; i = get_next_filter(t, &filter))
        {
            if (i == 0)
            {
                pipe(p[i]);
                switch (fork())
                {
                case 0:
                    close(p[i][0]);                                            // fecho leitura pipe
                    open_dup(get_input_file(t), O_RDONLY, 0666, STDIN_FILENO); // direciono leitura para ficheiro
                    dup2(p[i][1], STDOUT_FILENO);                              // direciono escrita para pipe
                    close(p[i][1]);                                            // evitar deadlock
                    status = execl(get_filter_path(cs, filter), get_filter_file(cs, filter), NULL);
                    printf("sai com errors!!!!!!\n");
                    _exit(status);
                default:
                    close(p[i][1]);
                    break;
                }
            }
            else if (i == (t_f - 1))
            {
                switch (fork())
                {
                case 0:
                    dup2(p[i - 1][0], STDIN_FILENO);                                                 // direciono leitura para pipe
                    open_dup(get_output_file(t), O_CREAT | O_TRUNC | O_WRONLY, 0666, STDOUT_FILENO); // direciono escrita para ficheiro
                    status = execl(get_filter_path(cs, filter), get_filter_file(cs, filter), NULL);
                    _exit(status);
                default:
                    close(p[i - 1][0]);
                    break;
                }
            }
            else
            {
                pipe(p[i]);
                switch (fork())
                {
                case 0:
                    close(p[i][0]);                  // fecho leitura do pr??prio pipe
                    dup2(p[i - 1][0], STDIN_FILENO); // leitura do pipe anterior
                    close(p[i - 1][0]);              // fecho ap??s direcionar
                    dup2(p[i][1], STDOUT_FILENO);    // direciono escrita para pipe
                    close(p[i][1]);                  // evitar deadlock
                    status = execl(get_filter_path(cs, filter), get_filter_file(cs, filter), NULL);
                    _exit(status);
                default:
                    close(p[i][1]);     // fecho escrita do pipe atual
                    close(p[i - 1][0]); // fecho leitura pipe anterior
                    break;
                }
            }
        }
        for (int i = 0; i < t_f; i++)
        {
            wait(&status);
        }
        sprintf(resp, "Terminated\n");
        write(STDOUT_FILENO, resp, strlen(resp));
        return 0;
    }
    return 1;
}

int exec_partial(Config_Server cs, Task t)
{
    char *filter;
    int i = get_next_filter(t, &filter);
    int N = get_task_total_filters(t);
    char tmp_file[1024];
    char *pid = get_task_pid(t);
    char resp[100];
    sprintf(resp, "Processing filter #%d: %s\n", i + 1, filter);
    write(STDOUT_FILENO, resp, strlen(resp));

    if (i == 0)
    {
        open_dup(get_input_file(t), O_RDONLY, 0666, STDIN_FILENO);
        sprintf(tmp_file, "tmp/%s_%d.tmp", pid, i);
        open_dup(tmp_file, O_CREAT | O_TRUNC | O_WRONLY, 0666, STDOUT_FILENO);
    }
    else if (i == (N - 1))
    {
        sprintf(tmp_file, "tmp/%s_%d.tmp", pid, i - 1);
        open_dup(tmp_file, O_RDONLY, 0666, STDIN_FILENO);
        open_dup(get_output_file(t), O_CREAT | O_TRUNC | O_WRONLY, 0666, STDOUT_FILENO);
    }
    else
    {
        sprintf(tmp_file, "tmp/%s_%d.tmp", pid, i);
        open_dup(tmp_file, O_CREAT | O_TRUNC | O_WRONLY, 0666, STDOUT_FILENO);
        sprintf(tmp_file, "tmp/%s_%d.tmp", pid, i - 1);
        open_dup(tmp_file, O_RDONLY, 0666, STDIN_FILENO);
    }
    
    if (fork() == 0)
    {
        execl(get_filter_path(cs, filter), get_filter_file(cs, filter), NULL);
        _exit(0);
    }

    wait(NULL);
    if (i > 0)
    {
        unlink(tmp_file);
    }
    if (i == (N - 1))
    {
        sprintf(resp, "Terminated\n");
        write(STDOUT_FILENO, resp, strlen(resp));
    }
    return 0;
}