#include <stdlib.h>

#include "_utils.h"
typedef uint8_t InteiroDe8Bits;

void *Inteiro8BitsViraPonteiro(uint8_t valor) {
  uint8_t *valor_p = (uint8_t *)malloc(sizeof(*valor_p));
  *valor_p = valor;
  return valor_p;
}

InteiroDe8Bits PonteiroViraInteiro8Bits(void *ptr) {
  return *((uint8_t *)ptr);
}
