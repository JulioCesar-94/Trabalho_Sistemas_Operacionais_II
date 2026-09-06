/* NUCLEO.H */
#ifndef NUCLEO_H
#define NUCLEO_H

typedef struct desc_p {
        char nome[35];
        enum { ativo, terminado } estado;
        PTR_DESC contexto;
        struct desc_p *prox_desc;
} DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

/* Variaveis globais expostas para uso externo */

extern PTR_DESC_PROC PRIM;
extern PTR_DESC d_esc;

/* Prototipos das funcoes importantes do nucleo */
extern void far cria_processo(char nome_p[35], void far (*end_proc)());
extern void far dispara_sistema();
extern void far termina_processo();
extern void far volta_dos();

#endif

