/**
 * @file pessoa.h
 * @brief Representação de um habitante de Bitnópolis.
 *
 * pessoa_t é um tipo opaco; use pessoa_criar() para instanciar e
 * pessoa_destruir() para liberar. Para persistência em hashfile use
 * pessoa_para_registro() / pessoa_criar_de_registro().
 */

#ifndef PESSOA_H
#define PESSOA_H

/** Tamanho máximo do CPF (aceita formato com pontuação, sem o terminador nulo). */
#define PESSOA_CPF_MAX       15u

/** Tamanho máximo de nome ou sobrenome (sem o terminador nulo). */
#define PESSOA_NOME_MAX      64u

/** Tamanho máximo da data de nascimento no formato dd/mm/aaaa (sem o terminador nulo). */
#define PESSOA_NASC_MAX      10u

/** Tipo opaco para uma pessoa. */
typedef struct pessoa pessoa_t;

/**
 * @brief Registro plano serializável para armazenamento em hashfile.
 *
 * Chave do hashfile: campo @c cpf.
 */
typedef struct {
    char cpf[PESSOA_CPF_MAX + 1u];             /**< CPF (chave). */
    char nome[PESSOA_NOME_MAX + 1u];           /**< Primeiro nome. */
    char sobrenome[PESSOA_NOME_MAX + 1u];      /**< Sobrenome. */
    char sexo;                                 /**< 'M' (masculino) ou 'F' (feminino). */
    char nasc[PESSOA_NASC_MAX + 1u];           /**< Data de nascimento (dd/mm/aaaa). */
} pessoa_registro_t;

/**
 * @brief Cria uma pessoa com os dados fornecidos.
 *
 * Todos os ponteiros devem ser não-NULL e não-vazios. @p sexo deve ser
 * @c 'M' ou @c 'F'. @p cpf deve ter entre 1 e PESSOA_CPF_MAX caracteres.
 *
 * @param cpf        CPF da pessoa.
 * @param nome       Primeiro nome.
 * @param sobrenome  Sobrenome.
 * @param sexo       'M' ou 'F'.
 * @param nasc       Data de nascimento (dd/mm/aaaa).
 * @return Nova instância ou NULL se algum argumento for inválido ou faltar memória.
 */
pessoa_t *pessoa_criar(const char *cpf, const char *nome,
                       const char *sobrenome, char sexo, const char *nasc);

/**
 * @brief Libera todos os recursos associados à pessoa. Aceita NULL com segurança.
 * @param p Instância a destruir.
 */
void pessoa_destruir(pessoa_t *p);

/** @brief Retorna o CPF da pessoa. NULL quando @p p é NULL. */
const char *pessoa_obter_cpf(const pessoa_t *p);

/** @brief Retorna o primeiro nome. NULL quando @p p é NULL. */
const char *pessoa_obter_nome(const pessoa_t *p);

/** @brief Retorna o sobrenome. NULL quando @p p é NULL. */
const char *pessoa_obter_sobrenome(const pessoa_t *p);

/** @brief Retorna o sexo ('M' ou 'F'). '\\0' quando @p p é NULL. */
char        pessoa_obter_sexo(const pessoa_t *p);

/** @brief Retorna a data de nascimento (dd/mm/aaaa). NULL quando @p p é NULL. */
const char *pessoa_obter_nasc(const pessoa_t *p);

/**
 * @brief Serializa a pessoa para um registro plano.
 *
 * @param p Pessoa de origem (não pode ser NULL; comportamento indefinido se for).
 * @return Registro plano preenchido.
 */
pessoa_registro_t pessoa_para_registro(const pessoa_t *p);

/**
 * @brief Reconstrói uma pessoa a partir de um registro plano.
 *
 * @param reg Registro de origem (não pode ser NULL).
 * @return Nova instância ou NULL se @p reg for NULL ou contiver dados inválidos.
 */
pessoa_t *pessoa_criar_de_registro(const pessoa_registro_t *reg);

#endif // PESSOA_H
