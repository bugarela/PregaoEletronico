#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "corretor.h"

int main(int argc, char *argv[]){
    FILE *fp;
    char buff[255];
    int qtd;
    int ret,i;

    if (argv[1] == NULL || argv[2] == NULL){
        printf("Uso: %s nthr nomearq\n",argv[0]);
        exit(1);
    }

    int nthr = atoi(argv[1]);
    strcpy(nomearq,argv[2]);
    printf("nthr= %i, nomearq= %s\n\n",nthr, nomearq);

    pthread_t threads[nthr];
    int rc;
    long t;

    rc = pthread_barrier_init(&barreira, NULL, nthr+1);
    if (rc != 0) {
        perror("erro em pthread_barrier_init()");
        exit(1);
    }

    /* Criar Threads */
    for(t=0;t<nthr;t++){
        rc = pthread_create(&threads[t], NULL, (void *)corretor, (void *)(t+1));
        if(rc) printf("Erro ao criar a thread %ld\n",t);
    }


    pthread_barrier_wait(&barreira);
    inicia(&ofertas);

    fp = fopen(nomearq, "r");

    ret = fscanf(fp, "%s %i", buff, &qtd);
    while (ret != EOF){
        if(ret ==2){
            inserir(buff, qtd, &ofertas);
            printf("Inserido %ix %s nas ofertas\n", qtd, buff);
        }
        ret = fscanf(fp, "%s %i", buff, &qtd);
    }

    fclose(fp);


    /*while (ofertas.inicio != NULL){
        Oferta atual = pop(&ofertas);
        printf("%ix %s\n", atual.qtd, atual.nome );
    }*/

    for(t=0;t<nthr;t++){
        rc = pthread_join(threads[t],NULL);
        if(!rc) printf("Erro no join da thread %ld\n", t);
    }
    return 0;

}
