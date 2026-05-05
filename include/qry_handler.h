/**
 * @file qry_handler.h
 * @brief Processador de consultas e atualizações (.qry) de Bitnópolis.
 *
 * Comandos suportados:
 * | Comando | Argumentos                              | Descrição                                    |
 * |---------|-----------------------------------------|----------------------------------------------|
 * | @c rq   | @c cep                                  | Remove a quadra; moradores tornam-se sem-teto. |
 * | @c pq   | @c cep                                  | Conta moradores por face (N/S/L/O) e total.  |
 * | @c censo | —                                      | Estatísticas gerais da cidade.               |
 * | @c h?   | @c cpf                                  | Dados de um habitante.                       |
 * | @c nasc | @c cpf @c nome @c sobrenome @c sexo @c nasc | Nova pessoa nasce (insere habitante).    |
 * | @c rip  | @c cpf                                  | Pessoa falece (remove habitante).            |
 * | @c mud  | @c cpf @c cep @c face @c num @c compl   | Morador muda de endereço.                    |
 * | @c dspj | @c cpf                                  | Morador é despejado (vira sem-teto).         |
 *
 * Saídas textuais são gravadas via @p txt; marcações visuais via @p svg
 * (ambos podem ser NULL para omitir a saída correspondente).
 */

#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H

#include "extensible_hash_file.h"
#include "svg_writer.h"
#include "txt_writer.h"

/**
 * @brief Resultado do processamento de um arquivo .qry.
 */
typedef struct {
    int comandos_processados; /**< Comandos executados com sucesso. */
    int erros;                /**< Comandos com argumentos inválidos ou entidade inexistente. */
} qry_handler_resultado_t;

/**
 * @brief Lê e processa um arquivo .qry, consultando e atualizando os hashfiles.
 *
 * @param qry_filepath   Caminho para o arquivo .qry (não-NULL).
 * @param quadras_hf     Hashfile de quadras aberto para leitura e escrita.
 * @param habitantes_hf  Hashfile de habitantes aberto para leitura e escrita.
 * @param svg            Writer SVG para marcações visuais, ou NULL para omitir.
 * @param txt            Writer de texto para respostas, ou NULL para omitir.
 * @return Estrutura com contadores de comandos processados e erros.
 */
qry_handler_resultado_t qry_handler_processar(
    const char *qry_filepath,
    extensible_hash_file_t quadras_hf,
    extensible_hash_file_t habitantes_hf,
    svg_writer_t *svg,
    txt_writer_t *txt);

#endif // QRY_HANDLER_H
