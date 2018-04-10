typedef struct a{
    struct a *prox;
    char nome[255];
    int qtd;
}Oferta;

typedef struct b{
    Oferta *inicio, *fim;
    int tamanho;
}Fila;

void inicia(Fila *fila);
void inserir(char nome[255], int qtd, Fila *fila);
Oferta pop(Fila *fila);
