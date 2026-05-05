/**
 * @file pm_handler.h
 * @brief Parser de arquivos .pm de Bitnópolis.
 *
 * Processa dois comandos:
 * - @c p  @c cpf @c nome @c sobrenome @c sexo @c nasc — insere habitante
 *   (sem-teto) no hashfile de habitantes.
 * - @c m  @c cpf @c cep @c face @c num @c compl — registra morador: valida
 *   se o CEP existe no hashfile de quadras e se o CPF existe no hashfile de
 *   habitantes, então atualiza o registro.
 *
 * Pré-condições:
 * - @p habitantes_hf deve estar aberto com @c record_size = sizeof(habitante_registro_t).
 * - @p quadras_hf deve estar aberto com @c record_size = sizeof(quadra_registro_t).
 * - O hashfile de quadras é consultado somente em leitura (não é modificado).
 */

#ifndef PM_HANDLER_H
#define PM_HANDLER_H

#include "extensible_hash_file.h"

/**
 * @brief Resultado do processamento de um arquivo .pm.
 */
typedef struct {
    int pessoas_inseridas;      /**< Habitantes (sem-teto) inseridos com sucesso. */
    int moradores_registrados;  /**< Habitantes promovidos a morador com sucesso. */
    int erros;                  /**< Linhas malformadas, CPF duplicado, CEP/CPF inexistente. */
} pm_handler_resultado_t;

/**
 * @brief Lê e processa um arquivo .pm, atualizando os hashfiles fornecidos.
 *
 * Linhas malformadas, CPF duplicado no comando @c p, ou CEP/CPF inexistente
 * no comando @c m incrementam @c resultado.erros.
 *
 * @param pm_filepath    Caminho para o arquivo .pm (não-NULL).
 * @param habitantes_hf  Hashfile de habitantes aberto para leitura e escrita.
 * @param quadras_hf     Hashfile de quadras aberto para leitura.
 * @return Estrutura com contadores de inserções, registros e erros.
 */
pm_handler_resultado_t pm_handler_processar(
    const char *pm_filepath,
    extensible_hash_file_t habitantes_hf,
    extensible_hash_file_t quadras_hf);

#endif // PM_HANDLER_H
