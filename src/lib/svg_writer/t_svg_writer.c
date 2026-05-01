#include "svg_writer.h"

#include "../unity/unity.h"

#include <stdio.h>
#include <string.h>

static const char *SVG_PATH = "/tmp/svg_writer_test.svg";

static char *read_svg(void) {
    static char buf[4096];
    FILE *f = fopen(SVG_PATH, "r");
    if (f == NULL) {
        buf[0] = '\0';
        return buf;
    }
    size_t n = fread(buf, 1u, sizeof(buf) - 1u, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

void setUp(void) { remove(SVG_PATH); }
void tearDown(void) { remove(SVG_PATH); }

void test_svg_writer_escreve_elementos_basicos(void) {
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 100.0, 100.0);
    char *contents;

    TEST_ASSERT_NOT_NULL(svg);
    svg_writer_retangulo(svg, 1.0, 2.0, 3.0, 4.0, "red", "black", 1.0);
    svg_writer_texto(svg, 5.0, 6.0, "ola", "8", "blue");
    svg_writer_circulo_preto(svg, 7.0, 8.0, 2.0);
    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);

    contents = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(contents, "<svg"));
    TEST_ASSERT_NOT_NULL(strstr(contents, "<rect x=\"1.00\" y=\"2.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, ">ola</text>"));
    TEST_ASSERT_NOT_NULL(strstr(contents, "<circle cx=\"7.00\" cy=\"8.00\""));
    TEST_ASSERT_NOT_NULL(strstr(contents, "</svg>"));
}

void test_svg_writer_rejeita_path_null(void) {
    TEST_ASSERT_NULL(svg_writer_criar(NULL, 100.0, 100.0));
    svg_writer_destruir(NULL);
    svg_writer_finalizar(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_svg_writer_escreve_elementos_basicos);
    RUN_TEST(test_svg_writer_rejeita_path_null);
    return UNITY_END();
}
