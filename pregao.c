#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

int main(int argc, char *argv[]){
   FILE *fp;
   char buff[255];
   int qtd;
   char nomearq[255];

   if (argv[1] == NULL || argv[2] == NULL){
       puts("Uso: ./pregao nthr nomearq\n");
       exit(1);
   }

   int nthr = atoi(argv[1]);
   strcpy(nomearq,argv[2]);

   printf("nthr= %i, nomearq= %s\n\n",nthr, nomearq);

   fp = fopen("exemplos/prgA", "r");
   while (fscanf(fp, "%s", buff) != EOF){
       fscanf(fp, "%i", &qtd);
       printf("1 : %ix %s\n", qtd, buff );
   }
   fclose(fp);
   return 0;

}
