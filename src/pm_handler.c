#include "pm_handler.h"

#include "file_reader.h"
#include "habitante.h"
#include "quadra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PM_CPF_MAX    16
#define PM_NOME_MAX   80
#define PM_NASC_MAX   16
#define PM_CEP_MAX    64
#define PM_COMPL_MAX  64

static void pm_processar_p(const char *linha,
                           extensible_hash_file_t habitantes_hf,
                           pm_handler_resultado_t *res) {
    char cpf[PM_CPF_MAX];
    char nome[PM_NOME_MAX];
    char sobrenome[PM_NOME_MAX];
    char sexo_str[4];
    char nasc[PM_NASC_MAX];
    int parsed;
    char sexo;
    pessoa_t *p;
    pessoa_registro_t preg;
    habitante_t *h;
    habitante_registro_t hreg;
    ehf_status_t status;

    parsed = sscanf(linha, "p %15s %79s %79s %3s %15s",
                    cpf, nome, sobrenome, sexo_str, nasc);
    if (parsed != 5) {
        res->erros++;
        return;
    }

    sexo = sexo_str[0];
    p = pessoa_criar(cpf, nome, sobrenome, sexo, nasc);
    if (p == NULL) {
        res->erros++;
        return;
    }

    preg = pessoa_para_registro(p);
    pessoa_destruir(p);

    h = habitante_criar(&preg);
    if (h == NULL) {
        res->erros++;
        return;
    }

    hreg = habitante_para_registro(h);
    habitante_destruir(h);

    status = ehf_insert(habitantes_hf, cpf, &hreg, sizeof(hreg));
    if (status == EHF_OK) {
        res->pessoas_inseridas++;
    } else {
        res->erros++;
    }
}

static void pm_processar_m(const char *linha,
                           extensible_hash_file_t habitantes_hf,
                           extensible_hash_file_t quadras_hf,
                           pm_handler_resultado_t *res) {
    char cpf[PM_CPF_MAX];
    char cep[PM_CEP_MAX];
    char face_str[4];
    int num;
    char compl[PM_COMPL_MAX];
    int parsed;
    char face;
    habitante_registro_t hreg;
    quadra_registro_t qreg;
    habitante_t *h;
    ehf_status_t status;

    parsed = sscanf(linha, "m %15s %63s %3s %d %63s",
                    cpf, cep, face_str, &num, compl);
    if (parsed < 4) {
        res->erros++;
        return;
    }
    if (parsed == 4) {
        compl[0] = '\0';
    }

    face = face_str[0];

    if (ehf_find(quadras_hf, cep, &qreg, sizeof(qreg)) != EHF_OK) {
        res->erros++;
        return;
    }

    if (ehf_find(habitantes_hf, cpf, &hreg, sizeof(hreg)) != EHF_OK) {
        res->erros++;
        return;
    }

    h = habitante_criar_de_registro(&hreg);
    if (h == NULL) {
        res->erros++;
        return;
    }

    habitante_definir_endereco(h, cep, face, num, compl);
    if (!habitante_eh_morador(h)) {
        habitante_destruir(h);
        res->erros++;
        return;
    }

    hreg = habitante_para_registro(h);
    habitante_destruir(h);

    status = ehf_remove(habitantes_hf, cpf);
    if (status != EHF_OK) {
        res->erros++;
        return;
    }

    status = ehf_insert(habitantes_hf, cpf, &hreg, sizeof(hreg));
    if (status == EHF_OK) {
        res->moradores_registrados++;
    } else {
        res->erros++;
    }
}

pm_handler_resultado_t pm_handler_processar(
    const char *pm_filepath,
    extensible_hash_file_t habitantes_hf,
    extensible_hash_file_t quadras_hf) {
    pm_handler_resultado_t res;
    FileData fd;
    Queue linhas;
    char *linha;

    memset(&res, 0, sizeof(res));

    if (pm_filepath == NULL || habitantes_hf == NULL || quadras_hf == NULL) {
        res.erros++;
        return res;
    }

    fd = file_data_create(pm_filepath);
    if (fd == NULL) {
        res.erros++;
        return res;
    }

    linhas = get_file_lines_queue(fd);

    while (!queue_is_empty(linhas)) {
        linha = (char *)queue_dequeue(linhas);
        if (linha == NULL) {
            continue;
        }

        if (strncmp(linha, "p ", 2) == 0) {
            pm_processar_p(linha, habitantes_hf, &res);
        } else if (strncmp(linha, "m ", 2) == 0) {
            pm_processar_m(linha, habitantes_hf, quadras_hf, &res);
        }
    }

    file_data_destroy(fd);
    return res;
}
