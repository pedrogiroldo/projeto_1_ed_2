#ifndef HABITANTE_H
#define HABITANTE_H

/*
 * Habitante de Bitnópolis — pessoa com endereço opcional.
 *
 * Um habitante sem endereço é chamado de "sem-teto". Um habitante com
 * endereço registrado é chamado de "morador". Use habitante_definir_endereco()
 * para tornar um habitante morador e habitante_remover_endereco() para
 * transformá-lo em sem-teto.
 *
 * habitante_t é opaco; use o registro plano habitante_registro_t para
 * persistência em hashfile (chave = CPF).
 */

#include "../pessoa/pessoa.h"

#define HABITANTE_CEP_MAX    32u
#define HABITANTE_COMPL_MAX  32u

typedef struct habitante habitante_t;

/*
 * Registro plano para armazenamento no hashfile de habitantes.
 * is_morador == 0 indica sem-teto; campos de endereço ficam zerados.
 * face válida: 'N', 'S', 'L', 'O' ou '\0' quando sem endereço.
 */
typedef struct {
    pessoa_registro_t pessoa;
    int  is_morador;
    char cep[HABITANTE_CEP_MAX + 1u];
    char face;
    int  num;
    char compl[HABITANTE_COMPL_MAX + 1u];
} habitante_registro_t;

/*
 * Cria um habitante (sem-teto) a partir de um registro de pessoa.
 * pessoa_reg não pode ser NULL. Retorna NULL se for NULL ou se faltar memória.
 * Não requer pessoa.c ao linkar — usa apenas o struct pessoa_registro_t.
 */
habitante_t *habitante_criar(const pessoa_registro_t *pessoa_reg);

/* Libera todos os recursos. Aceita NULL com segurança. */
void habitante_destruir(habitante_t *h);

/*
 * Define o endereço do habitante, tornando-o morador.
 * face deve ser 'N', 'S', 'L' ou 'O'. num deve ser >= 0.
 * Chamada inválida (h NULL, cep vazio, face ruim) é ignorada silenciosamente.
 */
void habitante_definir_endereco(habitante_t *h, const char *cep, char face,
                                int num, const char *compl);

/* Remove o endereço do habitante, tornando-o sem-teto. */
void habitante_remover_endereco(habitante_t *h);

/* Retorna 1 se o habitante é morador, 0 se é sem-teto. 0 para NULL. */
int habitante_eh_morador(const habitante_t *h);

/* Getters de pessoa. */
const char *habitante_obter_cpf(const habitante_t *h);
const char *habitante_obter_nome(const habitante_t *h);
const char *habitante_obter_sobrenome(const habitante_t *h);
char        habitante_obter_sexo(const habitante_t *h);
const char *habitante_obter_nasc(const habitante_t *h);

/* Getters de endereço — retornam NULL / '\0' / 0 quando sem endereço. */
const char *habitante_obter_cep(const habitante_t *h);
char        habitante_obter_face(const habitante_t *h);
int         habitante_obter_num(const habitante_t *h);
const char *habitante_obter_compl(const habitante_t *h);

/*
 * Serializa o habitante para um registro plano.
 * h não pode ser NULL; comportamento indefinido se for.
 */
habitante_registro_t habitante_para_registro(const habitante_t *h);

/*
 * Reconstrói um habitante a partir de um registro.
 * Retorna NULL se reg for NULL ou contiver dados inválidos.
 */
habitante_t *habitante_criar_de_registro(const habitante_registro_t *reg);

#endif
