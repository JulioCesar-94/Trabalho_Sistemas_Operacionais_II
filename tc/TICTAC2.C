/* TICTAC2.C (TICTAC MODIFICADO) */

#include <stdio.h>
#include <system.h>

/* Descritores utilizados */
PTR_DESC dmain, dtic, dtac;

/* Exercicio 2: Alternar, finitamente, a execucao entre duas corotinas por meio do transfer */

/* Co-rotina tic: imprime 'tic-' e transfere para a co-rotina tac, mas limitado a 100 iteracoes */
void far tic(){
    int i = 0;
    while (i < 100){
        printf("tic-");
        transfer(dtic, dtac);
        i++;
    }
    transfer(dtic, dmain);

}

/* Co-rotina tac: imprime 'tac' e transfere para a co-rotina tic */
void far tac(){
    while (1){
        printf("tac\n");
        transfer(dtac, dtic);
    }
}

int main(){
    dmain = cria_desc();    /* Cria um contexto auxliar para transferir a execucao */
    dtic = cria_desc();     /* Cria o descritor do contexto de tic */
    dtac = cria_desc();     /* Cria o descritor do contexto de tac */
    newprocess(tic, dtic);  /* Cria a co-rotina tic */
    newprocess(tac, dtac);  /* Cria a co-rotina tac */

    transfer(dmain, dtic);  /* Transfere a execucao do main para tic */

    printf("-------FIM-------\n");
}