#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "corretor.h"


void corretor(void *arg){
    long t = (long)arg;
    FILE *fp;
    char buff[255], buffer[20];
    int qtd;
    int ret,i;

    strcpy(buff, nomearq);
    strcat(buff, "-");
    sprintf(buffer, "%ld", t);
    strcat(buff, buffer);

    Lista ordensCompra;
    inicia(&ordensCompra);

    fp = fopen(buff,"r");

    ret = fscanf(fp, "%s %i", buff, &qtd);
    while (ret != EOF){
        if(ret ==2){
            inserir(buff, qtd, &ordensCompra);
            printf("Inserido %ix %s nas ordens de compra de %ld\n", qtd, buff, t);
        }
        ret = fscanf(fp, "%s %i", buff, &qtd);
    }

    fclose(fp);
}
