#include <stdio.h>
#include <SYSTEM.H>
#include <NUCLEO.H>
#include <io.h>
#include <fcntl.h>
#define MAX 10
#define TEST_SIZE 200

int in = 0;
int out = 0;

int buffer[MAX];

FILE *arquivo;

semaforo mutex;
semaforo vazio;
semaforo cheio;

void far producer(){
    int item = 0;
    int i;
    for(i = 0; i < TEST_SIZE; i++){
        item++;

        P(&vazio);
        P(&mutex);

        buffer[in] = item;
        fprintf(arquivo, "Produtor depositou item de valor %d no slot %d\n", item, in);
        in = (in + 1) % MAX;

        V(&mutex);
        V(&cheio);
    }

    fflush(arquivo);
    termina_processo();
}

void far consumer(){
    int item;
    int i;
    for(i = 0; i < TEST_SIZE; i++){
        P(&cheio);
        P(&mutex);

        item = buffer[out];
        fprintf(arquivo, "Consumidor remove item %d do slot %d\n", item, out);
        out = (out + 1) % MAX;

        V(&mutex);
        V(&vazio);
    }

    fflush(arquivo);
    fclose(arquivo);
    termina_processo();
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