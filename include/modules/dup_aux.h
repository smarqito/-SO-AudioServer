#ifndef DUP_AUX_H
#define DUP_AUX_H

#define REQUEST_PIPE "SERVER_REQUEST"
#define CLIENT_PIPE "SERVER_ANSWER"

#define LOG_FILE "logger.log"
#define ERR_FILE "errors.log"
#include <sys/types.h>

/**
 * @brief Create a fifo object
 * 
 */
void create_fifo(char *filename);

/**
 * @brief Abre um ficheiro / FIFO, utilizando as flags
 *        indicadas e o modo. No final, faz dup2 para
 *        o descritor dt
 *        O descritor aberto é fechado no final do dup2
 * 
 * @param name Nome do ficheiro a abrir
 * @param flag Flags com que vai abrir o ficheiro
 * @param mode Modo
 * @param dt Descritor para onde vai ser direcionado
 */
void open_dup(char *name, int flag, mode_t mode, int dt);

#endif