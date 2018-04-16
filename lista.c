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
    if (nova != NULL){ // Já existe na lista
        qtd+=nova->qtd;
        remover(nova,lista);
        inserir(nome,qtd,lista);
    }
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
    if(lista->fim == p)
        lista->fim = p->ant;
    if(p->ant != NULL)
        p->ant->prox = p->prox;
    if(p->prox != NULL)
        p->prox->ant = p->ant;

    p = NULL;
}

int compra(Oferta *ordem, Oferta *nova, Lista *lista, Lista *ordensCompra){
    if (nova == NULL)
        return 0;
    int ret;
    if(nova->qtd < ordem->qtd){
        ordem->qtd -= nova->qtd;
        ret = nova->qtd;
        remover(nova,lista);
    }
    else if (nova->qtd > ordem->qtd){
        ret = ordem->qtd;
        nova->qtd -= ret;
        remover(ordem,ordensCompra);
    }
    else{
        ret = ordem->qtd;
        remover(nova,lista);
        remover(ordem,ordensCompra);
    }

    return ret;
}

void imprime(Lista *lista){
    Oferta *p = lista->inicio;
    printf("Imprimindo lista:\n");
    while(p != NULL){
        printf("%ix %s\n",p->qtd,p->nome);
        p = p->prox;
    }
}

void inserirNaOrdem(char nome[255], int qtd, Lista *lista){
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
