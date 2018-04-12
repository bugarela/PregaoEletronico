#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

void inicia(Lista *lista){
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

void inserir(char nome[255], int qtd, Lista *lista){
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

Oferta pop(Lista *lista){
    Oferta retorno;
    strcpy(retorno.nome,lista->inicio->nome);
    retorno.qtd = lista->inicio->qtd;
    retorno.prox = NULL;
    printf("Retirando %ix na lista\n", retorno.qtd);
    lista->inicio = lista->inicio->prox;
    lista->tamanho--;
    return retorno;
}
