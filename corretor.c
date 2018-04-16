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
    int qtd,ret,i;

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
    ret = fscanf(fp, "%s %i", buff, &qtd);

    while (ret != EOF){

        if(ret != 2)
            printf("Erro na formatação do arquivo %s\nFormato = nomeProduto quantidade\n", buff);

        else {
            inserir(buff, qtd, &ordensCompra);
            inserir(buff, qtd, ordensRegistro);
        }

        ret = fscanf(fp, "%s %i", buff, &qtd);
    }

    fclose(fp);

    // Espera as outras Threads terminarem de inserir também
    pthread_barrier_wait(&barreira);

    // qtdUltimaAnterior verifica se houveram novas ofertas quando só a quantidade de alguma oferta é alterada
    int olhouTudo = 0, qtdUltimaAnterior = -1;
    Oferta *oferta = NULL;

    // Só executa enquanto há ordens não atendidas e ainda há (potencialmente) ofertas para olhar
    while(ordensCompra.inicio != NULL && (ofertasDiponiveis >= 0  || olhouTudo == 0)){
        ret = 0;

        // Entra na Região exclusiva entre Thread e Pregão.
        // Enquanto a Thread compra, o pregão não pode inserir
        pthread_mutex_lock(&mutex);

        // Espera enquanto já olhou todas as ofertas, mas ainda não acabou
        while (ofertasDiponiveis == 0)
            pthread_cond_wait(&c, &mutex);

        olhouTudo = 0;

        // Entra na Região Crítica das Threads
        // Só uma Thread olha as ofertas por vez
        pthread_mutex_lock(&mutexThreads);
        while(temThreadOlhando == 1)
            pthread_cond_wait(&cThreads, &mutexThreads);
        temThreadOlhando = 1;


        while(ordensCompra.inicio != NULL && !olhouTudo && !ret){

            if(ofertas.inicio == NULL)
                printf("Erro: Pegando oferta de lista vazia\n");

            // Primeira oferta = inicio
            if(oferta == NULL)
                oferta = ofertas.inicio;

            else
                if(oferta->prox == NULL){
                    if(oferta->qtd == qtdUltimaAnterior){
                        olhouTudo = 1;
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
                    int tem = oferta->qtd;
                    ret = compra(p,oferta,&ofertas,&ordensCompra);
                    if (!ret) printf("Erro ao comprar, thread %i\n",t);
                    if(ofertas.inicio == NULL && ofertasDiponiveis == 1)
                      ofertasDiponiveis = 0;
                }
            }

            if(ofertas.fim != NULL)
                qtdUltimaAnterior = ofertas.fim->qtd;
            else qtdUltimaAnterior = -1;

        }

        if(!ret)
          olhouTudo = 1;

        // Se acabou sem oferecer novas ofertas, termina
        if(ofertasDiponiveis == -1){
            pthread_mutex_unlock(&mutex);
            temThreadOlhando = 0;
            pthread_cond_signal(&cThreads);
            pthread_mutex_unlock(&mutexThreads);
            break;
        }

        pthread_mutex_unlock(&mutex);
        temThreadOlhando = 0;
        pthread_cond_signal(&cThreads);
        pthread_mutex_unlock(&mutexThreads);
        sched_yield();

    }

    Oferta *pReg = ordensRegistro->inicio;
    Oferta *p;

    while(pReg != NULL){
        Item *item = (Item*)malloc(sizeof(Item));
        if(port->inicio == NULL)
            port->inicio = item;

        pthread_mutex_lock(&mutexThreads);
        while(temThreadOlhando == 1)
            pthread_cond_wait(&cThreads, &mutexThreads);

        p = busca(pReg->nome,&ordensCompra);

        temThreadOlhando = 0;
        pthread_cond_signal(&cThreads);
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
