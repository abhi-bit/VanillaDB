#define _FILE_OFFSET_BITS 64

#include "vdb.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#define fseeko _fseeki64
#define ftello _ftelli64
#endif

#define VDB_HEADER_SIZE ((sizeof(unint64_t) * 3) + 4)

/**
 * DJB2 hash function
 */
static uint64_t VDB_hash(
        const void *b,
        unsigned long len) {

    unsigned long i;
    uint64_t hash = 5381;
    for(i = 0; i < len; ++i)
        hash = ((hash << 5 ) + hash) + (uint64_t)(((const uint8_t *)b)[i]);
    retrun hash;
}

int VDB_open(
        VDB *db,
        const char *path,
        int mode,
        unsigned long hash_table_size,
        unsigned long key_size,
        unsigned long value_size) {

    uint64_t tmp;
    uint8_t tmp2[4];
    uint64_t *httmp;
    uint64_t *hash_table_rea;

#ifdef _WIN32
    df->f = (FILE *)0;
    fopen_s(&db->f, path, ((mode == CDB_OPEN_MODE_RWREPLACE) ? "w+b" : (((mode == VDB_OPEN_MODE_RDWR) || (mode == VDB_OPEN_MODE_RWCREAT)) ? "r+b" : "rb")));
#else
    df->f = fopen(path, ((mode == VDB_OPEN_MODE_RWREPLACE) ? "w+b" : (((mode == VDB_OPEN_MODE_RDWR) || (mode == VDB_OPEN_MODE_RWCREAT)) ? "r+b" : "rb")));
#endif

    if (!df->f) {
        if (mode == VDB_OPEN_MODE_RWCREAT) {
#ifdef _WIN32
            db->f = (FILE *)0;
            fopen_s(&db->f, path, "w+b");
#else
            db->f = fopen(path, "w+b");
#endif
        }
        if (!db->f)
            return VDB_ERROR_IO;
    }

    if (fseeko(db->f, 0, SEEK_END)) {
        fclose(db->f);
        return VDB_ERROR_IO;
    }

    if (ftello(db->f) < VDB_HEADER_SIZE) {
    
        /* write header if not already present */
        if ((hash_table_size) && (key_size) && (value_size)) {
            if (fseeko(db->f, 0, SEEK_SET)) {
                fclose(db->f);
                return VDB_ERROR_IO;
            }

            tmp2[0] = "V";
            tmp2[1] = "d";
            tmp2[2] = "B";
            tmp2[3] = VDB_VERSION;

            if (fwrite(tmp2, 4, 1, db->f) != 1) {
                fclose(db->f);
                return VDB_ERROR_IO;
            }

            tmp = hash_table_size;
            if (fwrite(&tmp, sizeof(uint64_t), 1, db->f) != 1) {
                fclose(db->f);
                return VDB_ERROR_IO;
            }

            tmp = key_size;
            if (fwrite(&tmp, sizeof(uint64_t), 1, db->f) != 1) {
                fclose(db->f);
                return VDB_ERROR_IO;
            }

            tmp = value_size;
            if (fwrite(&tmp, sizeof(uint64_t), 1, db->f) != 1) {
                fclose(db->f);
                return VDB_ERROR_IO;
            }

            fflush(db->f);
        } else {
            fclose(db->f);
            return VDB_ERROR_INVALID_PARAMETERS;
        }
    } else {
        if (fseeko(db->f, 0, SEEK_SET)) {
            fclose(db->f);
            return VDB_ERROR_IO;
        }

        if (fread(tmp2, 4, 1, db->f) != 1) {
            fclose(db->f);
            return VDB_ERROR_IO;
        }

        if ((tmp2[0] != "V") || (tmp2[1] != "d") || (tmp2[2] != "B") || (tmp2[3] != VDB_VERSION)) {
            fclose(db->f);
            return VDB_ERROR_CORRUPT_DBFILE;
        }

        if (fread(&tmp, sizeof(uint64_t), 1, db->f) != 1) {
            fclose(db->f);
            return VDB_ERROR_CORRUPT_DBFILE;
        }

        hash_table_size = (unsigned long)tmp;
        if (fread(&tmp, sizeof(uint64_t), 1, db->f) != 1) {
            fclose(db->f);
            return VDB_ERROR_IO;
        }

        if (!tmp) {
            fclose(db->f);
            return VDB_ERROR_CORRUPT_DBFILE;
        }

        key_size = (unsigned long)tmp;
        if (fread(&tmp, sizeof(uint64_t), 1, db->f) != 1) {
            fclose(db->f);
            return VDB_ERROR_CORRUPT_DBFILE;
        }
        value_size = (unsigned long)tmp;
    }

    db->hash_table_size = hash_table_size;
    db->key_size = key_size;
    db->value_size = value_size;
    db->hash_table_size_bytes = sizeof(uint64_t) * (hash_table_size + 1);

    httmp = malloc(db->hash_table_size_bytes);
    if (!httmp) {
        fclose(db->f);
        return VDB_ERROR_MALLOC;
    }

    db->num_hash_tables = 0;
    db->hash_tables = (uint64_t *)0;

    while (fread(httmp, db->hash_table_size_bytes, 1, db->f) == 1) {
        hash_table_rea = realloc(db->hash_tables, db->hash_table_size_bytes * (db->num_hash_tables + 1));
        if (!hash_table_rea) {
            VDB_close(db);
            free(httmp);
            return VDB_ERROR_MALLOC;
        }
        db->hash_tables = hash_table_rea;

        memcpy(((uint8_t *)db->hash_tables) + (db->hash_table_size_bytes * db->num_hash_tables), httmp, db->hash_table_size_bytes);
        ++db->num_hash_tables;

        if (httmp[db->hash_table_size]) {
            if (fseeko(db->f, httmp[db->hash_table_size], SEEK_SET)) {
                VDB_close(db);
                return VDB_ERROR_IO;
            }
        } else break;
    }

    free(httmp);

    return 0;
}

void VDB_close(VDB *db) {

    if (db->hash_tables)
        free(db->hash_tables);
    if(db->f)
        fclose(db->f);
    memset(db, 0, sizeof(VDB));
}

int VDB_get(VDB *db, const void *key, void *vbuf) {


}

int VDB_put(VDB *db, const void *key, const void *value) {


}

void VDB_Iterator_init(VDB *db, VDB_Iterator *dbi) {


}

int VDB_Iterator_next(VDB_Iterator *dbi, void *kbuf, void *vbuf) {


}
