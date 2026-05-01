#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/args_handler/args_handler.h"
#include "lib/extensible_hash_file/extensible_hash_file.h"
#include "lib/geo_handler/geo_handler.h"
#include "lib/habitante/habitante.h"
#include "lib/pm_handler/pm_handler.h"
#include "lib/qry_handler/qry_handler.h"
#include "lib/quadra/quadra.h"
#include "lib/svg_writer/svg_writer.h"
#include "lib/txt_writer/txt_writer.h"

#define PATH_MAX_LEN 1024

static void join_path(char *out, size_t out_size,
                      const char *dir, const char *file) {
    snprintf(out, out_size, "%s/%s", dir, file);
}

static void strip_ext(char *buf, size_t buf_size, const char *filename) {
    const char *dot;
    size_t len;

    dot = strrchr(filename, '.');
    len = dot != NULL ? (size_t)(dot - filename) : strlen(filename);
    if (len >= buf_size) len = buf_size - 1u;
    memcpy(buf, filename, len);
    buf[len] = '\0';
}

int main(int argc, char **argv) {
    char *bed;
    char *bsd;
    char *geo_file;
    char *pm_file;
    char *qry_file;

    char geo_path[PATH_MAX_LEN];
    char pm_path[PATH_MAX_LEN];
    char qry_path[PATH_MAX_LEN];
    char quad_hf_path[PATH_MAX_LEN];
    char hab_hf_path[PATH_MAX_LEN];
    char quad_hfd_path[PATH_MAX_LEN];
    char hab_hfd_path[PATH_MAX_LEN];
    char svg_path[PATH_MAX_LEN];
    char txt_path[PATH_MAX_LEN];
    char base[PATH_MAX_LEN];

    extensible_hash_file_t quadras_hf = NULL;
    extensible_hash_file_t habitantes_hf = NULL;
    svg_writer_t *svg = NULL;
    txt_writer_t *txt = NULL;

    geo_handler_resultado_t geo_res;

    bed      = get_option_value(argc, argv, "e");
    bsd      = get_option_value(argc, argv, "o");
    geo_file = get_option_value(argc, argv, "f");
    pm_file  = get_option_value(argc, argv, "pm");
    qry_file = get_option_value(argc, argv, "q");

    if (bed == NULL || bsd == NULL || geo_file == NULL) {
        fprintf(stderr, "uso: ted -e <BED> -f <arq.geo> -o <BSD>"
                        " [-pm <arq.pm>] [-q <arq.qry>]\n");
        return 1;
    }

    join_path(geo_path,      sizeof(geo_path),      bed, geo_file);
    join_path(quad_hf_path,  sizeof(quad_hf_path),  bsd, "quadras.hf");
    join_path(hab_hf_path,   sizeof(hab_hf_path),   bsd, "habitantes.hf");
    join_path(quad_hfd_path, sizeof(quad_hfd_path), bsd, "quadras.hfd");
    join_path(hab_hfd_path,  sizeof(hab_hfd_path),  bsd, "habitantes.hfd");

    strip_ext(base, sizeof(base), geo_file);
    join_path(svg_path, sizeof(svg_path), bsd, base);
    strncat(svg_path, ".svg", sizeof(svg_path) - strlen(svg_path) - 1u);
    join_path(txt_path, sizeof(txt_path), bsd, base);
    strncat(txt_path, ".txt", sizeof(txt_path) - strlen(txt_path) - 1u);

    quadras_hf = ehf_create(quad_hf_path, 10u, sizeof(quadra_registro_t));
    if (quadras_hf == NULL) {
        fprintf(stderr, "erro: nao foi possivel criar hashfile de quadras\n");
        return 1;
    }

    habitantes_hf = ehf_create(hab_hf_path, 10u, sizeof(habitante_registro_t));
    if (habitantes_hf == NULL) {
        fprintf(stderr, "erro: nao foi possivel criar hashfile de habitantes\n");
        ehf_close(quadras_hf);
        return 1;
    }

    svg = svg_writer_criar(svg_path, 2000.0, 2000.0);

    geo_res = geo_handler_processar(geo_path, quadras_hf, svg);
    if (geo_res.erros > 0) {
        fprintf(stderr, "aviso: geo_handler reportou %d erro(s)\n", geo_res.erros);
    }
    fprintf(stdout, "quadras inseridas: %d\n", geo_res.quadras_inseridas);

    if (pm_file != NULL) {
        pm_handler_resultado_t pm_res;
        join_path(pm_path, sizeof(pm_path), bed, pm_file);
        pm_res = pm_handler_processar(pm_path, habitantes_hf, quadras_hf);
        if (pm_res.erros > 0) {
            fprintf(stderr, "aviso: pm_handler reportou %d erro(s)\n", pm_res.erros);
        }
        fprintf(stdout, "pessoas inseridas: %d  moradores: %d\n",
                pm_res.pessoas_inseridas, pm_res.moradores_registrados);
    }

    if (qry_file != NULL) {
        qry_handler_resultado_t qry_res;
        join_path(qry_path, sizeof(qry_path), bed, qry_file);
        txt = txt_writer_criar(txt_path);
        qry_res = qry_handler_processar(qry_path, quadras_hf, habitantes_hf,
                                        svg, txt);
        if (qry_res.erros > 0) {
            fprintf(stderr, "aviso: qry_handler reportou %d erro(s)\n", qry_res.erros);
        }
        fprintf(stdout, "comandos .qry processados: %d\n",
                qry_res.comandos_processados);
        txt_writer_destruir(txt);
        txt = NULL;
    }

    if (svg != NULL) {
        svg_writer_finalizar(svg);
        svg_writer_destruir(svg);
    }

    ehf_dump(quadras_hf, quad_hfd_path);
    ehf_dump(habitantes_hf, hab_hfd_path);

    ehf_close(quadras_hf);
    ehf_close(habitantes_hf);

    return 0;
}
