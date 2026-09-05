#include <stdio.h>
#include <system.h>
PTR_DESC dmain, dtic, dtac;

void far tic(){
    int i = 0;
    while (i < 100){
        printf("tic-");
        transfer(dtic, dtac);
        i++;
    }
    transfer(dtic, dmain);

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

    printf("-------FIM-------\n");
}