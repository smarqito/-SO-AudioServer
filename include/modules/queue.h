#if !defined(QUEUE_H)
#define QUEUE_H
#include "task.h"

#define MAX_TASKS 100

typedef struct queue *Queue;

/**
 * @brief Inicializa uma queue com espaço para #MAX_TASKS tasks
 * 
 * @return Queue queue inicializada
 */
Queue init_queue();

/**
 * @brief Adiciona uma task no próximo local disponível
 *        incrementa o #tarefas pendentes
 *        incrementa total tarefas
 * 
 * @param q Queue
 * @param request <pid> <type> <input> <output> [filters]
 * @return int 0 se não foi possível inserir: queue cheia ou não inicializada
 *         1 caso contrário
 */
int add_task(Queue q, char *request);

/**
 * @brief Retorna a próxima tarefa a ser executada
 *        define o estado da task para PROCESSING
 *        decrementa os pendentes
 *        coloca apontador para próxima tarefa
 * @param q 
 * @return Task 
 */
Task get_next_task(Queue q);

/**
 * @brief Define o estado de uma task a partir do seu pid
 *        se a task já tinha o estado a alterar, não faz nada
 *        se alterar para WAITING ou PENDING, incrementa pending da queue
 * 
 * @param q Queue
 * @param pid Pid da task a ser alterar
 * @param status Estado a definir
 * @return int 1 se fez alteração
 *             0 caso contrário
 */
int set_status_task(Queue q, int pid, Status status);

/**
 * @brief Coloca no apontador recebido por argumento, a task procurada por pid
 * 
 * @param q Queue
 * @param pid Pid da task a procurar
 * @param t Apontador onde vai ser 'colocada' a task, caso exista (ou NULL)
 * @return int índice onde se encontra a tarefa
 */
int get_pid_task(Queue q, int pid, Task *t);

/**
 * @brief Remove uma tarefa a partir do pid
 *        decrementa o total de tarefas
 *        APENAS REMOVE SE A TAREFA TIVER ESTADO FINISHED
 * 
 * @param q Queue
 * @param pid pid da task a remover
 * @return int 1 se foi possível remover
 *             0 caso contrário
 */
int remove_pid_task(Queue q, int pid);

/**
 * @brief Apresenta a queue
 * 
 * @param q Queue a ser apresentada
 */
void show_queue(Queue q);

#endif // QUEUE_H
