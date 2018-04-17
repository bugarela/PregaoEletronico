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
    unsigned int qtd,ret;
    int i, qtdI;

    // Edita o nome do arquivo buscado de acordo com os parametros
    // Ex: nomeArquivo-1
    strcpy(buff, nomearq);
    strcat(buff, "-");
    sprintf(buffer, "%i", t);
    strcat(buff, buffer);

    // Lista local para controle das ordens ainda não atendidas
    Lista ordensCompra;
    inicia(&ordensCompra);

    // Lista retornável pelo portfólio para ser impresso
    Lista *ordensRegistro = (Lista*) malloc(sizeof(Lista));
    inicia(ordensRegistro);

    fp = fopen(buff,"r");
    if(fp == NULL){
        printf("Warning (Ignorado): Arquivo inexistente (%s)\n",buff);
        port->inicio = NULL;
        pthread_barrier_wait(&barreira);
        pthread_exit(NULL);
    }
    ret = fscanf(fp, "%s %i", buff, &qtdI);

    while (ret != EOF){

        if(ret != 2)
            printf("Warning (Ignorado): %s\nFormato = nomeProduto quantidade\n", buff);

        else if(qtdI < 0)
            printf("Warning (Ignorado): Quantidade deve ser positiva (%s -> %i)\n", buff, qtdI);

        else {
            qtd = (unsigned int) qtdI;
            if(qtd > 0)
                inserir(buff, qtd, &ordensCompra);
            inserir(buff, qtd, ordensRegistro);
        }

        ret = fscanf(fp, "%s %i", buff, &qtdI);
    }

    fclose(fp);

    // Espera as outras Threads terminarem de inserir também
    pthread_barrier_wait(&barreira);

    int ofertasOlhadas = 0;
    Oferta *oferta = NULL;

    // Só executa enquanto há ordens não atendidas e ainda há (potencialmente) ofertas para olhar
    while(ordensCompra.inicio != NULL){
        ret = 0;

        // Entra na Região Crítica das Threads
        // Só uma Thread olha as ofertas por vez
        pthread_mutex_lock(&mutexThreads);

        // Entra na Região exclusiva entre Thread e Pregão.
        // Enquanto a Thread compra, o pregão não pode inserir
        pthread_mutex_lock(&mutex);

        // Espera enquanto já olhou todas as ofertas, mas ainda não acabou
        while (ofertasDiponiveis <= ofertasOlhadas && !acabou)
            pthread_cond_wait(&c, &mutex);
        temThreadOlhando = 1;

        // Tenta restaurar o estado anterior (ou, ao menos, algo perto)
        Oferta *p;
        while (oferta != NULL){
            // Vê se oferta ainda está na lista e no mesmo lugar
            p = busca(oferta->nome,&ofertas);
            if(p != oferta)
                oferta = oferta->ant;
            else
                break;
        }

        while(ordensCompra.inicio != NULL && (ofertasOlhadas <= ofertasDiponiveis) && !ret){

            ofertasOlhadas++;

            if(ofertas.inicio == NULL){
                printf("ERRO (Inesperado): Pegando oferta de lista vazia\n");
                exit(1);
            }

            // Primeira oferta = inicio
            if(oferta == NULL)
                oferta = ofertas.inicio;

            else
                if(oferta->prox == NULL){
                    if(oferta->qtd == qtdUltimaAnterior){
                        ofertasOlhadas = ofertasDiponiveis;
                        break;
                    }
                }
                else{
                    oferta = oferta->prox;
                }


            Oferta *p = ordensCompra.inicio;

            while (p != NULL && !ret){
                while(p != NULL && strcmp(oferta->nome,p->nome) != 0)
                    p = p->prox;

                if (p != NULL){
                    unsigned int tem = oferta->qtd;
                    ret = compra(p,oferta,&ofertas,&ordensCompra);
                    if (!ret) printf("Erro ao comprar, thread %i\n",t);
                    else
                        if(ofertasDiponiveis==1 && ret==tem)
                            ofertasDiponiveis = 0;
                }
            }

            if(oferta != NULL)
                qtdUltimaAnterior = oferta->qtd;

        }


        // Se acabou (não virão novas ofertas), e saiu do while porque não achou nada termina
        if(acabou == 1 && !ret){
            temThreadOlhando = 0;
            pthread_cond_signal(&cPregao);
            pthread_mutex_unlock(&mutex);
            pthread_mutex_unlock(&mutexThreads);
            break;
        }
        // Se não, passa a vez para o pregão (preferencialmente) ou alguma outra Thread
        temThreadOlhando = 0;
        pthread_cond_signal(&cPregao);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&mutexThreads);
        sched_yield();
    }

    Oferta *p;
    Oferta *pReg = ordensRegistro->inicio;

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
