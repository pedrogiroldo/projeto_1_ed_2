#include "qry_handler.h"

#include "extensible_hash_file.h"
#include "habitante.h"
#include "quadra.h"
#include "unity.h"

#include <stdio.h>
#include <string.h>

static const char *HF_HAB  = "/tmp/qry_handler_hab.hf";
static const char *HF_QUAD = "/tmp/qry_handler_quad.hf";
static const char *QRY_PATH = "/tmp/qry_handler_test.qry";
static const char *TXT_PATH = "/tmp/qry_handler_test.txt";
static const char *SVG_PATH = "/tmp/qry_handler_test.svg";

static void write_qry(const char *contents) {
    FILE *f = fopen(QRY_PATH, "w");
    if (f) { fputs(contents, f); fclose(f); }
}

static char *read_txt(void) {
    static char buf[8192];
    FILE *f = fopen(TXT_PATH, "r");
    if (!f) { buf[0] = '\0'; return buf; }
    size_t n = fread(buf, 1, sizeof(buf) - 1u, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

static char *read_svg(void) {
    static char buf[8192];
    FILE *f = fopen(SVG_PATH, "r");
    if (!f) { buf[0] = '\0'; return buf; }
    size_t n = fread(buf, 1, sizeof(buf) - 1u, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

static extensible_hash_file_t make_hab_hf(void) {
    return ehf_create(HF_HAB, 8u, sizeof(habitante_registro_t));
}

static extensible_hash_file_t make_quad_hf(void) {
    extensible_hash_file_t hf = ehf_create(HF_QUAD, 8u, sizeof(quadra_registro_t));
    quadra_t *q;
    quadra_registro_t reg;

    if (!hf) return NULL;
    q = quadra_criar("cep1", 0.0, 0.0, 100.0, 100.0, "red", "black", 1.0);
    if (q && quadra_para_registro(q, &reg) == 1)
        ehf_insert(hf, "cep1", &reg, sizeof(reg));
    quadra_destruir(q);
    return hf;
}

static void insert_habitante(extensible_hash_file_t hab,
                             const char *cpf, const char *nome,
                             char sexo, int is_morador,
                             const char *cep, char face, int num) {
    pessoa_registro_t preg;
    habitante_t *h;
    habitante_registro_t hreg;

    memset(&preg, 0, sizeof(preg));
    strncpy(preg.cpf, cpf, PESSOA_CPF_MAX);
    strncpy(preg.nome, nome, PESSOA_NOME_MAX);
    strncpy(preg.sobrenome, "Sobrenome", PESSOA_NOME_MAX);
    preg.sexo = sexo;
    strncpy(preg.nasc, "01/01/2000", PESSOA_NASC_MAX);

    h = habitante_criar(&preg);
    if (!h) return;
    if (is_morador) habitante_definir_endereco(h, cep, face, num, "");
    hreg = habitante_para_registro(h);
    habitante_destruir(h);
    ehf_insert(hab, cpf, &hreg, sizeof(hreg));
}

void setUp(void) {
    remove(HF_HAB);
    remove(HF_QUAD);
    remove(QRY_PATH);
    remove(TXT_PATH);
    remove(SVG_PATH);
}

void tearDown(void) {
    remove(HF_HAB);
    remove(HF_QUAD);
    remove(QRY_PATH);
    remove(TXT_PATH);
    remove(SVG_PATH);
}

void test_qry_nasc_insere_habitante(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    habitante_registro_t hreg;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    TEST_ASSERT_NOT_NULL(tw);
    write_qry("nasc 12345678900 Joao Silva M 15/03/1990\n");

    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);
    TEST_ASSERT_EQUAL_INT(0, res.erros);
    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));
    TEST_ASSERT_EQUAL_INT(0, hreg.is_morador);

    txt_writer_destruir(tw);
    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_rip_remove_habitante(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    habitante_registro_t hreg;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "12345678900", "Joao", 'M', 0, NULL, '\0', 0);

    write_qry("rip 12345678900\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);
    TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));

    txt_writer_destruir(tw);
    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_censo_conta_habitantes(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    char *txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "11111111111", "Ana", 'F', 1, "cep1", 'N', 10);
    insert_habitante(hab, "22222222222", "Bob", 'M', 0, NULL, '\0', 0);

    write_qry("censo\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);
    txt_writer_destruir(tw);

    txt = read_txt();
    TEST_ASSERT_NOT_NULL(strstr(txt, "total=2"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "moradores=1"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "sem_teto=1"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "prop_moradores_habitantes=50.00%"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "habitantes_homens=50.00%"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "moradores_mulheres=100.00%"));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_h_query_sem_teto(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    char *txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "12345678900", "Joao", 'M', 0, NULL, '\0', 0);

    write_qry("h? 12345678900\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);
    txt_writer_destruir(tw);

    txt = read_txt();
    TEST_ASSERT_NOT_NULL(strstr(txt, "sem-teto"));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_mud_atualiza_endereco(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    habitante_registro_t hreg;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "12345678900", "Joao", 'M', 0, NULL, '\0', 0);

    write_qry("mud 12345678900 cep1 S 99 ap2\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);
    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));
    TEST_ASSERT_EQUAL_INT(1, hreg.is_morador);
    TEST_ASSERT_EQUAL_STRING("cep1", hreg.cep);
    TEST_ASSERT_EQUAL_INT('S', hreg.face);
    TEST_ASSERT_EQUAL_INT(99, hreg.num);

    txt_writer_destruir(tw);
    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_dspj_vira_semteto(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    habitante_registro_t hreg;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "12345678900", "Joao", 'M', 1, "cep1", 'L', 5);

    write_qry("dspj 12345678900\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);
    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));
    TEST_ASSERT_EQUAL_INT(0, hreg.is_morador);

    txt_writer_destruir(tw);
    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_dspj_marca_svg_no_endereco(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 200.0, 200.0);
    qry_handler_resultado_t res;
    char *svg_txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    TEST_ASSERT_NOT_NULL(svg);
    insert_habitante(hab, "12345678900", "Joao", 'M', 1, "cep1", 'L', 5);

    write_qry("dspj 12345678900\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, svg, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);

    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);
    txt_writer_destruir(tw);

    svg_txt = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "<circle cx=\"0.00\" cy=\"5.00\""));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_rq_remove_quadra_e_despeja(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    habitante_registro_t hreg;
    quadra_registro_t qreg;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "12345678900", "Joao", 'M', 1, "cep1", 'N', 10);

    write_qry("rq cep1\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);

    TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND,
                          ehf_find(quad, "cep1", &qreg, sizeof(qreg)));
    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));
    TEST_ASSERT_EQUAL_INT(0, hreg.is_morador);

    txt_writer_destruir(tw);
    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_rq_marca_vertices_mesmo_sem_moradores(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 200.0, 200.0);
    qry_handler_resultado_t res;
    char *svg_txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    TEST_ASSERT_NOT_NULL(svg);

    write_qry("rq cep1\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, svg, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);

    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);
    txt_writer_destruir(tw);

    svg_txt = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(svg_txt,
        "x1=\"0.00\" y1=\"0.00\" x2=\"100.00\" y2=\"100.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt,
        "x1=\"100.00\" y1=\"0.00\" x2=\"0.00\" y2=\"100.00\""));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_pq_conta_faces(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    char *txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "11111111111", "A", 'M', 1, "cep1", 'N', 1);
    insert_habitante(hab, "22222222222", "B", 'F', 1, "cep1", 'N', 2);
    insert_habitante(hab, "33333333333", "C", 'M', 1, "cep1", 'S', 3);

    write_qry("pq cep1\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);
    txt_writer_destruir(tw);

    txt = read_txt();
    TEST_ASSERT_NOT_NULL(strstr(txt, "N=2"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "S=1"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "total=3"));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_pq_escreve_contagens_no_svg(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 200.0, 200.0);
    qry_handler_resultado_t res;
    char *svg_txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    TEST_ASSERT_NOT_NULL(svg);
    insert_habitante(hab, "11111111111", "A", 'M', 1, "cep1", 'N', 1);
    insert_habitante(hab, "22222222222", "B", 'F', 1, "cep1", 'S', 2);
    insert_habitante(hab, "33333333333", "C", 'M', 1, "cep1", 'L', 3);
    insert_habitante(hab, "44444444444", "D", 'F', 1, "cep1", 'S', 4);
    insert_habitante(hab, "55555555555", "E", 'M', 1, "cep1", 'O', 5);

    write_qry("pq cep1\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, svg, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);

    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);
    txt_writer_destruir(tw);

    svg_txt = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "<text x=\"50.00\" y=\"112.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, ">1</text>"));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "<text x=\"50.00\" y=\"-4.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, ">2</text>"));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "<text x=\"-12.00\" y=\"50.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "<text x=\"104.00\" y=\"50.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "<text x=\"50.00\" y=\"50.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, ">5</text>"));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_mud_marca_svg_no_destino(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 200.0, 200.0);
    qry_handler_resultado_t res;
    char *svg_txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    TEST_ASSERT_NOT_NULL(svg);
    insert_habitante(hab, "12345678900", "Joao", 'M', 0, NULL, '\0', 0);

    write_qry("mud 12345678900 cep1 S 99 ap2\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, svg, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);

    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);
    txt_writer_destruir(tw);

    svg_txt = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "<rect x=\"89.00\" y=\"-10.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "12345678900"));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_rip_marca_svg_no_endereco(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 200.0, 200.0);
    qry_handler_resultado_t res;
    char *svg_txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    TEST_ASSERT_NOT_NULL(svg);
    insert_habitante(hab, "12345678900", "Joao", 'M', 1, "cep1", 'N', 10);

    write_qry("rip 12345678900\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, svg, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);

    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);
    txt_writer_destruir(tw);

    svg_txt = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "x1=\"10.00\" y1=\"95.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "x2=\"10.00\" y2=\"105.00\""));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_rip_marca_svg_face_o(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    svg_writer_t *svg = svg_writer_criar(SVG_PATH, 200.0, 200.0);
    qry_handler_resultado_t res;
    char *svg_txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    TEST_ASSERT_NOT_NULL(svg);
    insert_habitante(hab, "12345678900", "Joao", 'M', 1, "cep1", 'O', 10);

    write_qry("rip 12345678900\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, svg, tw);
    TEST_ASSERT_EQUAL_INT(1, res.comandos_processados);

    svg_writer_finalizar(svg);
    svg_writer_destruir(svg);
    txt_writer_destruir(tw);

    svg_txt = read_svg();
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "x1=\"100.00\" y1=\"5.00\""));
    TEST_ASSERT_NOT_NULL(strstr(svg_txt, "x2=\"100.00\" y2=\"15.00\""));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_rip_duplicado_eh_idempotente_sem_erro(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    habitante_registro_t hreg;
    char *txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "12345678900", "Joao", 'M', 0, NULL, '\0', 0);

    write_qry("rip 12345678900\nrip 12345678900\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(2, res.comandos_processados);
    TEST_ASSERT_EQUAL_INT(0, res.erros);
    TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));

    txt_writer_destruir(tw);
    txt = read_txt();
    TEST_ASSERT_NOT_NULL(strstr(txt, "rip cpf=12345678900"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "rip 12345678900: ja removido"));

    ehf_close(hab);
    ehf_close(quad);
}

void test_qry_dspj_duplicado_eh_idempotente_sem_erro(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    txt_writer_t *tw = txt_writer_criar(TXT_PATH);
    qry_handler_resultado_t res;
    habitante_registro_t hreg;
    char *txt;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    insert_habitante(hab, "12345678900", "Joao", 'M', 1, "cep1", 'L', 5);

    write_qry("dspj 12345678900\ndspj 12345678900\n");
    res = qry_handler_processar(QRY_PATH, quad, hab, NULL, tw);
    TEST_ASSERT_EQUAL_INT(2, res.comandos_processados);
    TEST_ASSERT_EQUAL_INT(0, res.erros);
    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));
    TEST_ASSERT_EQUAL_INT(0, hreg.is_morador);

    txt_writer_destruir(tw);
    txt = read_txt();
    TEST_ASSERT_NOT_NULL(strstr(txt, "dspj cpf=12345678900"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "dspj 12345678900: ja sem endereco"));

    ehf_close(hab);
    ehf_close(quad);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_qry_nasc_insere_habitante);
    RUN_TEST(test_qry_rip_remove_habitante);
    RUN_TEST(test_qry_censo_conta_habitantes);
    RUN_TEST(test_qry_h_query_sem_teto);
    RUN_TEST(test_qry_mud_atualiza_endereco);
    RUN_TEST(test_qry_dspj_vira_semteto);
    RUN_TEST(test_qry_dspj_marca_svg_no_endereco);
    RUN_TEST(test_qry_rq_remove_quadra_e_despeja);
    RUN_TEST(test_qry_rq_marca_vertices_mesmo_sem_moradores);
    RUN_TEST(test_qry_pq_conta_faces);
    RUN_TEST(test_qry_pq_escreve_contagens_no_svg);
    RUN_TEST(test_qry_mud_marca_svg_no_destino);
    RUN_TEST(test_qry_rip_marca_svg_no_endereco);
    RUN_TEST(test_qry_rip_marca_svg_face_o);
    RUN_TEST(test_qry_rip_duplicado_eh_idempotente_sem_erro);
    RUN_TEST(test_qry_dspj_duplicado_eh_idempotente_sem_erro);
    return UNITY_END();
}
