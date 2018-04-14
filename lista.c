#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

void inicia(Lista *lista){
    lista->inicio = NULL;
    lista->fim = NULL;
}

Oferta* busca(char nome[255],Lista *lista){
    Oferta* p = lista->inicio;
    while(p != NULL && strcmp(p->nome,nome) != 0)
        p = p->prox;
    return p;
}

void inserir(char nome[255], int qtd, Lista *lista){
    Oferta *nova = busca(nome,lista);
    if (nova != NULL)
        nova->qtd+=qtd;
    else{
      Oferta *nova = malloc(sizeof(Oferta));
      strcpy (nova->nome,nome);
      nova->qtd = qtd;
      nova->prox = NULL;

      if(lista->inicio == NULL){
          lista->inicio = nova;
          nova->ant = NULL;
      } else {
          lista->fim->prox = nova;
          nova->ant = lista->fim;
      }

      lista->fim = nova;
    }
}

void remover(Oferta *p, Lista *lista){

    if(lista->inicio == p)
        lista->inicio = p->prox;
    else
        p->ant->prox = p->prox;
    if(lista->fim == p)
        lista->fim = p->ant;
    else
        p->prox->ant = p->ant;
    free(p);
    p = NULL;
}

int compra(Oferta *ordem, Oferta *nova, Lista *lista, Lista *ordensCompra){
    if (nova == NULL)
        return 0;
    int ret;

    if(nova->qtd < ordem->qtd){
        ordem->qtd -= nova->qtd;
        ret = nova->qtd;
        nova->qtd = 0;
    }
    else{
        nova->qtd -= ordem->qtd;
        ret = ordem->qtd;
        ordem->qtd = 0;
        remover(ordem,ordensCompra);
    }
    //printf("aaaa\n");

    if(nova->qtd == 0)
        remover(nova,lista);

    printf("comprou %i\n",ret);
    return ret;
}

/*Oferta* novaOferta(Lista *lista){

    Oferta *retorno;
    if(lista->inicio == NULL){
        printf("Buscou nova oferta em lista vazia\n");
        return NULL;
    }
    strcpy(retorno->nome,lista->inicio->nome);
    retorno->qtd = lista->inicio->qtd;
    retorno->prox = NULL;

    return retorno;
}*/
