#include <CUnit/Basic.h>
#include <stdlib.h>
#include <string.h>

#include "_comprimir.h"

#include "_descomprimir.h"
#include "_estruturas.h"
#include "_utils.h"

typedef struct Caso {
  const char *nome;
  CU_TestFunc funcao;
} Caso;

void testar_arvore_bin() {
  ArvoreBinaria *arvore, *esquerda, *direita;

  esquerda = arvore_bin_criar(Inteiro8BitsViraPonteiro(0), free, NULL, NULL);
  direita = arvore_bin_criar(Inteiro8BitsViraPonteiro(100), free, NULL, NULL);
  arvore = arvore_bin_criar(Inteiro8BitsViraPonteiro(50), free, esquerda, direita);

  CU_ASSERT_FALSE(arvore_bin_e_folha(arvore));
  CU_ASSERT_EQUAL(PonteiroViraInteiro8Bits(arvore_bin_item(arvore)), 50);
  CU_ASSERT_PTR_EQUAL(arvore_bin_esquerda(arvore), esquerda);
  CU_ASSERT_PTR_EQUAL(arvore_bin_direita(arvore), direita);
  arvore_bin_desalocar(arvore);

  arvore = arvore_bin_criar(Inteiro8BitsViraPonteiro(50), free, NULL, NULL);
  CU_ASSERT_TRUE(arvore_bin_e_folha(arvore));
  arvore_bin_desalocar(arvore);

  esquerda = arvore_bin_criar(Inteiro8BitsViraPonteiro(0), free, NULL, NULL);
  arvore = arvore_bin_criar(Inteiro8BitsViraPonteiro(50), free, esquerda, NULL);
  CU_ASSERT_FALSE(arvore_bin_e_folha(arvore));
  CU_ASSERT_PTR_EQUAL(arvore_bin_esquerda(arvore), esquerda);
  arvore_bin_desalocar(arvore);
}

void comparar_no_fila(NoFilaPrio *no, uint8_t item, unsigned prioridade) {
  CU_ASSERT_PTR_NOT_NULL_FATAL(no);
  CU_ASSERT_EQUAL(PonteiroViraInteiro8Bits(no_fila_prio_item(no)), item);
  CU_ASSERT_EQUAL(no_fila_prio_prioridade(no), prioridade);
}

void testar_fila_prio() {
  FilaPrio *fila;
  NoFilaPrio *no;
  void *item;

  fila = fila_prio_criar();
  CU_ASSERT_PTR_NULL_FATAL(fila_prio_desenfileirar(fila));
  CU_ASSERT_TRUE(fila_prio_esta_vazia(fila));

  fila_prio_enfileirar(fila, Inteiro8BitsViraPonteiro(6), free, 7);
  fila_prio_enfileirar(fila, Inteiro8BitsViraPonteiro(9), free, 10);
  fila_prio_enfileirar(fila, Inteiro8BitsViraPonteiro(3), free, 1);
  CU_ASSERT_FALSE(fila_prio_esta_vazia(fila));

  no = fila_prio_desenfileirar(fila);
  comparar_no_fila(no, 3, 1);
  item = no_fila_prio_tomar_item(no);
  CU_ASSERT_EQUAL(PonteiroViraInteiro8Bits(item), 3);
  CU_ASSERT_PTR_EQUAL_FATAL(no_fila_prio_item(no), NULL);
  no_fila_prio_desalocar(no);
  free(item);

  no = fila_prio_desenfileirar(fila);
  comparar_no_fila(no, 6, 7);
  no_fila_prio_desalocar(no);

  fila_prio_enfileirar(fila, Inteiro8BitsViraPonteiro(100), free, 5);
  no = fila_prio_desenfileirar(fila);
  comparar_no_fila(no, 100, 5);
  no_fila_prio_desalocar(no);

  no = fila_prio_desenfileirar(fila);
  comparar_no_fila(no, 9, 10);
  no_fila_prio_desalocar(no);

  CU_ASSERT_PTR_NULL_FATAL(fila_prio_desenfileirar(fila));
  CU_ASSERT(fila_prio_esta_vazia(fila));

  fila_prio_desalocar(fila);
}



int main() {
  CU_pSuite suite;
  size_t i;

  if (CU_initialize_registry() != CUE_SUCCESS) {
    return CU_get_error();
  }

  suite = CU_add_suite("Suite", NULL, NULL);
  if (!suite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  Caso casos[] = {
    {"Árvore binária", testar_arvore_bin},
    {"Fila de prioridade", testar_fila_prio},
  };

  for (i = 0; i < sizeof(casos) / sizeof(*casos); i++) {
    if (!CU_add_test(suite, casos[i].nome, casos[i].funcao)) {
      CU_cleanup_registry();
      return CU_get_error();
    }
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
