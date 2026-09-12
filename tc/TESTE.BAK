/* TESTE.C */
#include <stdio.h>
#include <SYSTEM.H>
#include <NUCLEO.H>

/* Um programa simples para testar a implementacao da troca de processos do nucleo.c */

/* Processo 1 */
void far processo1() {
     int i = 0;
     while (i < 10000) {
           printf("Processo 1\n");
           i++;
     }
     termina_processo();
}

/* Processo 2 */
void far processo2() {
     int i = 0;
     while (i < 10000) {
           printf("Processo 2\n");
           i++;
     }
     termina_processo();
}

int main() {

      /* Cria os processos 1 e 2 e dispara o sistema para que o escalonador despache os processos para a execucao */
    cria_processo("P1", processo1);
    cria_processo("P2", processo2);
    dispara_sistema();

    return 0;
}

