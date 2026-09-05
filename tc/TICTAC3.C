#include <stdio.h>
#include <stdlib.h>
#include <system.h>
#include <time.h>
PTR_DESC dmain, dtic, dtac, d_escalonador;

/* Variaveis globais que servem como parametros para o iotransfer() */

/* Define a lista circular de nos */
typedef struct reg* no;
struct reg{
     PTR_DESC desc;
     no prox;
};

/* Inicializa a lista circular com NULL */
no lista = NULL;

/* Co-rotina tic */
void far tic(){
     while (1){
          printf("tic\n");
     }
}

/* Co-rotina tac */
void far tac(){
     while (1){
          printf("tac\n");
     }
}

/* Co-rotina do escalador */
void far escalonador(){
     no p = lista;  /* Define um no p inicial */

     p_est->p_origem = d_escalonador; /* Define o descritor do escalonador como a co-rotina chamadora */
     p_est->p_destino = p->desc; /* Define o descritor do no p inicial como co-rotina chamada */
     p_est->num_vetor = 8; /* o numero do vetor de interrupcao que sera assciado com a co-rotina chamadora eh o 8 */

     enable(); /* habilita as interrupcoes */
     while (1){
          iotransfer(); /* o escalonador chama a co-rotina dando uma fatia de tempo para execucao */
          disable(); /* desabilita as interrupcoes */
          p = p->prox; /* o ponteiro da lista passa para a proxima co-rotina */
          p_est->p_destino = p->desc; /* o descritor da proxima co-rotina passa a ser a co-rotina chamada */
          enable(); /* habilita as interrupcoes */
     }
}

int main(){
     no p, q; /* define os dois nos das co-rotinas tic e tac */

     /* criacao e inicializacao dos descritores */
     dmain = cria_desc();
     dtic = cria_desc();
     dtac = cria_desc();
     d_escalonador = cria_desc();
     newprocess(tic, dtic);
     newprocess(tac, dtac);
     newprocess(escalonador, d_escalonador);

     /* aloca memoria para os dois nos */
     p = (no) malloc(sizeof(struct reg));
     q = (no) malloc(sizeof(struct reg));

     /* criacao de uma lista circular com os nos p e q */
     p->desc = dtic;
     q->desc = dtac;
     p->prox = q;
     q->prox = p;
     lista = p;

     /* transfere a execucao para o escalonador */
     transfer(dmain, d_escalonador);
}