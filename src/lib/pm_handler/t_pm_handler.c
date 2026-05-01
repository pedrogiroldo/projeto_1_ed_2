#include "pm_handler.h"

#include "../extensible_hash_file/extensible_hash_file.h"
#include "../habitante/habitante.h"
#include "../quadra/quadra.h"
#include "../unity/unity.h"

#include <stdio.h>
#include <string.h>

static const char *HF_HAB  = "/tmp/pm_handler_hab.hf";
static const char *HF_QUAD = "/tmp/pm_handler_quad.hf";
static const char *PM_PATH = "/tmp/pm_handler_test.pm";

static void write_pm(const char *contents) {
    FILE *f = fopen(PM_PATH, "w");
    if (f != NULL) {
        fputs(contents, f);
        fclose(f);
    }
}

static extensible_hash_file_t make_hab_hf(void) {
    return ehf_create(HF_HAB, 8u, sizeof(habitante_registro_t));
}

static extensible_hash_file_t make_quad_hf(void) {
    extensible_hash_file_t hf = ehf_create(HF_QUAD, 8u, sizeof(quadra_registro_t));
    quadra_t *q;
    quadra_registro_t reg;

    if (hf == NULL) return NULL;
    q = quadra_criar("cep5", 0.0, 0.0, 100.0, 100.0, "red", "black", 1.0);
    if (q != NULL && quadra_para_registro(q, &reg) == 1) {
        ehf_insert(hf, "cep5", &reg, sizeof(reg));
    }
    quadra_destruir(q);
    return hf;
}

void setUp(void) {
    remove(HF_HAB);
    remove(HF_QUAD);
    remove(PM_PATH);
}

void tearDown(void) {
    remove(HF_HAB);
    remove(HF_QUAD);
    remove(PM_PATH);
}

void test_pm_handler_insere_pessoa(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    pm_handler_resultado_t res;
    habitante_registro_t hreg;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    write_pm("p 12345678900 Joao Silva M 15/03/1990\n");

    res = pm_handler_processar(PM_PATH, hab, quad);
    TEST_ASSERT_EQUAL_INT(1, res.pessoas_inseridas);
    TEST_ASSERT_EQUAL_INT(0, res.moradores_registrados);
    TEST_ASSERT_EQUAL_INT(0, res.erros);

    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));
    TEST_ASSERT_EQUAL_STRING("Joao", hreg.pessoa.nome);
    TEST_ASSERT_EQUAL_INT(0, hreg.is_morador);

    ehf_close(hab);
    ehf_close(quad);
}

void test_pm_handler_registra_morador(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    pm_handler_resultado_t res;
    habitante_registro_t hreg;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    write_pm("p 12345678900 Joao Silva M 15/03/1990\n"
             "m 12345678900 cep5 N 42 apto1\n");

    res = pm_handler_processar(PM_PATH, hab, quad);
    TEST_ASSERT_EQUAL_INT(1, res.pessoas_inseridas);
    TEST_ASSERT_EQUAL_INT(1, res.moradores_registrados);
    TEST_ASSERT_EQUAL_INT(0, res.erros);

    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_find(hab, "12345678900", &hreg, sizeof(hreg)));
    TEST_ASSERT_EQUAL_INT(1, hreg.is_morador);
    TEST_ASSERT_EQUAL_STRING("cep5", hreg.cep);
    TEST_ASSERT_EQUAL_INT('N', hreg.face);
    TEST_ASSERT_EQUAL_INT(42, hreg.num);

    ehf_close(hab);
    ehf_close(quad);
}

void test_pm_handler_morador_cep_inexistente_conta_erro(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    pm_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    write_pm("p 12345678900 Joao Silva M 15/03/1990\n"
             "m 12345678900 cep999 N 1 \n");

    res = pm_handler_processar(PM_PATH, hab, quad);
    TEST_ASSERT_EQUAL_INT(1, res.pessoas_inseridas);
    TEST_ASSERT_EQUAL_INT(0, res.moradores_registrados);
    TEST_ASSERT_EQUAL_INT(1, res.erros);

    ehf_close(hab);
    ehf_close(quad);
}

void test_pm_handler_morador_cpf_inexistente_conta_erro(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    pm_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    write_pm("m 99999999999 cep5 S 10 \n");

    res = pm_handler_processar(PM_PATH, hab, quad);
    TEST_ASSERT_EQUAL_INT(0, res.pessoas_inseridas);
    TEST_ASSERT_EQUAL_INT(0, res.moradores_registrados);
    TEST_ASSERT_EQUAL_INT(1, res.erros);

    ehf_close(hab);
    ehf_close(quad);
}

void test_pm_handler_cpf_duplicado_conta_erro(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    pm_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    write_pm("p 12345678900 Joao Silva M 15/03/1990\n"
             "p 12345678900 Clone Silva M 01/01/2000\n");

    res = pm_handler_processar(PM_PATH, hab, quad);
    TEST_ASSERT_EQUAL_INT(1, res.pessoas_inseridas);
    TEST_ASSERT_EQUAL_INT(1, res.erros);

    ehf_close(hab);
    ehf_close(quad);
}

void test_pm_handler_null_path_conta_erro(void) {
    extensible_hash_file_t hab = make_hab_hf();
    extensible_hash_file_t quad = make_quad_hf();
    pm_handler_resultado_t res;

    TEST_ASSERT_NOT_NULL(hab);
    TEST_ASSERT_NOT_NULL(quad);
    res = pm_handler_processar(NULL, hab, quad);
    TEST_ASSERT_EQUAL_INT(1, res.erros);

    ehf_close(hab);
    ehf_close(quad);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pm_handler_insere_pessoa);
    RUN_TEST(test_pm_handler_registra_morador);
    RUN_TEST(test_pm_handler_morador_cep_inexistente_conta_erro);
    RUN_TEST(test_pm_handler_morador_cpf_inexistente_conta_erro);
    RUN_TEST(test_pm_handler_cpf_duplicado_conta_erro);
    RUN_TEST(test_pm_handler_null_path_conta_erro);
    return UNITY_END();
}
