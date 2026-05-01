#include "pessoa.h"

#include <stdlib.h>
#include <string.h>

struct pessoa {
    char cpf[PESSOA_CPF_MAX + 1u];
    char nome[PESSOA_NOME_MAX + 1u];
    char sobrenome[PESSOA_NOME_MAX + 1u];
    char sexo;
    char nasc[PESSOA_NASC_MAX + 1u];
};

static int pessoa_sexo_valido(char sexo) {
    return sexo == 'M' || sexo == 'F';
}

static int pessoa_str_valida(const char *s, size_t max) {
    size_t len;
    if (s == NULL) return 0;
    len = strlen(s);
    return len > 0u && len <= max;
}

pessoa_t *pessoa_criar(const char *cpf, const char *nome,
                       const char *sobrenome, char sexo, const char *nasc) {
    pessoa_t *p;

    if (!pessoa_str_valida(cpf, PESSOA_CPF_MAX) ||
        !pessoa_str_valida(nome, PESSOA_NOME_MAX) ||
        !pessoa_str_valida(sobrenome, PESSOA_NOME_MAX) ||
        !pessoa_sexo_valido(sexo) ||
        !pessoa_str_valida(nasc, PESSOA_NASC_MAX)) {
        return NULL;
    }

    p = (pessoa_t *)calloc(1u, sizeof(*p));
    if (p == NULL) {
        return NULL;
    }

    strncpy(p->cpf, cpf, PESSOA_CPF_MAX);
    strncpy(p->nome, nome, PESSOA_NOME_MAX);
    strncpy(p->sobrenome, sobrenome, PESSOA_NOME_MAX);
    p->sexo = sexo;
    strncpy(p->nasc, nasc, PESSOA_NASC_MAX);

    return p;
}

void pessoa_destruir(pessoa_t *p) {
    free(p);
}

const char *pessoa_obter_cpf(const pessoa_t *p) {
    return p != NULL ? p->cpf : NULL;
}

const char *pessoa_obter_nome(const pessoa_t *p) {
    return p != NULL ? p->nome : NULL;
}

const char *pessoa_obter_sobrenome(const pessoa_t *p) {
    return p != NULL ? p->sobrenome : NULL;
}

char pessoa_obter_sexo(const pessoa_t *p) {
    return p != NULL ? p->sexo : '\0';
}

const char *pessoa_obter_nasc(const pessoa_t *p) {
    return p != NULL ? p->nasc : NULL;
}

pessoa_registro_t pessoa_para_registro(const pessoa_t *p) {
    pessoa_registro_t reg;
    memset(&reg, 0, sizeof(reg));
    strncpy(reg.cpf, p->cpf, PESSOA_CPF_MAX);
    strncpy(reg.nome, p->nome, PESSOA_NOME_MAX);
    strncpy(reg.sobrenome, p->sobrenome, PESSOA_NOME_MAX);
    reg.sexo = p->sexo;
    strncpy(reg.nasc, p->nasc, PESSOA_NASC_MAX);
    return reg;
}

pessoa_t *pessoa_criar_de_registro(const pessoa_registro_t *reg) {
    if (reg == NULL) {
        return NULL;
    }
    return pessoa_criar(reg->cpf, reg->nome, reg->sobrenome, reg->sexo,
                        reg->nasc);
}
