#include "pessoa.h"

#include "../unity/unity.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_pessoa_criar_guarda_propriedades(void) {
    pessoa_t *p = pessoa_criar("12345678900", "Joao", "Silva", 'M', "15/03/1990");

    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("12345678900", pessoa_obter_cpf(p));
    TEST_ASSERT_EQUAL_STRING("Joao", pessoa_obter_nome(p));
    TEST_ASSERT_EQUAL_STRING("Silva", pessoa_obter_sobrenome(p));
    TEST_ASSERT_EQUAL_INT('M', pessoa_obter_sexo(p));
    TEST_ASSERT_EQUAL_STRING("15/03/1990", pessoa_obter_nasc(p));

    pessoa_destruir(p);
}

void test_pessoa_criar_rejeita_argumentos_invalidos(void) {
    TEST_ASSERT_NULL(pessoa_criar(NULL, "Joao", "Silva", 'M', "15/03/1990"));
    TEST_ASSERT_NULL(pessoa_criar("", "Joao", "Silva", 'M', "15/03/1990"));
    TEST_ASSERT_NULL(pessoa_criar("12345678900", NULL, "Silva", 'M', "15/03/1990"));
    TEST_ASSERT_NULL(pessoa_criar("12345678900", "", "Silva", 'M', "15/03/1990"));
    TEST_ASSERT_NULL(pessoa_criar("12345678900", "Joao", NULL, 'M', "15/03/1990"));
    TEST_ASSERT_NULL(pessoa_criar("12345678900", "Joao", "", 'M', "15/03/1990"));
    TEST_ASSERT_NULL(pessoa_criar("12345678900", "Joao", "Silva", 'X', "15/03/1990"));
    TEST_ASSERT_NULL(pessoa_criar("12345678900", "Joao", "Silva", '\0', "15/03/1990"));
    TEST_ASSERT_NULL(pessoa_criar("12345678900", "Joao", "Silva", 'M', NULL));
    TEST_ASSERT_NULL(pessoa_criar("12345678900", "Joao", "Silva", 'M', ""));
}

void test_pessoa_getters_aceitam_null(void) {
    TEST_ASSERT_NULL(pessoa_obter_cpf(NULL));
    TEST_ASSERT_NULL(pessoa_obter_nome(NULL));
    TEST_ASSERT_NULL(pessoa_obter_sobrenome(NULL));
    TEST_ASSERT_EQUAL_INT('\0', pessoa_obter_sexo(NULL));
    TEST_ASSERT_NULL(pessoa_obter_nasc(NULL));
}

void test_pessoa_converte_para_registro_e_recria(void) {
    pessoa_t *original = pessoa_criar("98765432100", "Maria", "Santos", 'F', "22/07/1985");
    pessoa_registro_t reg;
    pessoa_t *recriada;

    TEST_ASSERT_NOT_NULL(original);

    reg = pessoa_para_registro(original);
    recriada = pessoa_criar_de_registro(&reg);

    TEST_ASSERT_NOT_NULL(recriada);
    TEST_ASSERT_EQUAL_STRING(pessoa_obter_cpf(original), pessoa_obter_cpf(recriada));
    TEST_ASSERT_EQUAL_STRING(pessoa_obter_nome(original), pessoa_obter_nome(recriada));
    TEST_ASSERT_EQUAL_STRING(pessoa_obter_sobrenome(original), pessoa_obter_sobrenome(recriada));
    TEST_ASSERT_EQUAL_INT(pessoa_obter_sexo(original), pessoa_obter_sexo(recriada));
    TEST_ASSERT_EQUAL_STRING(pessoa_obter_nasc(original), pessoa_obter_nasc(recriada));

    pessoa_destruir(original);
    pessoa_destruir(recriada);
}

void test_pessoa_criar_de_registro_rejeita_null(void) {
    TEST_ASSERT_NULL(pessoa_criar_de_registro(NULL));
}

void test_pessoa_aceita_sexo_feminino(void) {
    pessoa_t *p = pessoa_criar("11111111111", "Ana", "Costa", 'F', "01/01/2000");

    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT('F', pessoa_obter_sexo(p));

    pessoa_destruir(p);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pessoa_criar_guarda_propriedades);
    RUN_TEST(test_pessoa_criar_rejeita_argumentos_invalidos);
    RUN_TEST(test_pessoa_getters_aceitam_null);
    RUN_TEST(test_pessoa_converte_para_registro_e_recria);
    RUN_TEST(test_pessoa_criar_de_registro_rejeita_null);
    RUN_TEST(test_pessoa_aceita_sexo_feminino);
    return UNITY_END();
}
