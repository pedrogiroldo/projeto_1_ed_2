#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H

/*
 * Processador de consultas e atualizações (.qry) de Bitnópolis.
 *
 * Comandos suportados:
 *   rq  cep                         — remove quadra; moradores → sem-teto
 *   pq  cep                         — conta moradores por face e total
 *   censo                           — estatísticas gerais da cidade
 *   h?  cpf                         — dados de um habitante
 *   nasc cpf nome sobrenome sexo nasc — nova pessoa nasce
 *   rip cpf                         — pessoa falece
 *   mud cpf cep face num compl      — morador muda de endereço
 *   dspj cpf                        — morador é despejado
 *
 * Saídas: resultados textuais em txt e marcações visuais em svg (pode ser NULL).
 */

#include "../extensible_hash_file/extensible_hash_file.h"
#include "../svg_writer/svg_writer.h"
#include "../txt_writer/txt_writer.h"

typedef struct {
    int comandos_processados;
    int erros;
} qry_handler_resultado_t;

qry_handler_resultado_t qry_handler_processar(
    const char *qry_filepath,
    extensible_hash_file_t quadras_hf,
    extensible_hash_file_t habitantes_hf,
    svg_writer_t *svg,
    txt_writer_t *txt);

#endif
