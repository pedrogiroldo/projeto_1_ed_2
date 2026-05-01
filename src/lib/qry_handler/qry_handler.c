#include "qry_handler.h"

#include "../file_reader/file_reader.h"
#include "../habitante/habitante.h"
#include "../pessoa/pessoa.h"
#include "../quadra/quadra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QRY_CPF_MAX   16
#define QRY_CEP_MAX   64
#define QRY_NOME_MAX  80
#define QRY_NASC_MAX  16
#define QRY_COMPL_MAX 64

/* ---- helpers ---- */

typedef struct {
    int total;
    int moradores;
    int sem_teto;
    int homens;
    int mulheres;
} censo_ctx_t;

typedef struct {
    const char *cep;
    int norte;
    int sul;
    int leste;
    int oeste;
} pq_ctx_t;

static void censo_visitor(const char *key, const void *record,
                          size_t record_size, void *user_data) {
    const habitante_registro_t *hreg = (const habitante_registro_t *)record;
    censo_ctx_t *ctx = (censo_ctx_t *)user_data;
    (void)key;
    (void)record_size;

    ctx->total++;
    if (hreg->is_morador) {
        ctx->moradores++;
    } else {
        ctx->sem_teto++;
    }
    if (hreg->pessoa.sexo == 'M') {
        ctx->homens++;
    } else {
        ctx->mulheres++;
    }
}

static void pq_visitor(const char *key, const void *record,
                       size_t record_size, void *user_data) {
    const habitante_registro_t *hreg = (const habitante_registro_t *)record;
    pq_ctx_t *ctx = (pq_ctx_t *)user_data;
    (void)key;
    (void)record_size;

    if (!hreg->is_morador) return;
    if (strcmp(hreg->cep, ctx->cep) != 0) return;

    switch (hreg->face) {
        case 'N': ctx->norte++;  break;
        case 'S': ctx->sul++;    break;
        case 'L': ctx->leste++;  break;
        case 'O': ctx->oeste++;  break;
        default: break;
    }
}

typedef struct {
    const char *cep_alvo;
    extensible_hash_file_t habitantes_hf;
    txt_writer_t *txt;
    svg_writer_t *svg;
    quadra_registro_t *qreg;
} rq_ctx_t;

static void rq_visitor(const char *key, const void *record,
                       size_t record_size, void *user_data) {
    const habitante_registro_t *hreg = (const habitante_registro_t *)record;
    rq_ctx_t *ctx = (rq_ctx_t *)user_data;
    habitante_t *h;
    habitante_registro_t updated;
    (void)record_size;

    if (!hreg->is_morador || strcmp(hreg->cep, ctx->cep_alvo) != 0) return;

    txt_writer_linha(ctx->txt, "rq %s: morador removido cpf=%s nome=%s %s",
                     ctx->cep_alvo, key,
                     hreg->pessoa.nome, hreg->pessoa.sobrenome);

    if (ctx->svg != NULL && ctx->qreg != NULL) {
        double ax = ctx->qreg->x + ctx->qreg->largura;
        double ay = ctx->qreg->y + ctx->qreg->altura;
        svg_writer_x_vermelho(ctx->svg, ax, ay, 10.0);
    }

    h = habitante_criar_de_registro(hreg);
    if (h == NULL) return;

    habitante_remover_endereco(h);
    updated = habitante_para_registro(h);
    habitante_destruir(h);

    ehf_remove(ctx->habitantes_hf, key);
    ehf_insert(ctx->habitantes_hf, key, &updated, sizeof(updated));
}

/* ---- command handlers ---- */

static void cmd_rq(const char *linha,
                   extensible_hash_file_t quadras_hf,
                   extensible_hash_file_t habitantes_hf,
                   svg_writer_t *svg, txt_writer_t *txt,
                   qry_handler_resultado_t *res) {
    char cep[QRY_CEP_MAX];
    quadra_registro_t qreg;
    rq_ctx_t ctx;

    if (sscanf(linha, "rq %63s", cep) != 1) { res->erros++; return; }

    if (ehf_find(quadras_hf, cep, &qreg, sizeof(qreg)) != EHF_OK) {
        res->erros++;
        return;
    }

    ctx.cep_alvo     = cep;
    ctx.habitantes_hf = habitantes_hf;
    ctx.txt          = txt;
    ctx.svg          = svg;
    ctx.qreg         = &qreg;

    ehf_foreach(habitantes_hf, rq_visitor, sizeof(habitante_registro_t), &ctx);
    ehf_remove(quadras_hf, cep);
    res->comandos_processados++;
}

static void cmd_pq(const char *linha,
                   extensible_hash_file_t habitantes_hf,
                   svg_writer_t *svg, txt_writer_t *txt,
                   qry_handler_resultado_t *res) {
    char cep[QRY_CEP_MAX];
    pq_ctx_t ctx;
    int total;

    if (sscanf(linha, "pq %63s", cep) != 1) { res->erros++; return; }

    memset(&ctx, 0, sizeof(ctx));
    ctx.cep = cep;

    ehf_foreach(habitantes_hf, pq_visitor, sizeof(habitante_registro_t), &ctx);

    total = ctx.norte + ctx.sul + ctx.leste + ctx.oeste;
    txt_writer_linha(txt,
        "pq %s: N=%d S=%d L=%d O=%d total=%d",
        cep, ctx.norte, ctx.sul, ctx.leste, ctx.oeste, total);

    if (svg != NULL) {
        (void)svg;
    }
    res->comandos_processados++;
}

static void cmd_censo(extensible_hash_file_t habitantes_hf,
                      txt_writer_t *txt,
                      qry_handler_resultado_t *res) {
    censo_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    ehf_foreach(habitantes_hf, censo_visitor, sizeof(habitante_registro_t), &ctx);

    txt_writer_linha(txt, "censo: total=%d moradores=%d sem_teto=%d "
                     "homens=%d mulheres=%d",
                     ctx.total, ctx.moradores, ctx.sem_teto,
                     ctx.homens, ctx.mulheres);
    res->comandos_processados++;
}

static void cmd_h_query(const char *linha,
                        extensible_hash_file_t habitantes_hf,
                        txt_writer_t *txt,
                        qry_handler_resultado_t *res) {
    char cpf[QRY_CPF_MAX];
    habitante_registro_t hreg;

    if (sscanf(linha, "h? %15s", cpf) != 1) { res->erros++; return; }

    if (ehf_find(habitantes_hf, cpf, &hreg, sizeof(hreg)) != EHF_OK) {
        txt_writer_linha(txt, "h? %s: nao encontrado", cpf);
        res->erros++;
        return;
    }

    if (hreg.is_morador) {
        txt_writer_linha(txt,
            "h? cpf=%s nome=%s %s sexo=%c nasc=%s endereco=%s/%c/%d %s",
            hreg.pessoa.cpf, hreg.pessoa.nome, hreg.pessoa.sobrenome,
            hreg.pessoa.sexo, hreg.pessoa.nasc,
            hreg.cep, hreg.face, hreg.num, hreg.compl);
    } else {
        txt_writer_linha(txt,
            "h? cpf=%s nome=%s %s sexo=%c nasc=%s sem-teto",
            hreg.pessoa.cpf, hreg.pessoa.nome, hreg.pessoa.sobrenome,
            hreg.pessoa.sexo, hreg.pessoa.nasc);
    }
    res->comandos_processados++;
}

static void cmd_nasc(const char *linha,
                     extensible_hash_file_t habitantes_hf,
                     txt_writer_t *txt,
                     qry_handler_resultado_t *res) {
    char cpf[QRY_CPF_MAX];
    char nome[QRY_NOME_MAX];
    char sobrenome[QRY_NOME_MAX];
    char sexo_str[4];
    char nasc[QRY_NASC_MAX];
    int parsed;
    char sexo;
    pessoa_t *p;
    pessoa_registro_t preg;
    habitante_t *h;
    habitante_registro_t hreg;

    parsed = sscanf(linha, "nasc %15s %79s %79s %3s %15s",
                    cpf, nome, sobrenome, sexo_str, nasc);
    if (parsed != 5) { res->erros++; return; }

    sexo = sexo_str[0];
    p = pessoa_criar(cpf, nome, sobrenome, sexo, nasc);
    if (p == NULL) { res->erros++; return; }

    preg = pessoa_para_registro(p);
    pessoa_destruir(p);

    h = habitante_criar(&preg);
    if (h == NULL) { res->erros++; return; }

    hreg = habitante_para_registro(h);
    habitante_destruir(h);

    if (ehf_insert(habitantes_hf, cpf, &hreg, sizeof(hreg)) == EHF_OK) {
        txt_writer_linha(txt, "nasc %s: inserido", cpf);
        res->comandos_processados++;
    } else {
        res->erros++;
    }
}

static void cmd_rip(const char *linha,
                    extensible_hash_file_t habitantes_hf,
                    svg_writer_t *svg, txt_writer_t *txt,
                    qry_handler_resultado_t *res) {
    char cpf[QRY_CPF_MAX];
    habitante_registro_t hreg;

    if (sscanf(linha, "rip %15s", cpf) != 1) { res->erros++; return; }

    if (ehf_find(habitantes_hf, cpf, &hreg, sizeof(hreg)) != EHF_OK) {
        res->erros++;
        return;
    }

    txt_writer_linha(txt, "rip %s: removido nome=%s %s",
                     cpf, hreg.pessoa.nome, hreg.pessoa.sobrenome);

    if (svg != NULL && hreg.is_morador) {
        svg_writer_cruz_vermelha(svg, 0.0, 0.0, 10.0);
    }

    ehf_remove(habitantes_hf, cpf);
    res->comandos_processados++;
}

static void cmd_mud(const char *linha,
                    extensible_hash_file_t quadras_hf,
                    extensible_hash_file_t habitantes_hf,
                    svg_writer_t *svg, txt_writer_t *txt,
                    qry_handler_resultado_t *res) {
    char cpf[QRY_CPF_MAX];
    char cep[QRY_CEP_MAX];
    char face_str[4];
    int num;
    char compl[QRY_COMPL_MAX];
    int parsed;
    char face;
    habitante_registro_t hreg;
    quadra_registro_t qreg;
    habitante_t *h;

    parsed = sscanf(linha, "mud %15s %63s %3s %d %63s",
                    cpf, cep, face_str, &num, compl);
    if (parsed < 4) { res->erros++; return; }
    if (parsed == 4) compl[0] = '\0';

    face = face_str[0];

    if (ehf_find(quadras_hf, cep, &qreg, sizeof(qreg)) != EHF_OK ||
        ehf_find(habitantes_hf, cpf, &hreg, sizeof(hreg)) != EHF_OK) {
        res->erros++;
        return;
    }

    h = habitante_criar_de_registro(&hreg);
    if (h == NULL) { res->erros++; return; }

    habitante_definir_endereco(h, cep, face, num, compl);
    if (!habitante_eh_morador(h)) {
        habitante_destruir(h);
        res->erros++;
        return;
    }

    hreg = habitante_para_registro(h);
    habitante_destruir(h);

    if (svg != NULL) {
        svg_writer_quadrado_cpf(svg, qreg.x + 5.0, qreg.y + 5.0, 20.0, cpf);
    }

    ehf_remove(habitantes_hf, cpf);
    if (ehf_insert(habitantes_hf, cpf, &hreg, sizeof(hreg)) == EHF_OK) {
        txt_writer_linha(txt, "mud %s: novo endereco %s/%c/%d", cpf, cep, face, num);
        res->comandos_processados++;
    } else {
        res->erros++;
    }
}

static void cmd_dspj(const char *linha,
                     extensible_hash_file_t habitantes_hf,
                     svg_writer_t *svg, txt_writer_t *txt,
                     qry_handler_resultado_t *res) {
    char cpf[QRY_CPF_MAX];
    habitante_registro_t hreg;
    habitante_t *h;

    if (sscanf(linha, "dspj %15s", cpf) != 1) { res->erros++; return; }

    if (ehf_find(habitantes_hf, cpf, &hreg, sizeof(hreg)) != EHF_OK) {
        res->erros++;
        return;
    }

    if (!hreg.is_morador) {
        res->erros++;
        return;
    }

    txt_writer_linha(txt,
        "dspj %s: despejado de %s/%c/%d",
        cpf, hreg.cep, hreg.face, hreg.num);

    if (svg != NULL) {
        svg_writer_circulo_preto(svg, 0.0, 0.0, 5.0);
    }

    h = habitante_criar_de_registro(&hreg);
    if (h == NULL) { res->erros++; return; }

    habitante_remover_endereco(h);
    hreg = habitante_para_registro(h);
    habitante_destruir(h);

    ehf_remove(habitantes_hf, cpf);
    if (ehf_insert(habitantes_hf, cpf, &hreg, sizeof(hreg)) == EHF_OK) {
        res->comandos_processados++;
    } else {
        res->erros++;
    }
}

/* ---- public API ---- */

qry_handler_resultado_t qry_handler_processar(
    const char *qry_filepath,
    extensible_hash_file_t quadras_hf,
    extensible_hash_file_t habitantes_hf,
    svg_writer_t *svg,
    txt_writer_t *txt) {
    qry_handler_resultado_t res;
    FileData fd;
    Queue linhas;
    char *linha;

    memset(&res, 0, sizeof(res));

    if (qry_filepath == NULL || quadras_hf == NULL || habitantes_hf == NULL) {
        res.erros++;
        return res;
    }

    fd = file_data_create(qry_filepath);
    if (fd == NULL) {
        res.erros++;
        return res;
    }

    linhas = get_file_lines_queue(fd);

    while (!queue_is_empty(linhas)) {
        linha = (char *)queue_dequeue(linhas);
        if (linha == NULL) continue;

        if (strncmp(linha, "rq ", 3) == 0) {
            cmd_rq(linha, quadras_hf, habitantes_hf, svg, txt, &res);
        } else if (strncmp(linha, "pq ", 3) == 0) {
            cmd_pq(linha, habitantes_hf, svg, txt, &res);
        } else if (strcmp(linha, "censo") == 0) {
            cmd_censo(habitantes_hf, txt, &res);
        } else if (strncmp(linha, "h? ", 3) == 0) {
            cmd_h_query(linha, habitantes_hf, txt, &res);
        } else if (strncmp(linha, "nasc ", 5) == 0) {
            cmd_nasc(linha, habitantes_hf, txt, &res);
        } else if (strncmp(linha, "rip ", 4) == 0) {
            cmd_rip(linha, habitantes_hf, svg, txt, &res);
        } else if (strncmp(linha, "mud ", 4) == 0) {
            cmd_mud(linha, quadras_hf, habitantes_hf, svg, txt, &res);
        } else if (strncmp(linha, "dspj ", 5) == 0) {
            cmd_dspj(linha, habitantes_hf, svg, txt, &res);
        }
    }

    file_data_destroy(fd);
    return res;
}
