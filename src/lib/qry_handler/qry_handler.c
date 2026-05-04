#include "qry_handler.h"

#include "../file_reader/file_reader.h"
#include "../habitante/habitante.h"
#include "../pessoa/pessoa.h"
#include "../quadra/quadra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define QRY_CPF_MAX   16
#define QRY_CEP_MAX   64
#define QRY_NOME_MAX  80
#define QRY_NASC_MAX  16
#define QRY_COMPL_MAX 64

/* ---- helpers ---- */

typedef struct {
    double x;
    double y;
} qry_ponto_t;

static qry_ponto_t endereco_para_ponto(const quadra_registro_t *qreg,
                                       char face, int num) {
    qry_ponto_t p;

    p.x = 0.0;
    p.y = 0.0;

    if (qreg == NULL) {
        return p;
    }

    switch (face) {
        case 'N':
            p.x = qreg->x + (double)num;
            p.y = qreg->y + qreg->altura;
            break;
        case 'S':
            p.x = qreg->x + (double)num;
            p.y = qreg->y;
            break;
        case 'L':
            p.x = qreg->x;
            p.y = qreg->y + (double)num;
            break;
        case 'O':
            p.x = qreg->x + qreg->largura;
            p.y = qreg->y + (double)num;
            break;
        default:
            p.x = qreg->x;
            p.y = qreg->y;
            break;
    }

    return p;
}

static double pct(int parte, int total) {
    return total == 0 ? 0.0 : ((double)parte * 100.0) / (double)total;
}

typedef struct {
    int total;
    int moradores;
    int sem_teto;
    int homens;
    int mulheres;
    int moradores_homens;
    int moradores_mulheres;
    int sem_teto_homens;
    int sem_teto_mulheres;
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
        if (hreg->pessoa.sexo == 'M') {
            ctx->moradores_homens++;
        } else {
            ctx->moradores_mulheres++;
        }
    } else {
        ctx->sem_teto++;
        if (hreg->pessoa.sexo == 'M') {
            ctx->sem_teto_homens++;
        } else {
            ctx->sem_teto_mulheres++;
        }
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
    char (*cpfs)[QRY_CPF_MAX];
    size_t quantidade;
    size_t capacidade;
} rq_ctx_t;

static void rq_visitor(const char *key, const void *record,
                       size_t record_size, void *user_data) {
    const habitante_registro_t *hreg = (const habitante_registro_t *)record;
    rq_ctx_t *ctx = (rq_ctx_t *)user_data;
    (void)record_size;

    if (!hreg->is_morador || strcmp(hreg->cep, ctx->cep_alvo) != 0) return;

    if (ctx->quantidade >= ctx->capacidade) {
        size_t nova_capacidade = ctx->capacidade == 0u ? 16u : ctx->capacidade * 2u;
        char (*novos_cpfs)[QRY_CPF_MAX];

        novos_cpfs = (char (*)[QRY_CPF_MAX])realloc(
            ctx->cpfs, nova_capacidade * sizeof(*ctx->cpfs));
        if (novos_cpfs == NULL) {
            return;
        }

        ctx->cpfs = novos_cpfs;
        ctx->capacidade = nova_capacidade;
    }

    strncpy(ctx->cpfs[ctx->quantidade], key, QRY_CPF_MAX - 1u);
    ctx->cpfs[ctx->quantidade][QRY_CPF_MAX - 1u] = '\0';
    ctx->quantidade++;
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
    size_t i;

    if (sscanf(linha, "rq %63s", cep) != 1) { res->erros++; return; }

    if (ehf_find(quadras_hf, cep, &qreg, sizeof(qreg)) != EHF_OK) {
        res->erros++;
        return;
    }

    memset(&ctx, 0, sizeof(ctx));
    ctx.cep_alvo = cep;

    ehf_foreach(habitantes_hf, rq_visitor, sizeof(habitante_registro_t), &ctx);

    for (i = 0u; i < ctx.quantidade; ++i) {
        habitante_registro_t hreg;
        habitante_t *h;

        if (ehf_find(habitantes_hf, ctx.cpfs[i], &hreg, sizeof(hreg)) != EHF_OK) {
            continue;
        }

        txt_writer_linha(txt, "rq %s: morador removido cpf=%s nome=%s %s",
                         cep, ctx.cpfs[i],
                         hreg.pessoa.nome, hreg.pessoa.sobrenome);

        h = habitante_criar_de_registro(&hreg);
        if (h == NULL) {
            res->erros++;
            continue;
        }

        habitante_remover_endereco(h);
        hreg = habitante_para_registro(h);
        habitante_destruir(h);

        if (ehf_remove(habitantes_hf, ctx.cpfs[i]) == EHF_OK) {
            ehf_insert(habitantes_hf, ctx.cpfs[i], &hreg, sizeof(hreg));
        }
    }

    free(ctx.cpfs);

    if (svg != NULL) {
        svg_writer_x_vermelho(svg, qreg.x, qreg.y, 10.0);
    }

    ehf_remove(quadras_hf, cep);
    res->comandos_processados++;
}

static void cmd_pq(const char *linha,
                   extensible_hash_file_t quadras_hf,
                   extensible_hash_file_t habitantes_hf,
                   svg_writer_t *svg, txt_writer_t *txt,
                   qry_handler_resultado_t *res) {
    char cep[QRY_CEP_MAX];
    pq_ctx_t ctx;
    quadra_registro_t qreg;
    int total;
    char buf[32];

    if (sscanf(linha, "pq %63s", cep) != 1) { res->erros++; return; }

    if (ehf_find(quadras_hf, cep, &qreg, sizeof(qreg)) != EHF_OK) {
        res->erros++;
        return;
    }

    memset(&ctx, 0, sizeof(ctx));
    ctx.cep = cep;

    ehf_foreach(habitantes_hf, pq_visitor, sizeof(habitante_registro_t), &ctx);

    total = ctx.norte + ctx.sul + ctx.leste + ctx.oeste;
    txt_writer_linha(txt,
        "pq %s: N=%d S=%d L=%d O=%d total=%d",
        cep, ctx.norte, ctx.sul, ctx.leste, ctx.oeste, total);

    if (svg != NULL) {
        snprintf(buf, sizeof(buf), "%d", ctx.norte);
        svg_writer_texto(svg, qreg.x + qreg.largura / 2.0,
                         qreg.y + qreg.altura + 12.0, buf, "10", "black");
        snprintf(buf, sizeof(buf), "%d", ctx.sul);
        svg_writer_texto(svg, qreg.x + qreg.largura / 2.0, qreg.y - 4.0,
                         buf, "10", "black");
        snprintf(buf, sizeof(buf), "%d", ctx.leste);
        svg_writer_texto(svg, qreg.x - 12.0,
                         qreg.y + qreg.altura / 2.0, buf, "10", "black");
        snprintf(buf, sizeof(buf), "%d", ctx.oeste);
        svg_writer_texto(svg, qreg.x + qreg.largura + 4.0,
                         qreg.y + qreg.altura / 2.0, buf, "10", "black");
        snprintf(buf, sizeof(buf), "%d", total);
        svg_writer_texto(svg, qreg.x + qreg.largura / 2.0,
                         qreg.y + qreg.altura / 2.0, buf, "12", "black");
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
                     "homens=%d mulheres=%d "
                     "prop_moradores_habitantes=%.2f%% "
                     "habitantes_homens=%.2f%% habitantes_mulheres=%.2f%% "
                     "moradores_homens=%.2f%% moradores_mulheres=%.2f%% "
                     "sem_tetos_homens=%.2f%% sem_tetos_mulheres=%.2f%%",
                     ctx.total, ctx.moradores, ctx.sem_teto,
                     ctx.homens, ctx.mulheres,
                     pct(ctx.moradores, ctx.total),
                     pct(ctx.homens, ctx.total),
                     pct(ctx.mulheres, ctx.total),
                     pct(ctx.moradores_homens, ctx.moradores),
                     pct(ctx.moradores_mulheres, ctx.moradores),
                     pct(ctx.sem_teto_homens, ctx.sem_teto),
                     pct(ctx.sem_teto_mulheres, ctx.sem_teto));
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
                    extensible_hash_file_t quadras_hf,
                    extensible_hash_file_t habitantes_hf,
                    svg_writer_t *svg, txt_writer_t *txt,
                    qry_handler_resultado_t *res) {
    char cpf[QRY_CPF_MAX];
    habitante_registro_t hreg;
    quadra_registro_t qreg;

    if (sscanf(linha, "rip %15s", cpf) != 1) { res->erros++; return; }

    if (ehf_find(habitantes_hf, cpf, &hreg, sizeof(hreg)) != EHF_OK) {
        txt_writer_linha(txt, "rip %s: ja removido", cpf);
        res->comandos_processados++;
        return;
    }

    if (hreg.is_morador) {
        txt_writer_linha(txt,
            "rip cpf=%s nome=%s %s sexo=%c nasc=%s endereco=%s/%c/%d %s",
            hreg.pessoa.cpf, hreg.pessoa.nome, hreg.pessoa.sobrenome,
            hreg.pessoa.sexo, hreg.pessoa.nasc,
            hreg.cep, hreg.face, hreg.num, hreg.compl);
    } else {
        txt_writer_linha(txt,
            "rip cpf=%s nome=%s %s sexo=%c nasc=%s sem-teto",
            hreg.pessoa.cpf, hreg.pessoa.nome, hreg.pessoa.sobrenome,
            hreg.pessoa.sexo, hreg.pessoa.nasc);
    }

    if (svg != NULL && hreg.is_morador &&
        ehf_find(quadras_hf, hreg.cep, &qreg, sizeof(qreg)) == EHF_OK) {
        qry_ponto_t p = endereco_para_ponto(&qreg, hreg.face, hreg.num);
        svg_writer_cruz_vermelha(svg, p.x, p.y, 10.0);
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
        qry_ponto_t p = endereco_para_ponto(&qreg, face, num);
        svg_writer_quadrado_cpf(svg, p.x, p.y, 20.0, cpf);
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
                     extensible_hash_file_t quadras_hf,
                     extensible_hash_file_t habitantes_hf,
                     svg_writer_t *svg, txt_writer_t *txt,
                     qry_handler_resultado_t *res) {
    char cpf[QRY_CPF_MAX];
    habitante_registro_t hreg;
    quadra_registro_t qreg;
    habitante_t *h;

    if (sscanf(linha, "dspj %15s", cpf) != 1) { res->erros++; return; }

    if (ehf_find(habitantes_hf, cpf, &hreg, sizeof(hreg)) != EHF_OK) {
        txt_writer_linha(txt, "dspj %s: ja sem endereco (inexistente)", cpf);
        res->comandos_processados++;
        return;
    }

    if (!hreg.is_morador) {
        txt_writer_linha(txt, "dspj %s: ja sem endereco", cpf);
        res->comandos_processados++;
        return;
    }

    txt_writer_linha(txt,
        "dspj cpf=%s nome=%s %s sexo=%c nasc=%s endereco=%s/%c/%d %s",
        hreg.pessoa.cpf, hreg.pessoa.nome, hreg.pessoa.sobrenome,
        hreg.pessoa.sexo, hreg.pessoa.nasc,
        hreg.cep, hreg.face, hreg.num, hreg.compl);

    if (svg != NULL &&
        ehf_find(quadras_hf, hreg.cep, &qreg, sizeof(qreg)) == EHF_OK) {
        qry_ponto_t p = endereco_para_ponto(&qreg, hreg.face, hreg.num);
        svg_writer_circulo_preto(svg, p.x, p.y, 5.0);
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

static int linha_eh_ignorada(const char *linha) {
    size_t i = 0u;
    if (linha == NULL) return 1;

    while (linha[i] != '\0' && isspace((unsigned char)linha[i])) {
        i++;
    }

    if (linha[i] == '\0') return 1;
    if (linha[i] == '#') return 1;

    return 0;
}

static int linha_eh_censo(const char *linha) {
    size_t i = 0u;
    size_t j = 0u;
    const char *cmd = "censo";

    while (linha[i] != '\0' && isspace((unsigned char)linha[i])) {
        i++;
    }

    while (cmd[j] != '\0') {
        if (linha[i + j] != cmd[j]) return 0;
        j++;
    }

    i += j;
    while (linha[i] != '\0' && isspace((unsigned char)linha[i])) {
        i++;
    }

    return linha[i] == '\0';
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
        if (linha_eh_ignorada(linha)) continue;

        if (strncmp(linha, "rq ", 3) == 0) {
            cmd_rq(linha, quadras_hf, habitantes_hf, svg, txt, &res);
        } else if (strncmp(linha, "pq ", 3) == 0) {
            cmd_pq(linha, quadras_hf, habitantes_hf, svg, txt, &res);
        } else if (linha_eh_censo(linha)) {
            cmd_censo(habitantes_hf, txt, &res);
        } else if (strncmp(linha, "h? ", 3) == 0) {
            cmd_h_query(linha, habitantes_hf, txt, &res);
        } else if (strncmp(linha, "nasc ", 5) == 0) {
            cmd_nasc(linha, habitantes_hf, txt, &res);
        } else if (strncmp(linha, "rip ", 4) == 0) {
            cmd_rip(linha, quadras_hf, habitantes_hf, svg, txt, &res);
        } else if (strncmp(linha, "mud ", 4) == 0) {
            cmd_mud(linha, quadras_hf, habitantes_hf, svg, txt, &res);
        } else if (strncmp(linha, "dspj ", 5) == 0) {
            cmd_dspj(linha, quadras_hf, habitantes_hf, svg, txt, &res);
        }
    }

    file_data_destroy(fd);
    return res;
}
