#ifndef PESSOA_H
#define PESSOA_H

/*
 * Representação de um habitante de Bitnópolis.
 *
 * pessoa_t é um tipo opaco; use pessoa_criar() para instanciar e
 * pessoa_destruir() para liberar. Para persistência em hashfile use
 * pessoa_para_registro() / pessoa_criar_de_registro().
 */

#define PESSOA_CPF_MAX       11u   /* somente dígitos, sem pontuação */
#define PESSOA_NOME_MAX      64u
#define PESSOA_NASC_MAX      10u   /* formato dd/mm/aaaa */

typedef struct pessoa pessoa_t;

/* Registro plano e serializável, adequado para armazenamento em hashfile. */
typedef struct {
    char cpf[PESSOA_CPF_MAX + 1u];
    char nome[PESSOA_NOME_MAX + 1u];
    char sobrenome[PESSOA_NOME_MAX + 1u];
    char sexo;                          /* 'M' ou 'F' */
    char nasc[PESSOA_NASC_MAX + 1u];
} pessoa_registro_t;

/*
 * Cria uma pessoa com os dados fornecidos.
 *
 * Todos os ponteiros devem ser não-NULL e não-vazios. sexo deve ser 'M' ou 'F'.
 * cpf deve ter entre 1 e PESSOA_CPF_MAX caracteres.
 * Retorna NULL se qualquer argumento for inválido ou se faltar memória.
 */
pessoa_t *pessoa_criar(const char *cpf, const char *nome,
                       const char *sobrenome, char sexo, const char *nasc);

/* Libera todos os recursos associados à pessoa. Aceita NULL com segurança. */
void pessoa_destruir(pessoa_t *p);

/* Getters — retornam NULL / '\0' quando p é NULL. */
const char *pessoa_obter_cpf(const pessoa_t *p);
const char *pessoa_obter_nome(const pessoa_t *p);
const char *pessoa_obter_sobrenome(const pessoa_t *p);
char        pessoa_obter_sexo(const pessoa_t *p);
const char *pessoa_obter_nasc(const pessoa_t *p);

/*
 * Serializa a pessoa para um registro plano.
 * p não pode ser NULL; comportamento indefinido se for.
 */
pessoa_registro_t pessoa_para_registro(const pessoa_t *p);

/*
 * Reconstrói uma pessoa a partir de um registro.
 * Retorna NULL se reg for NULL ou contiver dados inválidos.
 */
pessoa_t *pessoa_criar_de_registro(const pessoa_registro_t *reg);

#endif
