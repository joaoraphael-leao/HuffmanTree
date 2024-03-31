#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "_comprimir.h"
#include "_descomprimir.h"

const char MSG_ERRO[] = "Uso: huffman.exe [comprimir|descomprimir] [entrada]\n";

int main(int argc, char *argv[]) {
  FILE *arquivo_in;
  char *acao, *caminho_in;
  bool deve_descomprimir;

#ifdef _WIN32
  // Evita problemas com texto no terminal no WindowsW
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
  setlocale(LC_ALL, ".UTF8");
#endif /* WIN32 */

  // Posibilidades válidas:
  // argv = {"./huffman", "comprimir", "dados.'extensao'"}
  // argv = {"./huffman", "descomprimir", "dados.huff"}

  if (argc != 3) {
    printf("%s", MSG_ERRO);
    return 0;
  }

  acao = argv[1];
  caminho_in = argv[2];
  

  if (!strcmp(acao, "comprimir")) 
  {
    deve_descomprimir = false;
  } 
  else if (!strcmp(acao, "descomprimir")) 
  {
    deve_descomprimir = true;
  } 
  else 
  {
    // Ação desconhecida
    printf(MSG_ERRO);
    return EXIT_FAILURE;
  }

  arquivo_in = fopen(caminho_in, "rb");

  if (!arquivo_in) {
    perror("Não foi possível abrir o arquivo de entrada");
    return EXIT_FAILURE;
  }

  if(!deve_descomprimir) // CONFERIR EXTENSAO ANTES MESMO DE ABRIR O ARQUIVO, APOS ISSO PASSAR O TAMANHO E A EXTENSAO PARA A FUNCAO
  {
      char *nome_do_arquivo = caminho_in;
      char *extensao = strrchr(nome_do_arquivo, '.');

      if (extensao != NULL) 
      {
          extensao++; // Pula o ponto
      } 
      InteiroDe8Bits size_extensao = (InteiroDe8Bits)strlen(extensao);
        if(size_extensao > 6)
      {
          printf("ERRO. Nao é possivel uma extensao maior que 6.\n");
          return 0;
      }
      else if(size_extensao == 0)
      {
          printf("Nao foi possivel encontrar a extensao do arquivo.\n");
          return 0;
      }
      else if(size_extensao > 0)
      {
          comprimir(arquivo_in, size_extensao, extensao); // FILE, UINT8_T, CHAR*
      }
  }

  else
  {
    descomprimir(arquivo_in);
  }
  
  fclose(arquivo_in);
  return EXIT_SUCCESS;
}
