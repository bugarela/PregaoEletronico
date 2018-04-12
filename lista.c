#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

void inicia(Lista *lista){
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
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

      if(lista->inicio == NULL)
          lista->inicio = nova;
      else
          lista->fim->prox = nova;

      lista->fim = nova;
      lista->tamanho++;
    }
}

int compra(char nome[255], int qtd, Lista *lista){
    Oferta *nova = busca(nome,lista);
    printf("comprou\n");
    if (nova == NULL)
        return 0;
    if(nova->qtd < qtd)
        qtd = nova->qtd;
    nova->qtd-=qtd;
    return qtd;
}

Oferta novaOferta(Lista *lista){
    while(lista->inicio->qtd == 0 && lista->inicio->prox != NULL)
        lista->inicio = lista->inicio->prox;
    Oferta retorno;
    printf("teste\n");
    if(lista->inicio == NULL){
        printf("banana");
        return retorno;
    }
    strcpy(retorno.nome,lista->inicio->nome);
    retorno.qtd = lista->inicio->qtd;
    retorno.prox = NULL;

    return retorno;
}
