#ifndef PM_HANDLER_H
#define PM_HANDLER_H

/*
 * Parser para arquivos .pm de Bitnópolis.
 *
 * Processa dois comandos:
 *   p cpf nome sobrenome sexo nasc   — insere habitante (sem-teto) no hashfile
 *   m cpf cep face num compl         — registra morador: valida CEP e CPF,
 *                                      atualiza registro no hashfile
 *
 * Pré-condições:
 *   - habitantes_hf deve estar aberto com record_size = sizeof(habitante_registro_t)
 *   - quadras_hf deve estar aberto com record_size = sizeof(quadra_registro_t)
 *   - O hashfile de quadras é consultado em leitura (não modificado)
 */

#include "../extensible_hash_file/extensible_hash_file.h"

typedef struct {
    int pessoas_inseridas;
    int moradores_registrados;
    int erros;
} pm_handler_resultado_t;

/*
 * Lê pm_filepath e aplica comandos p/m nos hashfiles fornecidos.
 * Linhas malformadas, CPF duplicado, CEP inexistente ou CPF inexistente no
 * comando m são contados em resultado.erros.
 */
pm_handler_resultado_t pm_handler_processar(
    const char *pm_filepath,
    extensible_hash_file_t habitantes_hf,
    extensible_hash_file_t quadras_hf);

#endif
