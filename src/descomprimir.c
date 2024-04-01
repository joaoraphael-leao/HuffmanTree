#include <stdlib.h>
#include <string.h>
#include "_descomprimir.h"
#include "_estruturas.h"
#include "_utils.h"

void descomprimir(FILE *arquivo_in) {
  ArvoreBinaria *arvore;
  
  InteiroDe8Bits cabecalho[2], lixo;

  fread(&cabecalho, 1, sizeof(cabecalho), arquivo_in); //JA PASSA 2 POSICOES
  lixo = cabecalho[0] >> 5;
  
  arvore = ler_arvore_preordem(arquivo_in);
  
  
  int tamanho_extensao = fgetc(arquivo_in) >> 5;
  FILE *arquivo_out = preparar_arquivo_descomprimido(arquivo_in, tamanho_extensao);
  if (arvore_bin_e_folha(arvore)) {
    
    // Caso especial de existir apenas um tipo de caractere (repetido ou não) no arquivo
    descomprimir_com_caractereUnico(arquivo_in, arquivo_out, arvore, lixo);
  } 
  else 
  {
    
    descomprimir_com_arvore(arquivo_in, arquivo_out, arvore, lixo);
  }
  fclose(arquivo_out);
  arvore_bin_desalocar(arvore);
}


ArvoreBinaria *ler_arvore_preordem(FILE *arquivo) {
  // Adaptado de https://stackoverflow.com/a/4909298
  ArvoreBinaria *esquerda, *direita;
  InteiroDe8Bits caractere;
  bool escape;
  int temp;

  temp = fgetc(arquivo);
  if (temp == EOF) {
    fprintf(stderr, "Erro: final do arquivo atingido ao ler a árvore de Huffman. O arquivo pode estar corrompido.\n");
    exit(EXIT_FAILURE);
  }
  caractere = (InteiroDe8Bits)temp;

  escape = caractere == '\\'; // Testa se há um escape

  if (escape) {
    temp = fgetc(arquivo);
    if (temp == EOF) {
      fprintf(stderr, "Erro: final do arquivo atingido ao ler a árvore de Huffman. O arquivo pode estar corrompido.\n");
      exit(EXIT_FAILURE);
    }
    caractere = (InteiroDe8Bits)temp;
  }

  if (caractere == '*' && !escape) {
    esquerda = ler_arvore_preordem(arquivo);
    direita = ler_arvore_preordem(arquivo);
  } else {
    esquerda = direita = NULL;
  }

  return arvore_bin_criar(Inteiro8BitsViraPonteiro(caractere), free, esquerda, direita);
}

long encontrar_tamanho_arquivo(FILE *arquivo) {
  long atual, ultima;

  atual = ftell(arquivo);          // Pega a posição atual
  fseek(arquivo, 0, SEEK_END);     // Vai para o final do arquivo
  ultima = ftell(arquivo);         // Pega a ultima posição/tamanho
  fseek(arquivo, atual, SEEK_SET); // Volta para onde estava

  return ultima;
}

void descomprimir_com_arvore(FILE *arquivo_in, FILE *arquivo_out, ArvoreBinaria *arvore, InteiroDe8Bits lixo) {
  ArvoreBinaria *atual;
  long posicaoArquivo, tamanho;
  int caractere;
  unsigned i, qtd_bits;
  bool ultimo_byte;

  posicaoArquivo = ftell(arquivo_in);
  tamanho = encontrar_tamanho_arquivo(arquivo_in);

  // Começa a percorrer na raíz
  atual = arvore;

  while ((caractere = fgetc(arquivo_in)) != EOF) {
    ultimo_byte = ( posicaoArquivo == tamanho - 1 ) ;

    // Se for o último byte, ignora os bits de lixo
    qtd_bits = (ultimo_byte) ? 8 - lixo : 8;

    for (i = 0; i < qtd_bits; i++) 
    {
      if (is_bit_i_set(caractere, 7 - i)) 
      {
        atual = arvore_bin_direita(atual);
      } 
      else 
      {
        atual = arvore_bin_esquerda(atual);
      }

      if (arvore_bin_e_folha(atual)) 
      {
        fputc(PonteiroViraInteiro8Bits(arvore_bin_item(atual)), arquivo_out);

        // Volta para a raíz da árvore
        atual = arvore;
      }
    }

    posicaoArquivo++;
  }
}

void descomprimir_com_caractereUnico(FILE *arquivo_in, FILE *arquivo_out, ArvoreBinaria *arvore, InteiroDe8Bits lixo) {
  long posicaoInicialBits, tamanho, qtd_bits;
  InteiroDe8Bits caractere;

  posicaoInicialBits = ftell(arquivo_in);
  tamanho = encontrar_tamanho_arquivo(arquivo_in);

  // Calcula a quantidade de bits comprimidos. Explicação:
  // tamanho - pos -> A quantidade de bytes comprimidos, que é o tamanho de tudo depois da tabela (pos)
  // * 8 -> De bytes para bits
  // - lixo -> Remove o lixo
  qtd_bits = (tamanho - posicaoInicialBits) * 8 - lixo;

  caractere = PonteiroViraInteiro8Bits(arvore_bin_item(arvore));

  for (long i = 0; i < qtd_bits; i++) 
  {
    // Já que sabemos que existe apenas um tipo de caractere no arquivo, não é necessário checar os bits da
    // entrada, simplesmente usamos a quantidade calculada anteriormente
    fputc(caractere, arquivo_out);
  }
}

FILE* preparar_arquivo_descomprimido(FILE *arquivo_in, int tamanho_extensao) 
{
  
  char *extensao = (char *)malloc((size_t)(tamanho_extensao + 1) * sizeof(char));

  fread(extensao, 1, (size_t)tamanho_extensao, arquivo_in);
  extensao[tamanho_extensao] = '\0';
  printf("EXTENSAO %s\n", extensao);
  char nome_arquivo[21] = "descomprimido.";
  strcat(nome_arquivo, extensao);

  free(extensao);

  FILE *arquivo_out = fopen(nome_arquivo, "wb");

  return arquivo_out;
}
