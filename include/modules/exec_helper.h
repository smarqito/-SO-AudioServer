#if !defined(EXEC_HELPER_H)
#define EXEC_HELPER_H

#include "task.h"
#include "config.h"

/**
 * @brief Execução total de uma tarefa, utilizando filhos e redirecionamento
 *        de descritores através de dup2
 * 
 * @param cs Configuração do servidor (para obter os ficheiros e path dos filtros)
 * @param t Tarefa a executar
 * @return int 
 */
int exec_full(Config_Server cs, Task t);

/**
 * @brief Execução parcial de uma tarefa, utilizando ficheiros temporários em tmp/
 *        Obtém o próximo filtro a executar e, através de redirecionamento de descritores,
 *        cria um filho que aplica o filtro atual
 * 
 * @param cs Configuração do servidor
 * @param t Tarefa
 * @return int 
 */
int exec_partial(Config_Server cs, Task t);

#endif // EXEC_HELPER_H
