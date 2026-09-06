/* Nucleo.c */
/* Bibliotecas de suporte ao nucleo */
#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        enable();
        while (1){
                iotransfer();
                disable();
                PRIM = procura_prox_ativo();
                if (PRIM == NULL) volta_dos();
                p_est->p_destino = PRIM->contexto;
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

/* Testar a implementação inicial do nucleo.c */
void far processo1() {
    int i = 0;
    while (i < 5000) {
        printf("Processo 1\n");
        i++;
    }
    termina_processo();
}

void far processo2() {
    int i = 0;
    while (i < 5000) {
        printf("Processo 2\n");
        i++;
    }
    termina_processo();
}


int main() {
    cria_processo("P1", processo1);
    cria_processo("P2", processo2);
    dispara_sistema();
    return 0;
}

