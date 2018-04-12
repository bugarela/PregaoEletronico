typedef struct a{
    struct a *prox;
    char nome[255];
    int qtd;
}Oferta;

typedef struct b{
    Oferta *inicio, *fim;
    int tamanho;
}Lista;

void inicia(Lista *lista);
void inserir(char nome[255], int qtd, Lista *lista);
Oferta pop(Lista *lista);
