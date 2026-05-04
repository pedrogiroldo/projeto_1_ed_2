#include "svg_writer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SVG_VIEWBOX_MARGEM 20.0
#define SVG_BUFFER_INICIAL 1024u
#define SVG_STROKE_LINHA 2.0
#define SVG_FONT_SIZE_PADRAO 12.0

struct svg_writer {
    FILE *file;
    char *conteudo;
    size_t tamanho;
    size_t capacidade;
    double largura_fallback;
    double altura_fallback;
    double min_x;
    double min_y;
    double max_x;
    double max_y;
    int possui_bounds;
    int erro;
};

static int svg_writer_reservar(svg_writer_t *sw, size_t adicional) {
    char *novo_conteudo;
    size_t nova_capacidade;
    size_t necessario;

    if (sw == NULL) {
        return 0;
    }

    necessario = sw->tamanho + adicional + 1u;
    if (necessario <= sw->capacidade) {
        return 1;
    }

    nova_capacidade = sw->capacidade == 0u ? SVG_BUFFER_INICIAL : sw->capacidade;
    while (nova_capacidade < necessario) {
        if (nova_capacidade > ((size_t)-1) / 2u) {
            return 0;
        }
        nova_capacidade *= 2u;
    }

    novo_conteudo = (char *)realloc(sw->conteudo, nova_capacidade);
    if (novo_conteudo == NULL) {
        return 0;
    }

    sw->conteudo = novo_conteudo;
    sw->capacidade = nova_capacidade;
    return 1;
}

static int svg_writer_anexar(svg_writer_t *sw, const char *fmt, ...) {
    va_list args;
    va_list args_copia;
    int escrito;

    if (sw == NULL || fmt == NULL || sw->erro) {
        return 0;
    }

    va_start(args, fmt);
    va_copy(args_copia, args);
    escrito = vsnprintf(NULL, 0u, fmt, args);
    va_end(args);

    if (escrito < 0 ||
        !svg_writer_reservar(sw, (size_t)escrito)) {
        va_end(args_copia);
        sw->erro = 1;
        return 0;
    }

    vsnprintf(sw->conteudo + sw->tamanho,
              sw->capacidade - sw->tamanho,
              fmt, args_copia);
    va_end(args_copia);

    sw->tamanho += (size_t)escrito;
    return 1;
}

static void svg_writer_expandir_bounds(svg_writer_t *sw,
                                       double min_x, double min_y,
                                       double max_x, double max_y) {
    if (sw == NULL) {
        return;
    }

    if (!sw->possui_bounds) {
        sw->min_x = min_x;
        sw->min_y = min_y;
        sw->max_x = max_x;
        sw->max_y = max_y;
        sw->possui_bounds = 1;
        return;
    }

    if (min_x < sw->min_x) sw->min_x = min_x;
    if (min_y < sw->min_y) sw->min_y = min_y;
    if (max_x > sw->max_x) sw->max_x = max_x;
    if (max_y > sw->max_y) sw->max_y = max_y;
}

static double svg_writer_font_size(const char *font_size) {
    double valor;

    if (font_size == NULL || sscanf(font_size, "%lf", &valor) != 1 ||
        valor <= 0.0) {
        return SVG_FONT_SIZE_PADRAO;
    }

    return valor;
}

static void svg_writer_expandir_linha(svg_writer_t *sw,
                                      double x1, double y1,
                                      double x2, double y2) {
    double min_x = x1 < x2 ? x1 : x2;
    double min_y = y1 < y2 ? y1 : y2;
    double max_x = x1 > x2 ? x1 : x2;
    double max_y = y1 > y2 ? y1 : y2;
    double margem_stroke = SVG_STROKE_LINHA / 2.0;

    svg_writer_expandir_bounds(sw,
                               min_x - margem_stroke,
                               min_y - margem_stroke,
                               max_x + margem_stroke,
                               max_y + margem_stroke);
}

svg_writer_t *svg_writer_criar(const char *output_path,
                               double largura, double altura) {
    svg_writer_t *sw;

    if (output_path == NULL) {
        return NULL;
    }

    sw = (svg_writer_t *)malloc(sizeof(*sw));
    if (sw == NULL) {
        return NULL;
    }

    memset(sw, 0, sizeof(*sw));
    sw->file = fopen(output_path, "w");
    if (sw->file == NULL) {
        free(sw);
        return NULL;
    }

    sw->largura_fallback = largura > 0.0 ? largura : 1.0;
    sw->altura_fallback = altura > 0.0 ? altura : 1.0;

    return sw;
}

void svg_writer_finalizar(svg_writer_t *sw) {
    double view_x;
    double view_y;
    double view_w;
    double view_h;

    if (sw == NULL || sw->file == NULL) {
        return;
    }

    if (sw->possui_bounds) {
        view_x = sw->min_x - SVG_VIEWBOX_MARGEM;
        view_y = sw->min_y - SVG_VIEWBOX_MARGEM;
        view_w = (sw->max_x - sw->min_x) + (SVG_VIEWBOX_MARGEM * 2.0);
        view_h = (sw->max_y - sw->min_y) + (SVG_VIEWBOX_MARGEM * 2.0);
    } else {
        view_x = 0.0;
        view_y = 0.0;
        view_w = sw->largura_fallback;
        view_h = sw->altura_fallback;
    }

    fprintf(sw->file,
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<svg xmlns=\"http://www.w3.org/2000/svg\""
            " width=\"%.2f\" height=\"%.2f\""
            " viewBox=\"%.2f %.2f %.2f %.2f\">\n",
            view_w, view_h, view_x, view_y, view_w, view_h);
    if (sw->conteudo != NULL && sw->tamanho > 0u) {
        fwrite(sw->conteudo, 1u, sw->tamanho, sw->file);
    }
    fprintf(sw->file, "</svg>\n");
    fclose(sw->file);
    sw->file = NULL;
}

void svg_writer_destruir(svg_writer_t *sw) {
    if (sw == NULL) {
        return;
    }
    if (sw->file != NULL) {
        fclose(sw->file);
    }
    free(sw->conteudo);
    free(sw);
}

void svg_writer_retangulo(svg_writer_t *sw,
                          double x, double y, double w, double h,
                          const char *fill, const char *stroke,
                          double stroke_width) {
    if (sw == NULL || sw->file == NULL) {
        return;
    }
    svg_writer_expandir_bounds(sw,
                               x - stroke_width / 2.0,
                               y - stroke_width / 2.0,
                               x + w + stroke_width / 2.0,
                               y + h + stroke_width / 2.0);
    svg_writer_anexar(sw,
                      "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\""
                      " fill=\"%s\" stroke=\"%s\" stroke-width=\"%.2f\"/>\n",
                      x, y, w, h,
                      fill != NULL ? fill : "none",
                      stroke != NULL ? stroke : "none",
                      stroke_width);
}

void svg_writer_texto(svg_writer_t *sw, double x, double y,
                      const char *texto, const char *font_size,
                      const char *fill) {
    double fs;
    double largura_texto;

    if (sw == NULL || sw->file == NULL || texto == NULL) {
        return;
    }

    fs = svg_writer_font_size(font_size);
    largura_texto = (double)strlen(texto) * fs * 0.6;
    svg_writer_expandir_bounds(sw, x, y - fs, x + largura_texto, y);
    svg_writer_anexar(sw,
                      "  <text x=\"%.2f\" y=\"%.2f\" font-size=\"%s\""
                      " fill=\"%s\">%s</text>\n",
                      x, y,
                      font_size != NULL ? font_size : "12",
                      fill != NULL ? fill : "black",
                      texto);
}

void svg_writer_x_vermelho(svg_writer_t *sw,
                           double x, double y, double tamanho) {
    double d;
    if (sw == NULL || sw->file == NULL) {
        return;
    }
    d = tamanho / 2.0;
    svg_writer_expandir_linha(sw, x - d, y - d, x + d, y + d);
    svg_writer_expandir_linha(sw, x + d, y - d, x - d, y + d);
    svg_writer_anexar(sw,
                      "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\""
                      " stroke=\"red\" stroke-width=\"2\"/>\n"
                      "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\""
                      " stroke=\"red\" stroke-width=\"2\"/>\n",
                      x - d, y - d, x + d, y + d,
                      x + d, y - d, x - d, y + d);
}

void svg_writer_cruz_vermelha(svg_writer_t *sw,
                              double x, double y, double tamanho) {
    double d;
    if (sw == NULL || sw->file == NULL) {
        return;
    }
    d = tamanho / 2.0;
    svg_writer_expandir_linha(sw, x, y - d, x, y + d);
    svg_writer_expandir_linha(sw, x - d, y, x + d, y);
    svg_writer_anexar(sw,
                      "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\""
                      " stroke=\"red\" stroke-width=\"2\"/>\n"
                      "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\""
                      " stroke=\"red\" stroke-width=\"2\"/>\n",
                      x, y - d, x, y + d,
                      x - d, y, x + d, y);
}

void svg_writer_circulo_preto(svg_writer_t *sw,
                              double x, double y, double raio) {
    if (sw == NULL || sw->file == NULL) {
        return;
    }
    svg_writer_expandir_bounds(sw, x - raio, y - raio, x + raio, y + raio);
    svg_writer_anexar(sw,
                      "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\""
                      " fill=\"black\"/>\n",
                      x, y, raio);
}

void svg_writer_quadrado_cpf(svg_writer_t *sw,
                             double x, double y, double lado,
                             const char *cpf) {
    if (sw == NULL || sw->file == NULL) {
        return;
    }
    svg_writer_retangulo(sw, x - lado / 2.0, y - lado / 2.0, lado, lado,
                         "none", "red", 1.5);
    svg_writer_texto(sw, x - lado / 2.0 + 1.0, y + 4.0,
                     cpf != NULL ? cpf : "", "6", "red");
}
