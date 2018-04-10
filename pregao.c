#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "fila.h"

Fila fila;

int main(int argc, char *argv[]){
   FILE *fp;
   char buff[255];
   int qtd;
   char nomearq[255];

   if (argv[1] == NULL || argv[2] == NULL){
       printf("Uso: %s nthr nomearq\n",argv[0]);
       exit(1);
   }

   int nthr = atoi(argv[1]);
   strcpy(nomearq,argv[2]);
   printf("nthr= %i, nomearq= %s\n\n",nthr, nomearq);

   inicia(&fila);

   fp = fopen(nomearq, "r");
   while (fscanf(fp, "%s", buff) != EOF){
       fscanf(fp, "%i", &qtd);
       //printf("%ix %s\n", qtd, buff );
       inserir(buff, qtd, &fila);
       printf("Inserido %ix %s na fila\n", fila.inicio->qtd, fila.inicio->nome);
   }

   fclose(fp);

   while (fila.inicio != NULL){
       Oferta atual = pop(&fila);
       printf("%ix %s\n", atual.qtd, atual.nome );
   }


   return 0;

}
