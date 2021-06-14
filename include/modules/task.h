#ifndef TASK_H
#define TASK_H

typedef struct task *Task;

/**
 * @brief Inicializa uma tarefa
 * 
 * @param request Comando com a tarefa completa
 * @return Task Tarefa inicializada
 */
Task init_task(char *request);

/**
 * @brief Retorna o path para o ficheiro de input
 * 
 * @param t Tarefa
 * @return char* path do ficheiro de input
 */
char *get_input_file(Task t);

/**
 * @brief Retorna o path para o ficheiro de destino
 * 
 * @param t Tarefa
 * @return char* Path de ficheiro de output
 */
char *get_output_file(Task t);

/**
 * @brief Retorna o filtro que aguarda processamento
 * 
 * @param t Tarefa
 * @return char* Filto a aguardar processamento
 */
char *get_current_filter(Task t);

/**
 * @brief Retorna o próximo filtro a processar (igual ao atual mas incrementa)
 * 
 * @param t Tarefa
 * @return char* Nome do filtro
 */
char *get_next_filter(Task t);

/**
 * @brief Print da tarefa
 * 
 * @param t Tarefa a imprimir
 */
void show_task(Task t);

#endif