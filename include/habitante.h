/**
 * @file habitante.h
 * @brief Habitante de Bitnópolis — pessoa com endereço opcional.
 *
 * Um habitante sem endereço é chamado de "sem-teto". Um habitante com
 * endereço registrado é chamado de "morador". Use habitante_definir_endereco()
 * para tornar um habitante morador e habitante_remover_endereco() para
 * transformá-lo em sem-teto.
 *
 * habitante_t é opaco; use o registro plano habitante_registro_t para
 * persistência em hashfile (chave = CPF).
 */

#ifndef HABITANTE_H
#define HABITANTE_H

#include "pessoa.h"

/** Tamanho máximo do CEP (sem o terminador nulo). */
#define HABITANTE_CEP_MAX    32u

/** Tamanho máximo do complemento de endereço (sem o terminador nulo). */
#define HABITANTE_COMPL_MAX  32u

/** Tipo opaco para um habitante. */
typedef struct habitante habitante_t;

/**
 * @brief Registro plano para armazenamento no hashfile de habitantes.
 *
 * Chave do hashfile: @c pessoa.cpf.
 * Quando @c is_morador == 0 (sem-teto), os campos de endereço ficam zerados.
 * @c face válida: @c 'N', @c 'S', @c 'L', @c 'O' ou @c '\\0' quando sem endereço.
 */
typedef struct {
    pessoa_registro_t pessoa;               /**< Dados pessoais do habitante. */
    int               is_morador;           /**< 1 se morador, 0 se sem-teto. */
    char              cep[HABITANTE_CEP_MAX + 1u];   /**< CEP da quadra onde reside. */
    char              face;                 /**< Face da quadra: 'N', 'S', 'L' ou 'O'. */
    int               num;                  /**< Número no logradouro. */
    char              compl[HABITANTE_COMPL_MAX + 1u]; /**< Complemento do endereço. */
} habitante_registro_t;

/**
 * @brief Cria um habitante (sem-teto) a partir de um registro de pessoa.
 *
 * @param pessoa_reg Dados da pessoa (não pode ser NULL).
 * @return Nova instância ou NULL se @p pessoa_reg for NULL ou faltar memória.
 */
habitante_t *habitante_criar(const pessoa_registro_t *pessoa_reg);

/**
 * @brief Libera todos os recursos do habitante. Aceita NULL com segurança.
 * @param h Instância a destruir.
 */
void habitante_destruir(habitante_t *h);

/**
 * @brief Define o endereço do habitante, tornando-o morador.
 *
 * Chamada inválida (h NULL, cep vazio, face inválida) é ignorada silenciosamente.
 *
 * @param h     Habitante a atualizar.
 * @param cep   CEP da quadra onde irá residir (não-NULL, não-vazio).
 * @param face  Face da quadra: 'N', 'S', 'L' ou 'O'.
 * @param num   Número no logradouro (>= 0).
 * @param compl Complemento do endereço (pode ser NULL ou vazio).
 */
void habitante_definir_endereco(habitante_t *h, const char *cep, char face,
                                int num, const char *compl);

/**
 * @brief Remove o endereço do habitante, tornando-o sem-teto.
 * @param h Habitante a atualizar.
 */
void habitante_remover_endereco(habitante_t *h);

/**
 * @brief Verifica se o habitante é morador.
 * @param h Habitante a consultar.
 * @return 1 se morador, 0 se sem-teto ou @p h for NULL.
 */
int habitante_eh_morador(const habitante_t *h);

/** @brief Retorna o CPF do habitante. NULL quando @p h é NULL. */
const char *habitante_obter_cpf(const habitante_t *h);

/** @brief Retorna o nome do habitante. NULL quando @p h é NULL. */
const char *habitante_obter_nome(const habitante_t *h);

/** @brief Retorna o sobrenome do habitante. NULL quando @p h é NULL. */
const char *habitante_obter_sobrenome(const habitante_t *h);

/** @brief Retorna o sexo do habitante ('M' ou 'F'). '\\0' quando @p h é NULL. */
char        habitante_obter_sexo(const habitante_t *h);

/** @brief Retorna a data de nascimento (formato dd/mm/aaaa). NULL quando @p h é NULL. */
const char *habitante_obter_nasc(const habitante_t *h);

/** @brief Retorna o CEP do endereço. NULL quando sem endereço ou @p h é NULL. */
const char *habitante_obter_cep(const habitante_t *h);

/** @brief Retorna a face da quadra. '\\0' quando sem endereço ou @p h é NULL. */
char        habitante_obter_face(const habitante_t *h);

/** @brief Retorna o número do logradouro. 0 quando sem endereço ou @p h é NULL. */
int         habitante_obter_num(const habitante_t *h);

/** @brief Retorna o complemento do endereço. NULL quando sem endereço ou @p h é NULL. */
const char *habitante_obter_compl(const habitante_t *h);

/**
 * @brief Serializa o habitante para um registro plano.
 *
 * @param h Habitante de origem (não pode ser NULL; comportamento indefinido se for).
 * @return Registro plano preenchido.
 */
habitante_registro_t habitante_para_registro(const habitante_t *h);

/**
 * @brief Reconstrói um habitante a partir de um registro plano.
 *
 * @param reg Registro de origem (não pode ser NULL).
 * @return Nova instância ou NULL se @p reg for NULL ou inválido.
 */
habitante_t *habitante_criar_de_registro(const habitante_registro_t *reg);

#endif // HABITANTE_H
