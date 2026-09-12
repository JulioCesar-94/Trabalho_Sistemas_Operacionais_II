#include <stdio.h>
#include <SYSTEM.H>
#include <NUCLEO.H>

/* Implementacao do problema do produtor e consumidor para testar a sincronizacao e exclusao mutua dos semaforos */

/* Define o tamanho maximo do buffer */
#define MAX 10

/* Define o numero maximo de iteracoes no produtor e consumidor */
#define TEST_SIZE 200


/* Ponteiros para depositar e retirar do buffer */
int in = 0;
int out = 0;

/* Buffer */
int buffer[MAX];

/* Arquivo para a saida dos resultados */
FILE *arquivo;

/* Semaforos a serem utilizados */
semaforo mutex;
semaforo vazio;
semaforo cheio;

/* Processo do produtor */
void far producer(){
    int item = 0;
    int i;
    for(i = 0; i < TEST_SIZE; i++){
        item++;     /* Produz o item */

        P(&vazio);  /* Chama a primitiva P(&vazio) para verificar se tem slots vazios no buffer */
        P(&mutex);  /* Chama P(&mutex) para indicar o inicio da regiao critica e impedir que outros processos acessem o buffer ao mesmo tempo */

        /* Escritra de um valor no buffer */
        buffer[in] = item;
        fprintf(arquivo, "Produtor depositou item de valor %d no slot %d\n", item, in);
        in = (in + 1) % MAX;    /* Avança o ponteiro para o proximo slot vazio (o modulo eh utilizado para garantir a caracteristica circular do buffer) */

        V(&mutex);  /* Chama V(&mutex) para indicar a saida da regiao critica e outro processo pode entrar na regiao critica */
        V(&cheio);  /* Chama V(&cheio) para desbloqueiar o consumidor para retirar o item do buffer */
    }

    fflush(arquivo);
    termina_processo();
}

/* Processo do consumidor */
void far consumer(){
    int item;
    int i;
    for(i = 0; i < TEST_SIZE; i++){
        P(&cheio);  /* Chama P(&cheio) para verificar se ha algum item para retirar no buffer */
        P(&mutex);  /* Chama P(&mutex) para indicar o inicio da regiao critica e impedir que outros processos acessem o buffer ao mesmo tempo*/

        /* Retira um valor do buffer */
        item = buffer[out];
        fprintf(arquivo, "Consumidor remove item %d do slot %d\n", item, out);
        out = (out + 1) % MAX;  /* Avança o ponteiro para o proximo slot cheio */

        V(&mutex);  /* Chama V(&mutex) para indicar a saida da regiao critica e outro processo pode entrar na regiao critica */
        V(&vazio);  /* Chama V(&vazio) para desbloqueiar o produtor, indicando que ha uma celula livre para um item */
    }

    fflush(arquivo);
    termina_processo();
}

int main(){
    /* Abertura do aquivo de resultados */
    arquivo = fopen("Results.txt", "w");
    if (arquivo == NULL){
        printf("Erro em abrir o arquivo\n");
        exit(1);
    }

    /* Inicializacao dos semaforos */
    inicializa_semaforo(&mutex, 1);
    inicializa_semaforo(&vazio, MAX);
    inicializa_semaforo(&cheio, 0);

    /* Cria os processos do produtor e consumidor e passa o controle para o escalonador */
    cria_processo("Cons", consumer);
    cria_processo("Prod", producer);
    dispara_sistema();

    return 0;
}

