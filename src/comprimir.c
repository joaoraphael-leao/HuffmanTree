#include <stdlib.h>
#include <string.h>
#include "_comprimir.h"
#include "_estruturas.h"
#include "_utils.h"
#include <math.h>

//CRIAR ARQUIVO OUT
void comprimir(FILE *arquivo_in, InteiroDe8Bits size_extensao, char* extensao) 
{
  ArvoreBinaria *arvore;
  Caminho tabela[256];
  long freqs[256];
  InteiroDe16Bits cabecalho, tamanho_arvore;
  InteiroDe8Bits lixo;

  FILE *arquivo_out = fopen("comprimido.huff", "wb");
  if (arquivo_out == NULL) 
  {
    printf("Erro ao abrir o arquivo para escrita\n");
    return;
  }
  ler_frequencias(arquivo_in, freqs);
  arvore = transformar_frequencias_em_arvore(freqs);

  if (arvore_bin_e_folha(arvore)) 
  {
    // Caso especial de existir apenas um tipo de caractere (repetido ou não) no arquivo
    tabela[PonteiroViraInteiro8Bits(arvore_bin_item(arvore))].qtd_bits = 1;
    tabela[PonteiroViraInteiro8Bits(arvore_bin_item(arvore))].bitsCaminho = 0;
  } 
  else 
  {
    transformar_arvore_em_caminho(arvore, tabela, 0, 0);
  }

  // Pula os dois primeiros bytes no arquivo de saída para salvar a arvore
  fseek(arquivo_out, 2, SEEK_CUR);
  tamanho_arvore = salvar_arvore_preordem(arvore, arquivo_out);

  fputc(size_extensao << 5, arquivo_out); //COLOCAR O TAMANHO DA EXTENSAO NOS 3 MAIS SIGNIFICATIVOS
  fwrite(extensao, 1, size_extensao, arquivo_out); //ESCREVER A EXTENSAO

  // Volta pa rao começo do arquivo de entrada e comprime o contéudo
  fseek(arquivo_in, 0, SEEK_SET);
  lixo = comprimir_com_tabela(arquivo_in, arquivo_out, tabela);

  // Volta para o começo do arquivo de saída e salva o cabeçalho
  fseek(arquivo_out, 0, SEEK_SET);
  cabecalho = lixo << 13 | tamanho_arvore; //pega o lixo e joga pros 3 primeiros bits do cabecalho, dps faz o | booleano com os 0 para salvar o tamanho
  
  //DIVISAO DOS PRIMEIROS BYTES EM 2 PARTES
  fputc(cabecalho >> 8, arquivo_out); //ESCREVE OS BITS MAIS SIGNIFICATIVOS
  fputc(cabecalho & 0xFF, arquivo_out); //FAZ O & COM OS BITS MENOS SIGNIFICATIVOS

  

  arvore_bin_desalocar(arvore);//FREE
}

void ler_frequencias(FILE *arquivo, long freqs[]) {
  int caractere;

  // Zera a tabela
  memset(freqs, 0, sizeof(*freqs) * 256);

  while ((caractere = fgetc(arquivo)) != EOF) {
    freqs[caractere]++;
  }
}

ArvoreBinaria *transformar_frequencias_em_arvore(long freqs[]) {
  ArvoreBinaria *huffman, *novo_no, *esquerda, *direita;
  FilaPrio *fila;
  NoFilaPrio *menor1, *menor2;
  long freq, nova_prioridade;
  unsigned i;

  fila = fila_prio_criar();

  // Coloca as frequências em uma fila de prioridade
  for (i = 0; i < 256; i++) {
    freq = freqs[i];
    if (freq > 0) {
      novo_no = arvore_bin_criar(Inteiro8BitsViraPonteiro((uint8_t)i), free, NULL, NULL);
      fila_prio_enfileirar(fila, novo_no, (Desalocador)arvore_bin_desalocar, freq);
    }
  }

  huffman = NULL;

  while (!fila_prio_esta_vazia(fila)) {
    // Desenfileira os nós com as menores frequências/prioridades
    menor1 = fila_prio_desenfileirar(fila);
    menor2 = fila_prio_desenfileirar(fila);

    // Se houver apenas um nó, o algoritmo finaliza
    if (!menor2) {
      huffman = (ArvoreBinaria *)no_fila_prio_tomar_item(menor1);
      no_fila_prio_desalocar(menor1);
      break;
    }

    // Cria um novo nó da árvore e torna menor1 e menor2 filhos dele
    esquerda = (ArvoreBinaria *)no_fila_prio_tomar_item(menor1);
    direita = (ArvoreBinaria *)no_fila_prio_tomar_item(menor2);
    novo_no = arvore_bin_criar(Inteiro8BitsViraPonteiro('*'), free, esquerda, direita);

    // Coloca o nó na fila de prioridade
    nova_prioridade = no_fila_prio_prioridade(menor1) + no_fila_prio_prioridade(menor2);
    fila_prio_enfileirar(fila, novo_no, (Desalocador)arvore_bin_desalocar, nova_prioridade);

    no_fila_prio_desalocar(menor1);
    no_fila_prio_desalocar(menor2);
  }

  fila_prio_desalocar(fila);
  return huffman;
}

void transformar_arvore_em_caminho(ArvoreBinaria *no, Caminho tabela[], unsigned bits, unsigned qtd_bits) {
  uint8_t caractere;

  if (arvore_bin_esquerda(no)) {
    transformar_arvore_em_caminho(arvore_bin_esquerda(no), tabela, bits, qtd_bits + 1);
  }

  if (arvore_bin_direita(no)) {
    transformar_arvore_em_caminho(arvore_bin_direita(no), tabela, set_bit(bits, qtd_bits), qtd_bits + 1);
  }

  if (arvore_bin_e_folha(no)) {
    caractere = PonteiroViraInteiro8Bits(arvore_bin_item(no));
    tabela[caractere].qtd_bits = qtd_bits;
    tabela[caractere].bitsCaminho = bits;
  }
}

uint16_t salvar_arvore_preordem(ArvoreBinaria *arvore, FILE *arquivo) {
  uint16_t tamanho;
  uint8_t caractere;

  tamanho = 0;

  if (arvore) {
    caractere = PonteiroViraInteiro8Bits(arvore_bin_item(arvore));

    if (caractere == '\\' || (caractere == '*' && arvore_bin_e_folha(arvore))) {
      fputc('\\', arquivo);
      tamanho++;
    }

    fputc(caractere, arquivo);
    tamanho++;

    tamanho += salvar_arvore_preordem(arvore_bin_esquerda(arvore), arquivo);
    tamanho += salvar_arvore_preordem(arvore_bin_direita(arvore), arquivo);
  }

  return tamanho;
}

uint8_t comprimir_com_tabela(FILE *arquivo_in, FILE *arquivo_out, Caminho tabela[]) {
  Caminho caminho;
  int caractere;
  InteiroDe8Bits bits, bit_atual, i;

  bits = 0;
  bit_atual = 0;

  // Para cada caractere
  while ((caractere = fgetc(arquivo_in)) != EOF) 
  {
    caminho = tabela[caractere];

    for (i = 0; i < caminho.qtd_bits; i++) 
    {
      if (is_bit_i_set(caminho.bitsCaminho, i)) 
      {
        // Se o bit for 1, usa set_bit no byte compactado
        bits = set_bit(bits, 7 - bit_atual); //7 - bit atual para inverter a ordem dos bits e deixar igual ao slide
      }

      bit_atual++;

      // Se o byte compactado estiver completo
      if (bit_atual == 8) {
        fputc(bits, arquivo_out);
        bits = 0;
        bit_atual = 0;
      }
    }
  }

  // Se um byte incompleto houver sobrado
  if (bit_atual > 0) {
    fputc(bits, arquivo_out);
  }

  // Retorna a quantidade de bits incompletos como lixo
  return 8 - bit_atual;
}


