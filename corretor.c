#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "corretor.h"
#include "utils.h"

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
        if(ret == 2){
            inserir(buff, qtd, &ordensCompra);
            printf("Inserido %ix %s nas ordens de compra de %ld\n", qtd, buff, t);
        }
        ret = fscanf(fp, "%s %i", buff, &qtd);
    }
    fclose(fp);

    pthread_barrier_wait(&barreira);
    printf("thread %ld saindo da barreira\n",t);
    int ofertasOlhadas = 0;
    Oferta *oferta = NULL;
    while(ordensCompra.inicio != NULL){

        ret = 0;

        pthread_mutex_lock(&mutex);
        while (ofertasDiponiveis == ofertasOlhadas)
            pthread_cond_wait(&c, &mutex);

        //printf ("Pegará uma oferta\n");
        if(ofertasOlhadas == 0)
            oferta = ofertas.inicio;
        else
            oferta = oferta->prox;

        ofertasOlhadas++;

        if(oferta == NULL){
            //printf("break\n");
            pthread_mutex_unlock(&mutex);
            break;
        }
        //printf ("Pegou uma oferta\n");

        Oferta *p = ordensCompra.inicio;

        while (p != NULL && !ret){
            //printf("procurando...\n");

            while(p != NULL && strcmp(oferta->nome,p->nome) != 0)
                p = p->prox;

            if (p != NULL){
                printf("Vai tentar comprar %ix %s\n",p->qtd,p->nome);
                ret = compra(p,oferta,&ofertas,&ordensCompra);
                //if(oferta == NULL) ofertasDiponiveis--;
                if (!ret) printf("Erro ao comprar, thread %ld\n",t);
            }

        }
        //printf("Thread liberando\n");

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}
