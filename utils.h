/* Include para trabalho de pregao eletronico */

#include <unistd.h>

/* sleep em ms; requer <unistd.h> */
#define msleep(x) usleep(1000*(x))

/* string de formato para itens no portfolio de cada thread */
#define fmtport  "  %-15s  %10u  %7u\n"

/* string de formato para itens no saldo de main() */
#define fmtsaldo "  %-15s  %10u  %8u\n"
