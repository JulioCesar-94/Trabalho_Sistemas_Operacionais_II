#include <stdio.h>
#include <system.h>
PTR_DESC dmain, dtic, dtac;

/* Exercicio 1: Alternar a execucao entre duas co-rotinas, infinitamente, por meio do transfer */

void far tic(){
     while (1){
           printf("tic-");
           transfer(dtic, dtac);
     }
}

void far tac(){
     while (1){
           printf("tac\n");
           transfer(dtac, dtic);
    }
}

int main(){
    dmain = cria_desc();
    dtic = cria_desc();
    dtac = cria_desc();
    newprocess(tic, dtic);
    newprocess(tac, dtac);
    transfer(dmain, dtic);
}