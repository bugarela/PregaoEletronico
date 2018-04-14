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

    //editar o nome da thread de acordo com os parametros
    strcpy(buff, nomearq);
    strcat(buff, "-");
    sprintf(buffer, "%i", t);
    strcat(buff, buffer);

    Lista ordensCompra;
    inicia(&ordensCompra);

    Lista *ordensRegistro = (Lista*) malloc(sizeof(Lista));
    inicia(ordensRegistro);

    fp = fopen(buff,"r");

    ret = fscanf(fp, "%s %i", buff, &qtd);
    while (ret != EOF){
        if(ret == 2){ // se possui o nome e a qtd
            inserir(buff, qtd, &ordensCompra);
            inserir(buff, qtd, ordensRegistro);
            //printf("Inserido %ix %s nas ordens de compra de %d\n", qtd, buff, t);
        }
        ret = fscanf(fp, "%s %i", buff, &qtd);
    }
    fclose(fp);

    pthread_barrier_wait(&barreira);
    //printf("thread %ld saindo da barreira\n",t);
    int ofertasOlhadas = 0;
    Oferta *oferta = NULL;
    int qtdUltimaAnterior = -1;

    while(ordensCompra.inicio != NULL){ //enquanto houver ordens de compra...
        ret = 0;
        pthread_mutex_lock(&mutexThreads);
        pthread_mutex_lock(&mutex);
        while (ofertasDiponiveis <= ofertasOlhadas && acabou == 0) // olhou todas as ofertas, mas ainda não acabou
            pthread_cond_wait(&c, &mutex);

        //printf("olhadas = %i disponiveis = %i\n acabou=%i",ofertasOlhadas,ofertasDiponiveis,acabou);

        if(ofertasDiponiveis <= ofertasOlhadas && acabou == 1){
            pthread_mutex_unlock(&mutex);
            pthread_mutex_unlock(&mutexThreads);
            break;
        }

        if(ofertas.inicio == NULL)
            printf("Deu mto ruim\n");

        ofertasOlhadas++;
        if(oferta == NULL){ //se for a primeira oferta...
            oferta = ofertas.inicio;
        }
        else
            if(oferta->prox == NULL){
                if(oferta->qtd == qtdUltimaAnterior){
                    printf("Repetindo %i %i\n",oferta->qtd,qtdUltimaAnterior);
                    ofertasOlhadas = ofertasDiponiveis;
                    if (acabou == 0){
                        pthread_mutex_unlock(&mutex);
                        pthread_mutex_unlock(&mutexThreads);
                        continue;
                    } else {
                        pthread_mutex_unlock(&mutex);
                        pthread_mutex_unlock(&mutexThreads);
                        break;
                    }
                }
            }
            else{
                oferta = oferta->prox;
            }

        //rintf ("Pegou a oferta %s\n", oferta->nome);

        Oferta *p = ordensCompra.inicio;

        while (p != NULL && !ret){
            //printf("procurando...\n");
            while(p != NULL && strcmp(oferta->nome,p->nome) != 0)
                p = p->prox;

            if (p != NULL){
                //printf("Vai tentar comprar %ix %s\n",p->qtd,p->nome);
                int tem = oferta->qtd;
                ret = compra(p,oferta,&ofertas,&ordensCompra);
                if (!ret) printf("Erro ao comprar, thread %i\n",t);
                if(ret == tem){
                    ofertasOlhadas--;
                    ofertasDiponiveis--;
                }
                //else
                    //printf("Comprou parcialmente %s\n",oferta->nome);
            }
        }

        if(ofertas.fim != NULL)
            qtdUltimaAnterior = ofertas.fim->qtd;
        else qtdUltimaAnterior = -1;

        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&mutexThreads);
        if(ret) sched_yield();
    }

    Oferta *pReg = ordensRegistro->inicio;
    Oferta *p;

    while(pReg != NULL){
        Item *item = (Item*)malloc(sizeof(Item));
        if(port->inicio == NULL)
            port->inicio = item;

        pthread_mutex_lock(&mutexThreads);
        p = busca(pReg->nome,&ordensCompra);
        pthread_mutex_unlock(&mutexThreads);
        
        qtd = pReg->qtd;
        if (p != NULL){
            qtd -= p->qtd;
        }

        item->comprado = qtd;
        memcpy(&item->oferta,pReg,sizeof(Oferta));

        if(port->fim != NULL)
            port->fim->prox = item;
        port->fim = item;

        pReg = pReg->prox;
    }
    pthread_exit(NULL);
}
