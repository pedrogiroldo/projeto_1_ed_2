#include "habitante.h"

#include <stdlib.h>
#include <string.h>

struct habitante {
    pessoa_registro_t pessoa;
    int  is_morador;
    char cep[HABITANTE_CEP_MAX + 1u];
    char face;
    int  num;
    char compl[HABITANTE_COMPL_MAX + 1u];
};

static int face_valida(char face) {
    return face == 'N' || face == 'S' || face == 'L' || face == 'O';
}

habitante_t *habitante_criar(const pessoa_registro_t *pessoa_reg) {
    habitante_t *h;

    if (pessoa_reg == NULL) {
        return NULL;
    }

    h = (habitante_t *)calloc(1u, sizeof(*h));
    if (h == NULL) {
        return NULL;
    }

    h->pessoa = *pessoa_reg;
    return h;
}

void habitante_destruir(habitante_t *h) {
    free(h);
}

void habitante_definir_endereco(habitante_t *h, const char *cep, char face,
                                int num, const char *compl) {
    if (h == NULL || cep == NULL || strlen(cep) == 0u || !face_valida(face) ||
        num < 0) {
        return;
    }

    strncpy(h->cep, cep, HABITANTE_CEP_MAX);
    h->cep[HABITANTE_CEP_MAX] = '\0';
    h->face = face;
    h->num = num;

    if (compl != NULL) {
        strncpy(h->compl, compl, HABITANTE_COMPL_MAX);
        h->compl[HABITANTE_COMPL_MAX] = '\0';
    } else {
        h->compl[0] = '\0';
    }

    h->is_morador = 1;
}

void habitante_remover_endereco(habitante_t *h) {
    if (h == NULL) {
        return;
    }
    h->is_morador = 0;
    memset(h->cep, 0, sizeof(h->cep));
    h->face = '\0';
    h->num = 0;
    memset(h->compl, 0, sizeof(h->compl));
}

int habitante_eh_morador(const habitante_t *h) {
    return h != NULL && h->is_morador;
}

const char *habitante_obter_cpf(const habitante_t *h) {
    return h != NULL ? h->pessoa.cpf : NULL;
}

const char *habitante_obter_nome(const habitante_t *h) {
    return h != NULL ? h->pessoa.nome : NULL;
}

const char *habitante_obter_sobrenome(const habitante_t *h) {
    return h != NULL ? h->pessoa.sobrenome : NULL;
}

char habitante_obter_sexo(const habitante_t *h) {
    return h != NULL ? h->pessoa.sexo : '\0';
}

const char *habitante_obter_nasc(const habitante_t *h) {
    return h != NULL ? h->pessoa.nasc : NULL;
}

const char *habitante_obter_cep(const habitante_t *h) {
    if (h == NULL || !h->is_morador) return NULL;
    return h->cep;
}

char habitante_obter_face(const habitante_t *h) {
    if (h == NULL || !h->is_morador) return '\0';
    return h->face;
}

int habitante_obter_num(const habitante_t *h) {
    if (h == NULL || !h->is_morador) return 0;
    return h->num;
}

const char *habitante_obter_compl(const habitante_t *h) {
    if (h == NULL || !h->is_morador) return NULL;
    return h->compl;
}

habitante_registro_t habitante_para_registro(const habitante_t *h) {
    habitante_registro_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.pessoa = h->pessoa;
    reg.is_morador = h->is_morador;
    strncpy(reg.cep, h->cep, HABITANTE_CEP_MAX);
    reg.face = h->face;
    reg.num = h->num;
    strncpy(reg.compl, h->compl, HABITANTE_COMPL_MAX);
    return reg;
}

habitante_t *habitante_criar_de_registro(const habitante_registro_t *reg) {
    habitante_t *h;

    if (reg == NULL) {
        return NULL;
    }

    h = habitante_criar(&reg->pessoa);
    if (h == NULL) {
        return NULL;
    }

    if (reg->is_morador) {
        habitante_definir_endereco(h, reg->cep, reg->face, reg->num,
                                   reg->compl);
    }

    return h;
}
