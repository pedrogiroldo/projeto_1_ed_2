#ifndef EXTENSIBLE_HASH_FILE_H
#define EXTENSIBLE_HASH_FILE_H

#include <stdbool.h>
#include <stdint.h>

typedef void *extensible_hash_file_t;

typedef enum {
    EHF_OK = 0,
    EHF_NOT_FOUND,
    EHF_DUPLICATE_KEY,
    EHF_INVALID_ARGUMENT,
    EHF_IO_ERROR,
    EHF_CORRUPTED_FILE
} ehf_status_t;

extensible_hash_file_t ehf_create(const char *index_path, uint32_t bucket_capacity);
extensible_hash_file_t ehf_open(const char *index_path);
void ehf_close(extensible_hash_file_t hash);

ehf_status_t ehf_insert(extensible_hash_file_t hash, uint32_t key, uint64_t data_offset);
ehf_status_t ehf_find(extensible_hash_file_t hash, uint32_t key, uint64_t *out_data_offset);
ehf_status_t ehf_remove(extensible_hash_file_t hash, uint32_t key);

bool ehf_is_open(extensible_hash_file_t hash);

#endif
