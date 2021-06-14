/**
 * @file config.c
 * @author Marco Sousa (a62608@alumni.uminho.pt)
 * @brief Módulo para gestão da configuração do servidor
 * @version 0.1
 * @date 2021-06-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "readln.h"
#include "config.h"

#define BUFFER_SIZE 4096

/**
 * @brief Configuração do servidor
 * 
 */
typedef struct config *Config;

/**
 * @brief Estrutura para armazenar a configuração de cada filtro
 * 
 */
struct config
{
    char *filter;
    char *file_name;
    int current, max;
} CNode;

/**
 * @brief Estrutura para armazenar a configuração do servidor
 * 
 */
struct config_server
{
    Config *config;
    char *configFolder;
    int total;
    int size;
} CSNode;

Config_Server init_config_server()
{
    Config_Server cs = malloc(sizeof(CSNode));
    cs->config = malloc(sizeof(Config) * 2);
    cs->size = 2;
    cs->total = 0;
    return cs;
}

/**
 * @brief Inicialização de uma estrutura na heap através de uma linha com a configuração
 * 
 * 
 * @param line <nome_filtro> <ficheiro> <max_isntâncias>
 * @return Config Configuração de um filtro
 */
Config init_config(char *line)
{
    char tmp[4][50];
    char *token, *helper;
    Config tmp_c = malloc(sizeof(CNode));
    int i;
    for (i = 0, helper = line;; i++, helper = NULL)
    {
        token = strtok(helper, " \n");
        if (token == NULL)
        {
            break;
        }
        strcpy(tmp[i], token);
    }
    tmp_c->filter = strdup(tmp[0]);
    tmp_c->file_name = strdup(tmp[1]);
    tmp_c->current = 0;
    tmp_c->max = atoi(tmp[2]);
    return tmp_c;
}

char *get_filters_folder(Config_Server cs)
{
    if (cs)
    {
        return strdup(cs->configFolder);
    }
    return NULL;
}

void set_filters_folder(Config_Server cs, char *file)
{
    if (cs)
    {
        cs->configFolder = strdup(file);
    }
}

int has_filter(Config_Server cs, char *filter)
{
    if (cs)
    {
        Config *list = cs->config;
        for (int i = 0; i < cs->total; i++)
        {
            if (strcmp((list[i])->filter, filter) == 0)
                return 1;
        }
    }
    return 0;
}

/**
 * @brief Retorna uma configuração do filtro, caso exista
 * 
 * @param cs Configuração do server
 * @param filter Filtro a procurar
 * @return Config Configuração encontrada ou NULL
 */
Config get_filter(Config_Server cs, char *filter)
{
    if (cs)
    {
        Config *list = cs->config;
        for (int i = 0; i < cs->size; i++)
        {
            if (strcmp((list[i])->filter, filter) == 0)
                return (list[i]);
        }
    }
    return NULL;
}

int get_max_filter(Config_Server cs, char *filter)
{
    Config result = get_filter(cs, filter);
    if (result != NULL)
    {
        return result->max;
    }
    return 0;
}

int add_inuse_process(Config_Server cs, char *filter)
{
    if (has_filter(cs, filter))
    {
        Config use = get_filter(cs, filter);
        if (use->max > use->current)
        {
            use->current++;
            return 1;
        }
    }
    return 0;
}

int get_inuse_filter(Config_Server cs, char *filter)
{
    if (cs)
    {
        Config result = get_filter(cs, filter);
        if (result != NULL)
        {
            return result->current;
        }
    }
    return -1;
}

/**
 * @brief Redimensiona a estrutura de configuração do servidor para o dobro do espaço
 * 
 * @param cs Configuração do servidor
 */
void resize_config(Config_Server cs)
{
    if (cs)
    {
        Config *tmp = malloc(cs->size * 2 * sizeof(CNode));
        Config *old = cs->config;
        for (int i = 0; i < cs->size; i++)
        {
            tmp[i] = old[i];
        }
        cs->size *= 2;
        cs->config = tmp;
        free(old);
    }
}

int add_filter(Config_Server cs, char *line)
{
    Config tmp = init_config(line);
    if (cs)
    {
        if (!has_filter(cs, tmp->filter))
        {
            if (cs->size == cs->total)
            {
                resize_config(cs);
            }
            cs->config[cs->total++] = tmp;
            return 1;
        }
    }
    return 0;
}

int parseConfigLines(int fd, Config_Server cs)
{
    char buffer[BUFFER_SIZE];
    int i = 0;
    if (cs)
    {
        while (readln(fd, buffer, BUFFER_SIZE) > 0)
        {
            i++;
            add_filter(cs, buffer);
        }
    }
    return i;
}

char *get_filter_file(Config_Server cs, char *filter)
{
    if (cs)
    {
        if (has_filter(cs, filter))
        {
            return get_filter(cs, filter)->file_name;
        }
    }
    return NULL;
}

void show_config(Config c)
{
    if (c)
    {
        printf("%s %s: %d/%d (current/max)\n", c->filter, c->file_name, c->current, c->max);
    }
}

void show_config_status(Config_Server cs)
{
    if (cs)
    {
        char tmp[200];
        sprintf(tmp, "path filtros: %s\n", get_filters_folder(cs));
        write(STDOUT_FILENO, tmp, strlen(tmp));

        for (int i = 0; i < cs->total; i++)
        {
            show_config(cs->config[i]);
        }
    }
}

/**
 * @brief Debug area
 * 
 */
// int main(int argc, char const *argv[])
// {
//     int config;
//     Config_Server cs = init_config_server();
//     int k ;
//     if ((config = open("aurrasd.conf", O_RDONLY)) < 0)
//     {
//         perror("config file");
//     }
//     else
//     {
//         k = parseConfigLines(config, cs);
//     }
//     for (int i = 0; i < cs->total; i++)
//     {
//         show_config(cs->config[i]);
//     }
//     add_inuse_process(cs, "alto");
//     printf("alto in use: %d\n", get_current_filter(cs, "alto"));
//     add_inuse_process(cs, "alto");
//     printf("alto in use: %d\n", get_current_filter(cs, "alto"));
//     add_inuse_process(cs, "alto");
//     printf("alto in use: %d\n", get_current_filter(cs, "alto"));
//     return 0;
// }
