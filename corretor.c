#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "corretor.h"
#include "utils.h"

void corretor(void *arg){
    Portfolio *port = (Portfolio*)arg;
    int t = port->nThread;
    port->inicio = NULL;
    port->fim = NULL;


    FILE *fp;
    char buff[255], buffer[20];
    int qtd;
    int ret,i;

    strcpy(buff, nomearq);
    strcat(buff, "-");
    sprintf(buffer, "%i", t);
    strcat(buff, buffer);

    Lista ordensCompra;
    inicia(&ordensCompra);

    Lista ordensRegistro;
    inicia(&ordensRegistro);

    fp = fopen(buff,"r");

    ret = fscanf(fp, "%s %i", buff, &qtd);
    while (ret != EOF){
        if(ret == 2){
            inserir(buff, qtd, &ordensCompra);
            inserir(buff, qtd, &ordensRegistro);
            //printf("Inserido %ix %s nas ordens de compra de %ld\n", qtd, buff, t);
        }
        ret = fscanf(fp, "%s %i", buff, &qtd);
    }
    fclose(fp);

    pthread_barrier_wait(&barreira);
    //printf("thread %ld saindo da barreira\n",t);
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
            printf("ERROOOOOOOOOOO\n");
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
                //printf("Vai tentar comprar %ix %s\n",p->qtd,p->nome);
                ret = compra(p,oferta,&ofertas,&ordensCompra);
                if (!ret) printf("Erro ao comprar, thread %i\n",t);
            }

        }
        pthread_mutex_unlock(&mutex);
        if(ret) sched_yield();
    }

    Oferta *pReg = ordensRegistro.inicio;
    Oferta *p;

    while(pReg != NULL){
        Item *item;
        if(port->inicio == NULL)
            port->inicio = item;

        p = busca(pReg->nome,&ordensCompra);
        qtd = pReg->qtd;
        if (p != NULL){
            qtd -= p->qtd;
        }


        item->oferta = pReg;
        printf("aa\n");
        item->demanda = qtd;

        if(port->fim != NULL)
            port->fim->prox = item;
        port->fim = item;

        pReg = pReg->prox;
    }

    pthread_exit(NULL);
}
