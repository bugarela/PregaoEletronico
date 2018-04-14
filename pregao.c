#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "corretor.h"

int main(int argc, char *argv[]){
    ofertasDiponiveis = 0;
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
    //printf("nthr= %i, nomearq= %s\n\n",nthr, nomearq);

    pthread_t threads[nthr];
    int rc;
    int t;

    rc = pthread_barrier_init(&barreira, NULL, nthr+1);
    if (rc != 0) {
        perror("erro em pthread_barrier_init()");
        exit(1);
    }

    Portfolio portfolios[nthr];
    /* Criar Threads */
    for(t=0;t<nthr;t++){
        portfolios[t].nThread = t+1;
        rc = pthread_create(&threads[t], NULL, (void *)corretor, (void *)(&portfolios[t]));
        if(rc) printf("Erro ao criar a thread %i\n",t);
    }


    pthread_barrier_wait(&barreira);

    Lista registroOfertas;
    inicia(&ofertas);
    inicia(&registroOfertas);

    fp = fopen(nomearq, "r");

    ret = fscanf(fp, "%s %i", buff, &qtd);
    while (ret != EOF){
        if(ret == 2){
            pthread_mutex_lock(&mutex);

            inserir(buff, qtd, &ofertas);

            ofertasDiponiveis++;
            pthread_cond_signal(&c);
            pthread_mutex_unlock(&mutex);

            inserir(buff, qtd, &registroOfertas);
        }
        ret = fscanf(fp, "%s %i", buff, &qtd);
    }

    fclose(fp);
    //printf("Acabou de inserir, pregao esperando...\n");

    for(t=0;t<nthr;t++){
        rc = pthread_join(threads[t],NULL);
        if(rc != 0) printf("Erro no join da thread %i\n", t+1);
        printf("Thread %i - Portfolio de itens:\n",t);
        printf("Item               Quantidade  Demanda\n");
        Item *i = portfolios[t].inicio;
        while(i != NULL){
            printf(fmtport,i->oferta->nome,i->oferta->qtd,i->demanda);
            i = i->prox;
        }
    }

    printf("Saldo de itens:\n");
    printf("Item               Quantidade  Ofertado\n");
    Oferta *pReg = registroOfertas.inicio;
    Oferta *p;
    while(pReg != NULL){
        p = busca(pReg->nome,&ofertas);
        if (p == NULL)
            qtd = 0;
        else{
            qtd = p->qtd;
            free(p);
        }
        printf(fmtsaldo,pReg->nome,qtd,pReg->qtd);
        registroOfertas.inicio = registroOfertas.inicio->prox;
        free(pReg);
        pReg = registroOfertas.inicio;
    }

    pthread_mutex_unlock(&mutex);

    return 0;

}
