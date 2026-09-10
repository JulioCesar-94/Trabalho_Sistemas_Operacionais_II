#include <stdio.h>
#include <SYSTEM.H>
#include <NUCLEO.H>

/* Um programa simples para testar a implementacao da troca de processos do nucleo.c */

void far processo1() {
     int i = 0;
     while (i < 10000) {
           printf("Processo 1\n");
           i++;
     }
     termina_processo();
}
void far processo2() {
     int i = 0;
     while (i < 10000) {
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