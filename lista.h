typedef struct a{
    struct a *prox, *ant;
    char nome[255];
    unsigned int qtd;
}Oferta;

typedef struct b{
    Oferta *inicio, *fim;
}Lista;

typedef struct c{
    Oferta oferta;
    unsigned int comprado;
    struct c *prox;
}Item;

typedef struct d{
    int nThread;
    Item *inicio,*fim;
}Portfolio;

void inicia(Lista *lista);
void inserir(char nome[255], unsigned int qtd, Lista *lista);
Oferta pop(Lista *lista);
unsigned int compra(Oferta *ordem, Oferta *nova, Lista *lista, Lista *ordensCompra);
Oferta *novaOferta(Lista *lista);
void remover(Oferta *p, Lista *lista);
Oferta* busca(char nome[255],Lista *lista);
void imprime(Lista *lista);
void inserirNaOrdem(char nome[255], unsigned int qtd, Lista *lista);
