#ifndef READLN
#define READLN
#include <stdlib.h>

/**
 * @brief Lê uma linha a partir do file descriptor indicado e coloca no buffer
 * 
 * @param fd File descriptor de onde irá ler
 * @param buffer Buffer que será preenchido
 * @param buffer_size Tamanho máximo do buffer
 * @return ssize_t Número de carateres lidos
 */
ssize_t readln(int fd, char *buffer, size_t buffer_size);

#endif
