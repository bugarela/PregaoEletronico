#include "lista.h"


#define FIM 999999

pthread_barrier_t barreira;
char nomearq[255];
Lista ofertas;
void corretor(void *arg);

pthread_mutex_t mutex;
pthread_mutex_t mutexThreads;
pthread_cond_t c;
pthread_cond_t cPregao;
pthread_cond_t cThreads;
int ofertasDiponiveis;
int acabou;
int temThreadOlhando;
int outraThreadOlhando;
int qtdUltimaAnterior;
