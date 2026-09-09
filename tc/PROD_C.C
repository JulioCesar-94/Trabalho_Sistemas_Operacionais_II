#include <stdio.h>
#include <SYSTEM.H>
#include <NUCLEO.H>
#include <io.h>
#include <fcntl.h>
#define MAX 10

int in = 0;
int out = 0;

int buffer[MAX];

FILE *arquivo;

semaforo mutex;
semaforo vazio;
semaforo cheio;

void far producer(){
    int item = 0;
    while (1){
        item++;

        P(&vazio);
        P(&mutex);

        buffer[in] = item;
        fprintf(arquivo, "Depositou item de valor %d no slot %d\n", item, in);
        fflush(arquivo);
        in = (in + 1) % MAX;

        V(&mutex);
        V(&cheio);
    }
}

void far consumer(){
    int item;
    while (1){
        P(&cheio);
        P(&mutex);

        item = buffer[out];
        fprintf(arquivo, "Remove item %d do slot %d\n", item, out);
        fflush(arquivo);
        out = (out + 1) % MAX;
        
        V(&mutex);
        V(&vazio);
    }
}

int main(){
    arquivo = fopen("Results.txt", "w");
    if (arquivo == NULL){
        printf("Erro em abrir o arquivo\n");
        exit(1);
    }

    inicializa_semaforo(&mutex, 1);
    inicializa_semaforo(&vazio, MAX);
    inicializa_semaforo(&cheio, 0);

    cria_processo("Cons", consumer);
    cria_processo("Prod", producer);
    dispara_sistema();

    return 0;
}

