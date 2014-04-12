#ifndef __VDB_H
#define __VDB_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VDB_VERSION 1

/**
 * VDB database state
 *
 * These fields can be read by a user, e.g. to look key_size and
 * value_seize, but should never be changed
 */

typedef struct {
    unsigned long hash_table_size;
    unsigned long key_size;
    unsigned long value_size;
    unsigned long hash_table_size_bytes;
    unsigned long num_hash_tables;
    uint64_t *hash_tables;
    FILE *f;
} VDB;

/**
 * I/O error of file not found
 */
#define VDB_ERROR_IO -1

/**
 * Out of memory
 */
#define VDB_ERROR_MALLOC -2

/*
 * Invalid parameters (e.g. missing _size parameters on init to create database
 */
#define VDB_ERROR_INVALID_PARAMETERS -3

/**
 * Database file appears corrupt
 */
#define VDB_ERROR_CORRUPT_DBFILE -4

/**
 * Open mode: read-only
 */
#define VDB_OPEN_MODE_RDONLY 1

/**
 * Open mode: read/write
 */
#define VDB_OPEN_MODE_RDWR 2

/**
 * Open mode: read/write, create if doesn't exist
 */
#define VDB_OPEN_MODE_RWCREAT 3

/**
 * Open mode: truncate database, open for reading and writing
 */
#define VDB_OPEN_MODE_RWREPLACE 4

/**
 * Open database
 *
 * @param db Database struct
 * @param path Path to the file
 * @param mode One of the VDB_OPEN_MODE constants
 * @param hash_table_size Size of the hash table in 64-bit entries
 * @param key_size Size of keys in bytes
 * @param value_size Size of values in bytes
 * @return 0 on success, nonzero on error
 */
extern int VDB_open(
        VDB *db,
        const char *path,
        int mode,
        unsigned long hash_table_size,
        unsigned long key_size,
        unsigned long value_size);

/**
 * Close database
 *
 * @param db Database struct
 */
extern void VDB_close(VDB *db);

/**
 * Get an entry
 *
 * @param db Database struct
 * @param key Key (key_size bytes)
 * @param vbuf Value buffer (value_size bytes capacity)
 * @return -1 on I/O error, 0 on success, 1 on not found
 */
extern int VDB_get(VDB *db, const void *key, void *buf);

/**
 * Put an entry (overwriting if already exists)
 *
 * in the already-exists case the size of teh database file doesn't change
 *
 * @param db Database struct
 * @param key Key (key_size bytes)
 * @param value Value(value_size bytes)
 * @return -1 on I/o error, 0 on success
 */
extern int VDB_put(VDB *db, const void *key, const void *value);

/**
 * Cursor used for iterating over all entries in database
 */
typedef struct {
    VDB *db;
    unsigned long h_no;
    unsigned long h_idx;
} VDB_Iterator;

/**
 * Initialize an iterator
 *
 * @param db Database struct
 * @param dbi Database iterator to initialize
 */
extern void CDB_Iterator_init(VDB *db, VDB_Iterator *dbi);

/**
 * Get the next entry
 *
 * The order of entries returned by iterator is undefined
 * Depends on how keys hash.
 *
 * @param dbi Database iterator
 * @param kbuf Buffer to fill with next key (key_size bytes)
 * @param vbuf Buffer to fill with next value (value_size bytes)
 * @return 0 if there are no more entries, negative on error, positive if kbuf/vbuf have been filled
 */
extern int VDB_Iterator_next(VDB_Iterator *dbi, void *kbuf, void *vbuf);

#ifdef __cplusplus
}
#endif

#endif
