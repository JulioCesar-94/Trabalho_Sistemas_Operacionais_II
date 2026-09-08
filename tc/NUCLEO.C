/* Nucleo.c */
/* Bibliotecas de suporte ao nucleo */
#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Estruturas de dados para indicar a região crítica do DOS */
typedef struct registros{
        unsigned bx1, es1;
}regis;

typedef union k{
        regis x;
        char far *y;
}APONTA_REG_CRIT;

APONTA_REG_CRIT a; /* Variável global para indicar a região crítica do DOS */

/* Definicao do BCP: DESCRITOR_PROC e PTR_DESC_PROC */

typedef struct desc_p{
        char nome[35];
        enum {ativo, terminado} estado;
        PTR_DESC contexto;
        struct desc_p *prox_desc;
} DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

/* Variaveis globais do descritor do escalonador (d_esc) e da lista de descritores de processo (PRIM)*/
PTR_DESC_PROC PRIM = NULL;
PTR_DESC d_esc;

/* Função de criar processo */
void far cria_processo(char nome_p[35], void far (*end_proc)()){
        /* Cria um descrior de processo (BCP) dinamicamente (malloc) atribui o ponteiro para p_aux*/
        PTR_DESC_PROC p_aux = (PTR_DESC_PROC) malloc(sizeof(DESCRITOR_PROC));

        strcpy(p_aux->nome, nome_p);
        p_aux->estado = ativo;
        p_aux->contexto = cria_desc();
        newprocess(end_proc, p_aux->contexto);

        /* Insere o descritor de processo no final da lista circular apontado por PRIM */
        if (PRIM == NULL){
                p_aux->prox_desc = p_aux;
                PRIM = p_aux;
        }
        else{
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
void far volta_dos(){
        disable();
        fflush(NULL);
        setvect(8,p_est->int_anterior);
        enable();
        exit(0);
}

/* Procura o próximo processo ativo */
PTR_DESC_PROC procura_prox_ativo(){
        PTR_DESC_PROC p = PRIM;

        while (p->prox_desc != PRIM){
                p = p->prox_desc;
                if (p->estado == ativo) return p;
        }

        return NULL;
}

/* Escalonador */
void far escalador(){
        p_est->p_origem = d_esc;
        p_est->p_destino = PRIM->contexto;
        p_est->num_vetor = 8;

        /* inicia ponteiro para R.C do DOS */
        _AH = 0x34;
        _AL = 0x00;
        geninterrupt(0x21);
        a.x.bx1 = _BX;
        a.x.es1 = _ES;

        enable();
        while (1){
                iotransfer();
                disable();

                /* Verifica se o processo está na região crítica */

                /* Se ele está na região crítica: o escalonador dá mais uma fatia de tempo para o processo atual, senão troca o contexto*/
                if (!*a.y){
                        PRIM = procura_prox_ativo();
                        if (PRIM == NULL) volta_dos();
                        p_est->p_destino = PRIM->contexto;
                }
                enable();
        }

}

/* Disparar o sistema */
void far dispara_sistema(){
        PTR_DESC d_aux;
        d_esc = cria_desc();
        d_aux = cria_desc();
        newprocess(escalador, d_esc);
        transfer(d_aux, d_esc);
}

/* Termina processo */
void far termina_processo(){
        disable();
        PRIM->estado = terminado;
        enable();
        while (1);
}

