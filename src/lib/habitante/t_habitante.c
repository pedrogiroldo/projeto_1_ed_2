#include "habitante.h"

#include "../unity/unity.h"

#include <string.h>

static pessoa_registro_t make_pessoa_reg(void) {
    pessoa_registro_t reg;
    memset(&reg, 0, sizeof(reg));
    strncpy(reg.cpf, "12345678900", PESSOA_CPF_MAX);
    strncpy(reg.nome, "Joao", PESSOA_NOME_MAX);
    strncpy(reg.sobrenome, "Silva", PESSOA_NOME_MAX);
    reg.sexo = 'M';
    strncpy(reg.nasc, "15/03/1990", PESSOA_NASC_MAX);
    return reg;
}

void setUp(void) {}
void tearDown(void) {}

void test_habitante_criar_sem_endereco(void) {
    pessoa_registro_t preg = make_pessoa_reg();
    habitante_t *h = habitante_criar(&preg);

    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_INT(0, habitante_eh_morador(h));
    TEST_ASSERT_EQUAL_STRING("12345678900", habitante_obter_cpf(h));
    TEST_ASSERT_EQUAL_STRING("Joao", habitante_obter_nome(h));
    TEST_ASSERT_EQUAL_INT('M', habitante_obter_sexo(h));
    TEST_ASSERT_NULL(habitante_obter_cep(h));
    TEST_ASSERT_EQUAL_INT('\0', habitante_obter_face(h));

    habitante_destruir(h);
}

void test_habitante_criar_rejeita_null(void) {
    TEST_ASSERT_NULL(habitante_criar(NULL));
}

void test_habitante_definir_e_remover_endereco(void) {
    pessoa_registro_t preg = make_pessoa_reg();
    habitante_t *h = habitante_criar(&preg);

    TEST_ASSERT_NOT_NULL(h);
    habitante_definir_endereco(h, "cep5", 'N', 42, "apto1");

    TEST_ASSERT_EQUAL_INT(1, habitante_eh_morador(h));
    TEST_ASSERT_EQUAL_STRING("cep5", habitante_obter_cep(h));
    TEST_ASSERT_EQUAL_INT('N', habitante_obter_face(h));
    TEST_ASSERT_EQUAL_INT(42, habitante_obter_num(h));
    TEST_ASSERT_EQUAL_STRING("apto1", habitante_obter_compl(h));

    habitante_remover_endereco(h);
    TEST_ASSERT_EQUAL_INT(0, habitante_eh_morador(h));
    TEST_ASSERT_NULL(habitante_obter_cep(h));

    habitante_destruir(h);
}

void test_habitante_definir_endereco_rejeita_face_invalida(void) {
    pessoa_registro_t preg = make_pessoa_reg();
    habitante_t *h = habitante_criar(&preg);

    TEST_ASSERT_NOT_NULL(h);
    habitante_definir_endereco(h, "cep1", 'X', 10, "");
    TEST_ASSERT_EQUAL_INT(0, habitante_eh_morador(h));

    habitante_destruir(h);
}

void test_habitante_round_trip_sem_teto(void) {
    pessoa_registro_t preg = make_pessoa_reg();
    habitante_t *original = habitante_criar(&preg);
    habitante_registro_t reg;
    habitante_t *recriado;

    TEST_ASSERT_NOT_NULL(original);
    reg = habitante_para_registro(original);
    recriado = habitante_criar_de_registro(&reg);

    TEST_ASSERT_NOT_NULL(recriado);
    TEST_ASSERT_EQUAL_INT(0, habitante_eh_morador(recriado));
    TEST_ASSERT_EQUAL_STRING(habitante_obter_cpf(original),
                             habitante_obter_cpf(recriado));

    habitante_destruir(original);
    habitante_destruir(recriado);
}

void test_habitante_round_trip_morador(void) {
    pessoa_registro_t preg = make_pessoa_reg();
    habitante_t *original = habitante_criar(&preg);
    habitante_registro_t reg;
    habitante_t *recriado;

    TEST_ASSERT_NOT_NULL(original);
    habitante_definir_endereco(original, "cep10", 'L', 87, "casa");

    reg = habitante_para_registro(original);
    recriado = habitante_criar_de_registro(&reg);

    TEST_ASSERT_NOT_NULL(recriado);
    TEST_ASSERT_EQUAL_INT(1, habitante_eh_morador(recriado));
    TEST_ASSERT_EQUAL_STRING("cep10", habitante_obter_cep(recriado));
    TEST_ASSERT_EQUAL_INT('L', habitante_obter_face(recriado));
    TEST_ASSERT_EQUAL_INT(87, habitante_obter_num(recriado));
    TEST_ASSERT_EQUAL_STRING("casa", habitante_obter_compl(recriado));

    habitante_destruir(original);
    habitante_destruir(recriado);
}

void test_habitante_criar_de_registro_rejeita_null(void) {
    TEST_ASSERT_NULL(habitante_criar_de_registro(NULL));
}

void test_habitante_getters_aceitam_null(void) {
    TEST_ASSERT_EQUAL_INT(0, habitante_eh_morador(NULL));
    TEST_ASSERT_NULL(habitante_obter_cpf(NULL));
    TEST_ASSERT_NULL(habitante_obter_nome(NULL));
    TEST_ASSERT_NULL(habitante_obter_cep(NULL));
    TEST_ASSERT_EQUAL_INT('\0', habitante_obter_face(NULL));
    TEST_ASSERT_EQUAL_INT(0, habitante_obter_num(NULL));
}

void test_habitante_faces_validas(void) {
    pessoa_registro_t preg = make_pessoa_reg();
    char faces[] = {'N', 'S', 'L', 'O'};
    size_t i;

    for (i = 0u; i < 4u; ++i) {
        habitante_t *h = habitante_criar(&preg);
        TEST_ASSERT_NOT_NULL(h);
        habitante_definir_endereco(h, "cep1", faces[i], 1, "");
        TEST_ASSERT_EQUAL_INT(1, habitante_eh_morador(h));
        TEST_ASSERT_EQUAL_INT(faces[i], habitante_obter_face(h));
        habitante_destruir(h);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_habitante_criar_sem_endereco);
    RUN_TEST(test_habitante_criar_rejeita_null);
    RUN_TEST(test_habitante_definir_e_remover_endereco);
    RUN_TEST(test_habitante_definir_endereco_rejeita_face_invalida);
    RUN_TEST(test_habitante_round_trip_sem_teto);
    RUN_TEST(test_habitante_round_trip_morador);
    RUN_TEST(test_habitante_criar_de_registro_rejeita_null);
    RUN_TEST(test_habitante_getters_aceitam_null);
    RUN_TEST(test_habitante_faces_validas);
    return UNITY_END();
}
