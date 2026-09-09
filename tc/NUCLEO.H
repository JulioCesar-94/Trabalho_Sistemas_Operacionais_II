/* NUCLEO.H */
#ifndef NUCLEO_H
#define NUCLEO_H

typedef struct desc_p {
        char nome[35];
        enum {ativo, bloc_p, terminado} estado;
        PTR_DESC contexto;
        struct desc_p *fila_sem;
        struct desc_p *prox_desc;
} DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

/* Variaveis globais expostas para uso externo */

extern PTR_DESC_PROC PRIM;
extern PTR_DESC d_esc;


typedef struct{
        int s;
        PTR_DESC_PROC Q;
}semaforo;

/* Prototipos das funcoes importantes do nucleo */
extern void far cria_processo(char nome_p[35], void far (*end_proc)());
extern void far dispara_sistema();
extern void far termina_processo();
extern void far volta_dos();
extern void far inicializa_semaforo(semaforo *sem, int n);
extern void far P(semaforo *sem);
extern void far V(semaforo *sem);

#endif

