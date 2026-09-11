/* Nucleo.c */
/* Bibliotecas importantes para o nucleo */
#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Estruturas de dados para indicar a regiao critica do DOS */
typedef struct registros{
        unsigned bx1, es1; /* Campos dos registradores BX e ES */
}regis;

typedef union k{
        regis x;        /* Valor dos registradores */
        char far *y;    /* Ponteiro para a regiao critica do DOS */
}APONTA_REG_CRIT;

APONTA_REG_CRIT a; /* Variavel global para indicar a regiao critica do DOS */

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* Definicao do BCP: DESCRITOR_PROC e PTR_DESC_PROC */

typedef struct desc_p{
        char nome[35];                          /* Nome ou identificacao do processo */
        enum {ativo, bloq_p, terminado} estado; /* Estado do processo */
        PTR_DESC contexto;                      /* Execucao do processo */
        struct desc_p *fila_sem;                /* Ponteiro para a fila de processos bloqueados pelo semaforo */
        struct desc_p *prox_desc;               /* Ponteiro para a lista de processos prontos */
} DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;          /* Ponteiro do descritor do processo */

/* Variaveis globais do descritor do escalonador (d_esc) e da lista de descritores de processo (PRIM)*/
PTR_DESC_PROC PRIM = NULL;
PTR_DESC d_esc;

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* Definicao do semaforo */
typedef struct{
        int s;                  /* Parte inteira do semaforo, que sinaliza o numero de instancias de recursos ou processos que podem estar numa mesma regiao critica */
        PTR_DESC_PROC Q;        /* Fila de processos bloqueados associado ao semaforo */
}semaforo;

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* Função de criar processo */
void far cria_processo(char nome_p[35], void far (*end_proc)()){
        /* Cria um descrior de processo (BCP) dinamicamente (malloc) atribui o ponteiro para p_aux*/
        PTR_DESC_PROC p_aux = (PTR_DESC_PROC) malloc(sizeof(DESCRITOR_PROC)); /* Aloca um espaco de memoria para esse descritor de processo */

        strcpy(p_aux->nome, nome_p);            /* Atribui o parametro nome_p ao campo nome do processo*/
        p_aux->estado = ativo;                  /* Inicializa o estado para ativo */
        p_aux->fila_sem = NULL;                 /* Inicializa a fila do semaforo como NULL */
        p_aux->contexto = cria_desc();          /* Inicializa o contexto do processo */
        newprocess(end_proc, p_aux->contexto);  /* Inicia a estrutura de contexto da co-rotina*/

        /* Insere o descritor de processo no final da lista circular apontado por PRIM 
        Insere o BCP na fila dos processos prontos */
        if (PRIM == NULL){
                /* Se nao tiver nenhum processo na lista: p_aux aponta para ele mesmo e PRIM aponta para p_aux */
                p_aux->prox_desc = p_aux;
                PRIM = p_aux;
        }
        else{
                /* Caso tenha ja tenha descritores na lista:
                o ponteiro p caminha ate o final da lista circular e eh feito o encadeamento */
                PTR_DESC_PROC p;
                p = PRIM;
                while (p->prox_desc != PRIM){
                        p = p->prox_desc;
                }

                p->prox_desc = p_aux;
                p_aux->prox_desc = PRIM;
        }
}

/* Volta dos */
/* Apos o termino de todos os processos ou de um possivel deadlock, o nucleo passa o controle para o DOS */
void far volta_dos(){
        disable();                      /* Desabilita as interrupcoes */
        setvect(8,p_est->int_anterior); /* Restabelece a rotina de interrucao do timer para o vetor de interrupcoes */
        enable();                       /* Habilita as interrucoes */
        exit(0);
}

/* Procura o proximo processo ativo */
/* Um ponteiro percorre a lista circular: se um processo estiver ativo, PRIM aponta para ele, ou seja, sera o proximo a ser escalonado */
/* Se nao tiver nenhum processo ativo, retorna NULL */
PTR_DESC_PROC procura_prox_ativo(){
        PTR_DESC_PROC p = PRIM->prox_desc;

        do{
                if (p->estado == ativo) return p;
                p = p->prox_desc;
        } while (p != PRIM->prox_desc);

        return NULL;
}

/* Escalonador */
/* Despacha o processo para a execução*/
void far escalador(){
        p_est->p_origem = d_esc;                /* A co-rotina chamadora eh a co-rotina do escalonador */
        p_est->p_destino = PRIM->contexto;      /* A co-rotina chamada pelo escalonador eh o contexto PRIM (primeiro BCP da lista) */
        p_est->num_vetor = 8;                   /* Associa o numero 8 do vetor de interrupcao (interrupcao do timer) a co-rotina do escalonador*/

        /* inicia ponteiro para R.C do DOS */
        _AH = 0x34;
        _AL = 0x00;
        geninterrupt(0x21);
        a.x.bx1 = _BX;
        a.x.es1 = _ES;

        while (1){
                iotransfer();
                disable();

                /* Verifica se o processo esta na regiao critica do DOS*/
                /* Se ele esta na região crítica: o escalonador da mais uma fatia de tempo para o processo atual, senao troca o contexto*/
                if (*a.y == 0){
                        PRIM = procura_prox_ativo();
                        if (PRIM == NULL) volta_dos();          
                        p_est->p_destino = PRIM->contexto; /* Atualiza o campo destino */
                }
                enable();
        }

}

/* Disparar o sistema */
/* O controle do DOS eh passado para o escalonador que ira despachar os processos */
void far dispara_sistema(){
        PTR_DESC d_aux;                 
        d_esc = cria_desc();            /* Cria o contexto da co-rotina do escalonador */
        d_aux = cria_desc();            /* Cria um contexto auxiliar para transferir a execucao por meio do transfer */
        newprocess(escalador, d_esc);   /* Cria o descritor do contexto do escalonador */
        transfer(d_aux, d_esc);         /* Tranfere o contexto atual para o escalonador */
}

/* Termina processo */
/* O processo chega ao fim de sua execucao */
void far termina_processo(){
        disable();                /* Desabilita interrupcoes */
        PRIM->estado = terminado; /* Muda o estado do processo para terminado */
        enable();                 /* Habilita as interrupcoes */
        while (1);                /* Loop eterno sem comandos para gastar a fatia de tempo final */
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* Funcoes para o suporte a semaforos */

/* Inicializar o semaforo com algum valor e inicializar a fila Q com NULL */
void far inicializa_semaforo(semaforo *sem, int n){
        sem->s = n;     /* Inicializa a variavel com o total de recursos */
        sem->Q = NULL;  /* Inicializa a fila de processos bloqueados */
}

/* Primitiva P */
/* Se a variavel inteira do semaforo for maior que 0, decrementa o valor dessa variavel */
/* Senao, insere o BCP do processo atual na fila dos bloqueados (bloqueia o processo)*/
void far P(semaforo *sem){
        PTR_DESC_PROC p_aux;
        disable();
        if (sem->s > 0) {
                sem->s--;
                enable();
        }
        else{   
                if (sem->Q == NULL){
                        /* Se a fila Q estiver fazia, a cabeca da fila aponta para o PRIM*/
                        sem->Q = PRIM;
                }
                else{
                        /* Senao, insere o BCP do processo no final dessa fila */
                        PTR_DESC_PROC p;
                        p = sem->Q;
                        while (p->fila_sem){
                                p = p->fila_sem;
                        }

                        p->fila_sem = PRIM;
                }

                /* Eh necessario garantir que o ponteiro final da fila seja NULL e que o estado do processo seja bloqueado (bloq_p)*/
                PRIM->fila_sem = NULL;
                PRIM->estado = bloq_p;

                /* Procura o proximo processo ativo */
                p_aux = PRIM;
                PRIM = procura_prox_ativo();

                /* Se nao tiver mais processos ativos, eh identificado um possivel deadlock e volta o controle para o DOS*/
                if (PRIM == NULL) {
                        enable();
                        printf("\nDEADLOCK DETECTADO\n");
                        volta_dos();
                }

                /* Caso contrario, transfere a execucao do processo bloqueado para o processo ativo */
                enable();
                transfer(p_aux->contexto, PRIM->contexto);
        }
}

/* Primitiva V */
/* Se a fila Q do semaforo estiver vazia, incrementa a variavel s */
/* Senao, remove o primeiro processo da fila dos bloqueados (desbloqueia o primeiro processo) */
void far V(semaforo *sem){
        PTR_DESC_PROC p;
        disable();
        if (sem->Q == NULL) {
                sem->s++;
        }
        else{
                /* Altera o estado do processo apontado por sem->Q para ativo e move a cabeca da fila para o proximo elemento (retirar o primeiro elemento de uma lista) */
                sem->Q->estado = ativo;
                p = sem->Q;
                sem->Q = sem->Q->fila_sem;
                p->fila_sem = NULL;

        }
        enable();
}

