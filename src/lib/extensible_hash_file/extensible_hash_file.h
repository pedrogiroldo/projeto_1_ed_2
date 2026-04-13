#ifndef EXTENSIBLE_HASH_FILE_H
#define EXTENSIBLE_HASH_FILE_H

/*
 * File-backed extensible hash index.
 *
 * Use ehf_create() to create a new hash file or ehf_open() to reuse an
 * existing one. Store fixed-size records by passing an alphanumeric key and a
 * record buffer to ehf_insert(), recover records with ehf_find(), remove
 * entries with ehf_remove(), and always finish with ehf_close().
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Opaque handle for a file-backed extensible hash index. */
typedef void *extensible_hash_file_t;

/* Result codes returned by the public index operations. */
typedef enum {
    EHF_OK = 0,
    EHF_NOT_FOUND,
    EHF_DUPLICATE_KEY,
    EHF_INVALID_ARGUMENT,
    EHF_IO_ERROR,
    EHF_CORRUPTED_FILE
} ehf_status_t;

/*
 * Creates a new hash file at index_path.
 *
 * bucket_capacity defines how many entries fit in each bucket before a split.
 * record_size defines the byte size of every record stored in this hash file.
 * Returns NULL when the arguments are invalid or allocation/write fails.
 */
extensible_hash_file_t ehf_create(const char *index_path, uint32_t bucket_capacity,
                                  size_t record_size);

/*
 * Opens an existing index file and validates its header.
 *
 * Returns NULL if index_path is invalid, if the file does not exist, or if the
 * contents do not match the expected format.
 */
extensible_hash_file_t ehf_open(const char *index_path);

/* Closes the index, releases its resources, and accepts NULL safely. */
void ehf_close(extensible_hash_file_t hash);

/*
 * Inserts a 1- to 32-character alphanumeric key associated with record bytes.
 *
 * Returns EHF_DUPLICATE_KEY when the key already exists, EHF_INVALID_ARGUMENT
 * for invalid handles/keys/record sizes, EHF_IO_ERROR for write failures, or
 * EHF_OK on success.
 */
ehf_status_t ehf_insert(extensible_hash_file_t hash, const char *key,
                        const void *record, size_t record_size);

/*
 * Finds a 1- to 32-character alphanumeric key.
 *
 * On success, copies the found record to out_record and returns EHF_OK.
 * Returns EHF_NOT_FOUND when the key does not exist and EHF_INVALID_ARGUMENT
 * when hash, key, out_record, or record_size is invalid.
 */
ehf_status_t ehf_find(extensible_hash_file_t hash, const char *key,
                      void *out_record, size_t record_size);

/*
 * Removes a 1- to 32-character alphanumeric key from the index.
 *
 * Returns EHF_NOT_FOUND when the key does not exist and EHF_INVALID_ARGUMENT
 * for invalid handles/keys.
 */
ehf_status_t ehf_remove(extensible_hash_file_t hash, const char *key);

/* Reports whether the handle points to an open, valid index. */
bool ehf_is_open(extensible_hash_file_t hash);

#endif
