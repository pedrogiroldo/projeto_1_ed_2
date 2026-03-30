#include "extensible_hash_file.h"

#include "../unity/unity.h"

#include <stdio.h>

static const char *TEST_INDEX_PATH = "/tmp/extensible_hash_file_test.idx";

void setUp(void) { remove(TEST_INDEX_PATH); }

void tearDown(void) { remove(TEST_INDEX_PATH); }

void test_ehf_create_returns_handle_for_valid_path(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_TRUE(ehf_is_open(hash));

  ehf_close(hash);
}

void test_ehf_create_rejects_invalid_arguments(void) {
  TEST_ASSERT_NULL(ehf_create(NULL, 2u));
  TEST_ASSERT_NULL(ehf_create(TEST_INDEX_PATH, 0u));
}

void test_ehf_open_rejects_invalid_path(void) {
  TEST_ASSERT_NULL(ehf_open(NULL));
}

void test_ehf_open_returns_null_for_missing_file(void) {
  TEST_ASSERT_NULL(ehf_open(TEST_INDEX_PATH));
}

void test_ehf_insert_and_find_existing_key(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);
  uint64_t data_offset = 0u;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, 7u, 123u));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, 7u, &data_offset));
  TEST_ASSERT_EQUAL_UINT64(123u, data_offset);

  ehf_close(hash);
}

void test_ehf_find_returns_not_found_for_missing_key(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);
  uint64_t data_offset = 0u;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND, ehf_find(hash, 99u, &data_offset));

  ehf_close(hash);
}

void test_ehf_insert_rejects_duplicate_key(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, 10u, 1000u));
  TEST_ASSERT_EQUAL_INT(EHF_DUPLICATE_KEY, ehf_insert(hash, 10u, 2000u));

  ehf_close(hash);
}

void test_ehf_remove_existing_key(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);
  uint64_t data_offset = 0u;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, 15u, 1500u));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_remove(hash, 15u));
  TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND, ehf_find(hash, 15u, &data_offset));

  ehf_close(hash);
}

void test_ehf_remove_missing_key_returns_not_found(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND, ehf_remove(hash, 44u));

  ehf_close(hash);
}

void test_ehf_persists_data_across_close_and_open(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);
  uint64_t data_offset = 0u;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, 1u, 11u));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, 2u, 22u));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, 3u, 33u));
  ehf_close(hash);

  hash = ehf_open(TEST_INDEX_PATH);
  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, 1u, &data_offset));
  TEST_ASSERT_EQUAL_UINT64(11u, data_offset);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, 2u, &data_offset));
  TEST_ASSERT_EQUAL_UINT64(22u, data_offset);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, 3u, &data_offset));
  TEST_ASSERT_EQUAL_UINT64(33u, data_offset);

  ehf_close(hash);
}

void test_ehf_handles_null_arguments_defensively(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);
  uint64_t data_offset = 0u;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT, ehf_insert(NULL, 1u, 1u));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT, ehf_find(NULL, 1u, &data_offset));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT, ehf_find(hash, 1u, NULL));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT, ehf_remove(NULL, 1u));

  ehf_close(hash);
}

void test_ehf_triggers_split_without_breaking_public_contract(void) {
  extensible_hash_file_t hash = ehf_create(TEST_INDEX_PATH, 2u);
  uint64_t data_offset = 0u;
  uint32_t key;

  TEST_ASSERT_NOT_NULL(hash);

  for (key = 0u; key < 10u; ++key) {
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, key, (uint64_t)key * 10u));
  }

  for (key = 0u; key < 10u; ++key) {
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, key, &data_offset));
    TEST_ASSERT_EQUAL_UINT64((uint64_t)key * 10u, data_offset);
  }

  ehf_close(hash);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_ehf_create_returns_handle_for_valid_path);
  RUN_TEST(test_ehf_create_rejects_invalid_arguments);
  RUN_TEST(test_ehf_open_rejects_invalid_path);
  RUN_TEST(test_ehf_open_returns_null_for_missing_file);
  RUN_TEST(test_ehf_insert_and_find_existing_key);
  RUN_TEST(test_ehf_find_returns_not_found_for_missing_key);
  RUN_TEST(test_ehf_insert_rejects_duplicate_key);
  RUN_TEST(test_ehf_remove_existing_key);
  RUN_TEST(test_ehf_remove_missing_key_returns_not_found);
  RUN_TEST(test_ehf_persists_data_across_close_and_open);
  RUN_TEST(test_ehf_handles_null_arguments_defensively);
  RUN_TEST(test_ehf_triggers_split_without_breaking_public_contract);
  return UNITY_END();
}
