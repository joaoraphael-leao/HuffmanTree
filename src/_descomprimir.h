#pragma once
#include <stdint.h>
#include <stdio.h>

#include "_estruturas.h"

/**
 * Descomprime um arquivo codificado por Huffman usando os dados e tabela no cabeçalho dele.
 * @param arquivo_in o arquivo comprimido
 * @param arquivo_out o arquivo onde o conteúdo descomprimido será salvo
 */
void descomprimir(FILE *arquivo_in);

/**
 * Lê uma árvore a partir da representação em pré-ordem de um arquivo.
 * @param arquivo o arquivo de onde a árvore será lida
 * @return um ponteiro para a árvore lida
 */
ArvoreBinaria *ler_arvore_preordem(FILE *arquivo);

/**
 * Encontra o tamanho do arquivo escpecificado.
 * @param arquivo o arquivo
 * @return o tamanho
 */
long encontrar_tamanho_arquivo(FILE *arquivo);

/**
 * Percorre um arquivo de entrada e usa a árvore Huffman passada para descomprimi-lo, salvando cada caractere
 * em um arquivo de saída.
 *
 * O lixo são os bits que devem ser ignorados no último byte comprimido.
 * @param arquivo_in o arquivo de entrada
 * @param arquivo_out o arquivo de saída
 * @param arvore a árvore Huffman
 * @param lixo a quantidade de bits de lixo
 */
void descomprimir_com_arvore(FILE *arquivo_in, FILE *arquivo_out, ArvoreBinaria *arvore, uint8_t lixo);

/**
 * Descomprime o caso especial de haver apenas um nó na árvore Huffman (um tipo de caractere).
 *
 * O lixo são os bits que devem ser ignorados no último byte comprimido.
 * @param arquivo_in o arquivo de entrada
 * @param arquivo_out o arquivo de saída
 * @param arvore a árvore Huffman com apenas um nó
 * @param lixo a quantidade de bits de lixo
 */
void descomprimir_com_caractereUnico(FILE *arquivo_in, FILE *arquivo_out, ArvoreBinaria *arvore, uint8_t lixo);



/**
 * Prepara um arquivo para ser descomprimido, lendo o cabeçalho e retornando um ponteiro para o arquivo
 * descomprimido.
 * @param arquivo_in o arquivo comprimido
 * @return um ponteiro para o arquivo descomprimido
 */
FILE* preparar_arquivo_descomprimido(FILE *arquivo_in, int tamanho_extensao);


/**
 * Lê o cabeçalho de um arquivo comprimido e retorna o tamanho da árvore em bytes.
 * @param arquivo_in o arquivo comprimido
 * @param cabecalho um ponteiro para onde o cabeçalho será salvo
 * @return o tamanho da árvore em bytes
 */
void posicionarAposArvore(FILE *arquivo_in, InteiroDe8Bits *cabecalho) ;