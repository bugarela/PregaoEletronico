#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fila.h"

void inicia(Fila *fila){
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
}

void inserir(char nome[255], int qtd, Fila *fila){
    Oferta *nova = malloc(sizeof(Oferta));
    strcpy (nova->nome,nome);
    nova->qtd = qtd;
    nova->prox = NULL;

    if(fila->inicio == NULL)
        fila->inicio = nova;
    else
        fila->fim->prox = nova;

    fila->fim = nova;
    fila->tamanho++;
}

Oferta pop(Fila *fila){
    Oferta retorno;
    strcpy(retorno.nome,fila->inicio->nome);
    retorno.qtd = fila->inicio->qtd;
    retorno.prox = NULL;
    printf("Retirando %ix na fila\n", retorno.qtd);
    fila->inicio = fila->inicio->prox;
    fila->tamanho--;
    return retorno;
}
