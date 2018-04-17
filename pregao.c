#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "corretor.h"

int main(int argc, char *argv[]){

    // Inicializa variáveis condicionais
    ofertasDiponiveis = 0;
    acabou = 0;
    temThreadOlhando = 0;

    FILE *fp;
    char buff[255];
    unsigned int qtd,ret;
    int i,t,qtdI;

    // qtdUltimaAnterior verifica se houveram novas ofertas quando só a quantidade de alguma oferta é alterada
    qtdUltimaAnterior = -1;

    if (argv[1] == NULL || argv[2] == NULL){
        printf("Uso: %s nthr nomearq\n",argv[0]);
        exit(1);
    }

    int nthr = atoi(argv[1]);
    if(nthr < 1){
        printf("ERRO (Abortado): Numero de Threads deve ser maior do que 1 (%i)\n",nthr);
        exit(1);
    }

    strcpy(nomearq,argv[2]);

    pthread_t threads[nthr];
    Portfolio portfolios[nthr];

    // Inicializa semáforos
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexThreads, NULL);
    pthread_cond_init(&c, NULL);
    pthread_cond_init(&cPregao, NULL);

    // Inicializa barreira
    ret = pthread_barrier_init(&barreira, NULL, nthr+1);
    if (ret != 0) {
        perror("erro em pthread_barrier_init()");
        exit(1);
    }

    // Cria Threads (corretores)
    for(t = 0; t < nthr; t++){
        portfolios[t].nThread = t+1;
        ret = pthread_create(&threads[t], NULL, (void *)corretor, (void *)(&portfolios[t]));
        if(ret) printf("Erro ao criar a thread %i\n", t);
    }

    // Espera Threads ficarem prontas
    pthread_barrier_wait(&barreira);

    // Inicializa Listas (Global - modificável; e local - registro)
    Lista registroOfertas;
    inicia(&ofertas);
    inicia(&registroOfertas);

    fp = fopen(nomearq, "r");
    if(fp == NULL){
        printf("ERRO (Abortado): Arquivo inexistente (%s)\n",nomearq);
        exit(1);
    }

    ret = fscanf(fp, "%s %i", buff, &qtdI);
    while (ret != EOF){

        if(ret != 2)
            printf("Warning (Ignorado): %s\nFormato = nomeProduto quantidade\n", nomearq);

        else if(qtdI < 0)
            printf("Warning (Ignorado): Quantidade deve ser positiva (%s -> %i)\n", buff, qtdI);

        else {

            qtd = (unsigned int) qtdI;

            if(strcmp(buff,"#") == 0) // Leu o símbolo #
                msleep(qtd);


            else {
                if(qtd > 0){
                    pthread_mutex_lock(&mutex);
                    while(temThreadOlhando == 1)
                        pthread_cond_wait(&cPregao, &mutex);

                    // Região Crítica
                    inserir(buff, qtd, &ofertas);
                    ofertasDiponiveis++;
                    // Fim Região Crítica

                    pthread_cond_signal(&c);
                    pthread_mutex_unlock(&mutex);
                }

                // Insere no registro sem fazer deslocamentos desnecessários
                inserirNaOrdem(buff, qtd, &registroOfertas);


            }
        }

        ret = fscanf(fp, "%s %i", buff, &qtdI);
    }

    fclose(fp);

    // Avisa Threads que não terão novas ofertas
    pthread_mutex_lock(&mutex);
    acabou = 1;
    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&mutex);

    // Espera Threads e imprime portfólio conforme ficam prontas
    for(t = 0; t < nthr; t++){
        ret = pthread_join(threads[t], NULL);
        if(ret != 0)
            printf("ERRO (Inesperado): Thread %i não deu join\n", t+1);

        printf("Thread %i - Portfolio de itens:\n", t+1);
        if(portfolios[t].inicio == NULL)
            printf("Sem retorno da Thread\n");
        else{
            printf("Item               Quantidade  Demanda\n");
            Item *i = portfolios[t].inicio;
            Item *freeI;
            while(i != NULL){
                printf(fmtport,i->oferta.nome,i->comprado,i->oferta.qtd);
                freeI = i;
                i = i->prox;
                free(freeI);
            }
        }
        printf("\n");
    }

    // Imprime Saldo
    printf("Saldo de itens:\n");
    printf("Item               Quantidade  Ofertado\n");
    Oferta *pReg = registroOfertas.inicio;
    Oferta *p;
    while(pReg != NULL){
        p = busca(pReg->nome, &ofertas);
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

    // Destrói semáforos
    pthread_cond_destroy(&c);
    pthread_cond_destroy(&cPregao);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexThreads);

    return 0;
}
