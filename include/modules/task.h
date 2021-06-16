#ifndef TASK_H
#define TASK_H

typedef struct task *Task;

typedef enum taskstatus
{
    PENDING,
    WAITING,
    PROCESSING,
    FINISHED,
    ERROR
} Status;

typedef enum execute_type
{
    FULL,
    PARTIAL
} ExecuteType;

/**
 * @brief Inicializa uma tarefa
 * 
 * @param request Comando com a tarefa completa
 * @return Task Tarefa inicializada
 */
Task init_task(char *request);

/**
 * @brief Retorna o set de filtros unitários
 * 
 * @param t Tarefa
 * @return char** Set de filtros isolados
 */
char **get_task_filter_set(Task t);

/**
 * @brief Retorna o contador de filtros necessários
 * 
 * @param t Tarefa
 * @return int* Lista de contador
 */
int *get_task_filter_counter(Task t);

/**
 * @brief Retorna o tamanho do set de filtros
 * 
 * @param t Tarefa
 * @return int Tamanho do set de filtros
 */
int get_task_filter_size(Task t);

/**
 * @brief Retorna o pid do cliente que pediu a tarefa
 * 
 * @param t Tarefa
 * @return char* Pid
 */
char *get_task_pid(Task t);

/**
 * @brief Retorna o tipo de execução da tarefa
 * 
 * @param t Tarefa
 * @return ExecuteType Tipo de execução
 */
ExecuteType get_task_execution_type(Task t);

/**
 * @brief Define o tipo de execução da tarefa
 * 
 * @param t Tarefa
 * @param type Tipo de execução
 */
void set_task_execution_type(Task t, ExecuteType type);

/**
 * @brief Retorna o pid do filho que está a executar a tarefa
 * 
 * @param t Task
 * @return int Pid do filho a executar
 */
int get_task_executer(Task t);

void set_task_executer(Task t, int pid);

/**
 * @brief Retorna o comando a ser executado: [status, transform]
 * 
 * @param t Task
 * @return char* Comando, caso esteja definido!
 */
char *get_task_command(Task t);

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
 * @param current Onde vai ser armazenado o próximo filtro a executar
 * @return índice do filtro em processamento / aguarda
 */
int get_current_filter(Task t, char **current);

/**
 * @brief Retorna o próximo filtro a processar (igual ao current mas incrementa)
 * 
 * @param t Tarefa
 * @param next Onde vai ser armazenado o próximo filtro a executar
 * @return indíce do filtro que inicia processamento
 */
int get_next_filter(Task t, char **next);

/**
 * @brief Get the previous filter object
 * 
 * @param t 
 * @param next 
 * @return int 
 */
int get_previous_filter(Task t, char **next);

/**
 * @brief Incrementa o apontador para o próximo filtro
 * 
 * @param t 
 */
void increment_filter(Task t);

/**
 * @brief Retorna o estado de uma tarefa
 * 
 * @param t Tarefa
 * @return Status Estado
 */
Status get_task_status(Task t);

/**
 * @brief Define o estado de uma tarefa
 * 
 * @param t Tarefa
 * @param status Estado
 */
void set_task_status(Task t, Status status);

/**
 * @brief Retorna o total de filtros a serem aplicados
 * 
 * @param t 
 * @return int 
 */
int get_task_total_filters(Task t);

/**
 * @brief Abre o descritor para o fifo do cliente
 * 
 * @param t 
 */
void open_task_client_fd(Task t);

/**
 * @brief Get the task client fd
 * 
 * @param t 
 * @return int 
 */
int get_task_client_fd(Task t);


char **get_task_filters(Task t);

/**
 * @brief Print da tarefa
 * 
 * @param t Tarefa a imprimir
 */
void show_task(Task t);

#endif