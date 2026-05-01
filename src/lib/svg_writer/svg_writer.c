#include "svg_writer.h"

#include <stdio.h>
#include <stdlib.h>

struct svg_writer {
    FILE *file;
};

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

    sw->file = fopen(output_path, "w");
    if (sw->file == NULL) {
        free(sw);
        return NULL;
    }

    fprintf(sw->file,
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<svg xmlns=\"http://www.w3.org/2000/svg\""
            " width=\"%.2f\" height=\"%.2f\""
            " viewBox=\"0 0 %.2f %.2f\">\n",
            largura, altura, largura, altura);

    return sw;
}

void svg_writer_finalizar(svg_writer_t *sw) {
    if (sw == NULL || sw->file == NULL) {
        return;
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
    free(sw);
}

void svg_writer_retangulo(svg_writer_t *sw,
                          double x, double y, double w, double h,
                          const char *fill, const char *stroke,
                          double stroke_width) {
    if (sw == NULL || sw->file == NULL) {
        return;
    }
    fprintf(sw->file,
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
    if (sw == NULL || sw->file == NULL || texto == NULL) {
        return;
    }
    fprintf(sw->file,
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
    fprintf(sw->file,
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
    fprintf(sw->file,
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
    fprintf(sw->file,
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
