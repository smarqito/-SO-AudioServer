#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Estrutura a ser utilizada para armazenar a configuração do servidor, nomeadamente
 *        <nome filtro>
 *        <nome_ficheiro_execução>
 *        <instâncias_em_execução>
 *        <maximo_instâncias>
 * 
 */
typedef struct config_server *Config_Server;

/**
 * @brief Inicializa uma configuração de servidor com espaço para 2 filtros
 * 
 * @return Config_Server 
 */
Config_Server init_config_server();

/**
 * @brief Devolve a pasta dos filtros
 * 
 * @param cs Configuração do servidor
 * @return char* Path dos filtros
 */
char *get_filters_folder(Config_Server cs);

/**
 * @brief Define a path para os filtros
 * 
 * @param cs Configuração do servidor
 * @param file Path da folder dos filtros
 */
void set_filters_folder(Config_Server cs, char *file);

/**
 * @brief Verifica se um filtro existe na configuração do server
 * 
 * @param cs Config_Server
 * @param filter Filtro a procurar
 * @return int 1 se encontrar, 0 caso contrário
 */
int has_filter(Config_Server cs, char *filter);

/**
 * @brief Retorna o máximo de processos em execução para um determinado filtro
 * 
 * @param cs Configuração do server
 * @param filter Filtro a procurar
 * @return int Número máximo de processos em execução (0 se não existir filtro)
 */
int get_max_filter(Config_Server cs, char *filter);

/**
 * @brief Adiciona uma nova instância em execução
 * 
 * @param cs Configuração do servidor
 * @param filter Filtro
 * @return int 1 caso num processos em excução < maximo permitido
 */
int add_inuse_process(Config_Server cs, char *filter);

/**
 * @brief Retorna o número de processos que estão em execução para um determinado filtro
 * 
 * @param cs Configração do servidor
 * @param filter Filtro
 * @return int >= 0 se processo existir; -1 caso contrário
 */
int get_inuse_filter(Config_Server cs, char *filter);

/**
 * @brief Adiciona um novo filtro, utilizando uma linha com formato:
 *        <nome_filtro> <ficheiro> <max_isntâncias>
 * 
 * @param cs Configuração do servidor
 * @param line 
 * @return int 
 */
int add_filter(Config_Server cs, char *line);

/**
 * @brief Efetua parsing de um ficheiro de configuração, em que cada linha tem o formato:
 *        <nome_filtro> <ficheiro> <max_isntâncias>
 * 
 * @param fd Descritor do ficheiro a ser efetuado parsing
 * @param cs Configuração do servidor
 * @return int Número de filtros adicionados
 */
int parseConfigLines(int fd, Config_Server cs);

/**
 * @brief Retorna o ficheiro de execução associado ao filtro
 * 
 * @param cs Coniguração do servidor
 * @param filter Filtro
 * @return char* Nome do ficheiro a ser utilizado no exec
 */
char *get_filter_file(Config_Server cs, char *filter);

/**
 * @brief Mostra o estado da configuração do servidor
 * 
 * @param c 
 */
void show_config_status(Config_Server cs);

#endif