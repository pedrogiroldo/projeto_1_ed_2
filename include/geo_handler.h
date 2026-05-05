/**
 * @file geo_handler.h
 * @brief Parser de arquivos .geo de Bitnópolis.
 *
 * Processa dois comandos:
 * - @c cq  @c sw @c cfill @c cstrk — define estilo global (espessura,
 *   preenchimento, borda) para as quadras seguintes.
 * - @c q   @c cep @c x @c y @c w @c h — insere quadra no hashfile e a
 *   desenha no SVG.
 *
 * Uso típico:
 * @code
 *   extensible_hash_file_t hf = ehf_create(path, cap, sizeof(quadra_registro_t));
 *   svg_writer_t *svg = svg_writer_criar(svg_path, 0, 0);
 *   geo_handler_resultado_t r = geo_handler_processar(geo_path, hf, svg);
 *   // usar r.quadras_inseridas e r.erros
 *   svg_writer_finalizar(svg);
 *   svg_writer_destruir(svg);
 *   ehf_close(hf);
 * @endcode
 */

#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H

#include "extensible_hash_file.h"
#include "svg_writer.h"

/**
 * @brief Resultado do processamento de um arquivo .geo.
 */
typedef struct {
    int quadras_inseridas; /**< Número de quadras inseridas com sucesso. */
    int erros;             /**< Linhas malformadas ou CEPs duplicados. */
} geo_handler_resultado_t;

/**
 * @brief Lê e processa um arquivo .geo, inserindo quadras no hashfile.
 *
 * Para cada comando @c q válido, insere um @c quadra_registro_t no hashfile e,
 * se @p svg não for NULL, desenha o retângulo correspondente.
 * Linhas malformadas ou com CEP duplicado incrementam @c resultado.erros.
 *
 * @param geo_filepath Caminho para o arquivo .geo (não-NULL).
 * @param quadras_hf   Hashfile aberto com @c record_size = sizeof(quadra_registro_t).
 * @param svg          Writer SVG de destino, ou NULL para omitir o desenho.
 * @return Estrutura com contadores de inserções e erros.
 */
geo_handler_resultado_t geo_handler_processar(
    const char *geo_filepath,
    extensible_hash_file_t quadras_hf,
    svg_writer_t *svg);

#endif // GEO_HANDLER_H
