#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H

/*
 * Parser para arquivos .geo de Bitnópolis.
 *
 * Processa dois comandos:
 *   cq sw cfill cstrk   — define estilo global (espessura, preenchimento, borda)
 *   q  cep x y w h      — insere quadra no hashfile e a desenha no SVG
 *
 * Uso:
 *   1. Crie o hashfile de quadras com ehf_create().
 *   2. Crie o svg_writer_t com svg_writer_criar().
 *   3. Chame geo_handler_processar(); ela retorna os contadores.
 *   4. Feche/finalize os outros recursos normalmente.
 */

#include "extensible_hash_file.h"
#include "svg_writer.h"

typedef struct {
    int quadras_inseridas;
    int erros;
} geo_handler_resultado_t;

/*
 * Lê geo_filepath, insere cada quadra em quadras_hf e a desenha em svg.
 * svg pode ser NULL (nenhum desenho é feito).
 * Linhas malformadas ou com CEP duplicado são contadas em resultado.erros.
 */
geo_handler_resultado_t geo_handler_processar(
    const char *geo_filepath,
    extensible_hash_file_t quadras_hf,
    svg_writer_t *svg);

#endif
