#include "cstructures/hashmap.h"
#include "cstructures/memory.h"
#include <string.h>
#include <assert.h>

#define SLOT(hm, pos)  (*(cs_hash32*)((uint8_t*)hm->storage + (sizeof(cs_hash32) + hm->key_size) * pos))
#define KEY(hm, pos)   ((void*)((uint8_t*)hm->storage + (sizeof(cs_hash32) + hm->key_size) * pos + sizeof(cs_hash32)))
#define VALUE(hm, pos) ((void*)((uint8_t*)hm->storage + (sizeof(cs_hash32) + hm->key_size) * hm->table_count + hm->value_size * pos))

#ifdef CSTRUCTURES_HASHMAP_STATS
#   include <stdio.h>
#   define STATS_INIT(hm) \
            hm->stats.total_insertions = 0; \
            hm->stats.total_deletions = 0; \
            hm->stats.total_tombstones = 0; \
            hm->stats.total_tombstone_reuses = 0; \
            hm->stats.total_rehashes = 0; \
            hm->stats.total_insertion_probes = 0; \
            hm->stats.total_deletion_probes = 0; \
            hm->stats.max_slots_used = 0; \
            hm->stats.max_slots_tombstoned = 0; \
            hm->stats.current_tombstone_count = 0

#   define STATS_INSERTION_PROBE(hm) \
            hm->stats.total_insertion_probes++

#   define STATS_DELETION_PROBE(hm) \
            hm->stats.total_deletion_probes++

#   define STATS_INSERTED_IN_UNUSED(hm) do { \
            hm->stats.total_insertions++; \
            if (hm->slots_used > hm->stats.max_slots_used) \
                hm->stats.max_slots_used = hm->slots_used; \
            } while (0)

#   define STATS_INSERTED_IN_TOMBSTONE(hm) do { \
            hm->stats.total_tombstone_reuses++; \
            hm->stats.total_insertions++; \
            if (hm->slots_used > hm->stats.max_slots_used) \
                hm->stats.max_slots_used = hm->slots_used; \
            } while (0)

#   define STATS_DELETED(hm) do { \
            hm->stats.total_deletions++; \
            hm->stats.total_tombstones++; \
            hm->stats.current_tombstone_count++; \
            if (hm->stats.current_tombstone_count > hm->stats.max_slots_tombstoned) \
                hm->stats.max_slots_tombstoned = hm->stats.current_tombstone_count; \
            } while (0)

#   define STATS_REHASH(hm) do { \
            hm->stats.total_rehashes++; \
            hm->stats.current_tombstone_count = 0; \
            } while (0)

#   define STATS_REPORT(hm) do { \
            fprintf(stderr, \
                    "[hashmap stats] key size: %d, value size: %d\n" \
                    "  total insertions:        %lu\n" \
                    "  total deletions:         %lu\n" \
                    "  total tombstones:        %lu\n" \
                    "  total tombestone reuses: %lu\n" \
                    "  total rehashes:          %lu\n" \
                    "  total insertion probes:  %lu\n" \
                    "  total deletion probes:   %lu\n" \
                    "  max slots used:          %lu\n" \
                    "  max slots tombstoned:    %lu\n" \
                    , hm->key_size \
                    , hm->value_size \
                    , hm->stats.total_insertions \
                    , hm->stats.total_deletions \
                    , hm->stats.total_tombstones \
                    , hm->stats.total_tombstone_reuses \
                    , hm->stats.total_rehashes \
                    , hm->stats.total_insertion_probes \
                    , hm->stats.total_deletion_probes \
                    , hm->stats.max_slots_used \
                    , hm->stats.max_slots_tombstoned); \
            } while (0)

#else
#   define STATS_INIT(hm)
#   define STATS_INSERTION_PROBE(hm)
#   define STATS_DELETION_PROBE(hm)
#   define STATS_INSERTED_IN_UNUSED(hm)
#   define STATS_INSERTED_IN_TOMBSTONE(hm)
#   define STATS_DELETED(hm)
#   define STATS_REHASH(hm)
#   define STATS_REPORT(hm)
#endif

/* ------------------------------------------------------------------------- */
/*
 * Need to account for the possibility that our hash function will produce
 * reserved values. In this case, return a value that is not reserved in a
 * predictable way.
 */
static cs_hash32
hash_wrapper(const struct cs_hashmap* hm, const void* data, cs_hash32 len)
{
    cs_hash32 hash = hm->hash(data, len);
    if (hash == HM_SLOT_UNUSED || hash == HM_SLOT_RIP || hash == HM_SLOT_INVALID)
        return 2;
    return hash;
}

/* ------------------------------------------------------------------------- */
static void*
malloc_and_init_storage(cs_hash32 key_size, cs_hash32 value_size, cs_hash32 table_count)
{
    /* Store the hashes, keys and values in one contiguous chunk of memory */
    void* storage = MALLOC((sizeof(cs_hash32) + key_size + value_size) * table_count);
    if (storage == NULL)
        return NULL;

    /* Initialize hash table -- NOTE: Only works if HM_SLOT_UNUSED is 0 */
    memset(storage, 0, (sizeof(cs_hash32) + key_size) * table_count);
    return storage;
}

/* ------------------------------------------------------------------------- */
static int
resize_rehash(struct cs_hashmap* hm, cs_hash32 new_table_count)
{
    struct cs_hashmap new_hm;
    cs_hash32 i;

    STATS_REHASH(hm);

    memcpy(&new_hm, hm, sizeof(struct cs_hashmap));
    new_hm.table_count = new_table_count;
    new_hm.slots_used = 0;
    new_hm.storage = malloc_and_init_storage(hm->key_size, hm->value_size, new_table_count);
    if (new_hm.storage == NULL)
        return -1;

    for (i = 0; i != hm->table_count; ++i)
    {
        if (SLOT(hm, i) == HM_SLOT_UNUSED || SLOT(hm, i) == HM_SLOT_RIP)
            continue;
        if (hashmap_insert(&new_hm, KEY(hm, i), VALUE(hm, i)) != HM_OK)
        {
            FREE(new_hm.storage);
            return -1;
        }
    }

    /* Swap storage and free old */
    FREE(hm->storage);
    hm->storage = new_hm.storage;
    hm->table_count = new_table_count;

    return 0;
}

/* ------------------------------------------------------------------------- */
enum cs_hashmap_status
hashmap_create(struct cs_hashmap** hm, cs_hash32 key_size, cs_hash32 value_size)
{
    return hashmap_create_with_options(hm, key_size, value_size,
                                       HM_DEFAULT_TABLE_COUNT,
                                       hash32_jenkins_oaat);
}

/* ------------------------------------------------------------------------- */
CSTRUCTURES_PRIVATE_API enum cs_hashmap_status
hashmap_create_with_options(struct cs_hashmap** hm,
                            uint32_t key_size,
                            uint32_t value_size,
                            uint32_t table_count,
                            hash32_func hash_func)
{
    *hm = MALLOC(sizeof(**hm));
    if (*hm == NULL)
        return HM_OOM;

    return hashmap_init_with_options(*hm, key_size, value_size,
                                     table_count, hash_func);
}

/* ------------------------------------------------------------------------- */
enum cs_hashmap_status
hashmap_init(struct cs_hashmap* hm, cs_hash32 key_size, cs_hash32 value_size)
{
    return hashmap_init_with_options(hm, key_size, value_size,
                                     HM_DEFAULT_TABLE_COUNT, hash32_jenkins_oaat);
}

/* ------------------------------------------------------------------------- */
enum cs_hashmap_status
hashmap_init_with_options(struct cs_hashmap* hm,
                          uint32_t key_size,
                          uint32_t value_size,
                          uint32_t table_count,
                          hash32_func hash_func)
{
    assert(hm);
    assert(key_size > 0);
    assert(table_count > 0);
    assert(hash_func);

    hm->key_size = key_size;
    hm->value_size = value_size;
    hm->hash = hash_func;
    hm->slots_used = 0;
    hm->table_count = table_count;
    hm->storage = malloc_and_init_storage(hm->key_size, hm->value_size, hm->table_count);
    if (hm->storage == NULL)
        return HM_OOM;

    STATS_INIT(hm);

    return HM_OK;
}

/* ------------------------------------------------------------------------- */
void
hashmap_deinit(struct cs_hashmap* hm)
{
    STATS_REPORT(hm);
    FREE(hm->storage);
}

/* ------------------------------------------------------------------------- */
void
hashmap_free(struct cs_hashmap* hm)
{
    hashmap_deinit(hm);
    FREE(hm);
}

/* ------------------------------------------------------------------------- */
enum cs_hashmap_status
hashmap_insert(struct cs_hashmap* hm, const void* key, const void* value)
{
    cs_hash32 hash, pos, i, last_tombstone;

    /* NOTE: Rehashing may change table count, make sure to compute hash after this */
    if (hm->slots_used * 100 / hm->table_count >= HM_REHASH_AT_PERCENT)
        if (resize_rehash(hm, hm->table_count * HM_EXPAND_FACTOR) != 0)
            return HM_OOM;

    /* Init values */
    hash = hash_wrapper(hm, key, hm->key_size);
    pos = hash % hm->table_count;
    i = 0;
    last_tombstone = HM_SLOT_INVALID;

    while (SLOT(hm, pos) != HM_SLOT_UNUSED)
    {
        /* If the same hash already exists in this slot, and this isn't the
         * result of a hash collision (which we can verify by comparing the
         * original keys), then we can conclude this key was already inserted */
        if (SLOT(hm, pos) == hash)
        {
            if (memcmp(KEY(hm, pos), key, hm->key_size) == 0)
                return HM_EXISTS;
        }
        else
            if (SLOT(hm, pos) == HM_SLOT_RIP)
                last_tombstone = pos;

        /* Quadratic probing following p(K,i)=(i^2+i)/2. If the hash table
         * size is a power of two, this will visit every slot */
        i++;
        pos += i;
        pos = pos % hm->table_count;
        STATS_INSERTION_PROBE(hm);
    }

    /* It's safe to insert new values at the end of a probing sequence */
    if (last_tombstone != HM_SLOT_INVALID)
    {
        pos = last_tombstone;
        STATS_INSERTED_IN_TOMBSTONE(hm);
    }
    else
    {
        STATS_INSERTED_IN_UNUSED(hm);
    }

    /* Store hash, key and value */
    SLOT(hm, pos) = hash;
    memcpy(KEY(hm, pos), key, hm->key_size);
    if (value)  /* value may be NULL, and memcpy() with a NULL source is undefined, even if len is 0 */
        memcpy(VALUE(hm, pos), value, hm->value_size);

    hm->slots_used++;

    return HM_OK;
}

/* ------------------------------------------------------------------------- */
void*
hashmap_erase(struct cs_hashmap* hm, const void* key)
{
    cs_hash32 hash = hash_wrapper(hm, key, hm->key_size);
    cs_hash32 pos = hash % hm->table_count;
    cs_hash32 i = 0;

    while (1)
    {
        if (SLOT(hm, pos) == hash)
        {
            if (memcmp(KEY(hm, pos), key, hm->key_size) == 0)
                break;
        }
        else
        {
            if (SLOT(hm, pos) == HM_SLOT_UNUSED)
                return NULL;
        }

        /* Quadratic probing following p(K,i)=(i^2+i)/2. If the hash table
         * size is a power of two, this will visit every slot */
        i++;
        pos += i;
        pos = pos % hm->table_count;
        STATS_DELETION_PROBE(hm);
    }

    hm->slots_used--;
    STATS_DELETED(hm);

    SLOT(hm, pos) = HM_SLOT_RIP;
    return VALUE(hm, pos);
}

/* ------------------------------------------------------------------------- */
void*
hashmap_find(const struct cs_hashmap* hm, const void* key)
{
    cs_hash32 hash = hash_wrapper(hm, key, hm->key_size);
    cs_hash32 pos = hash % hm->table_count;
    cs_hash32 i = 0;
    while (1)
    {
        if (SLOT(hm, pos) == hash)
        {
            if (memcmp(KEY(hm, pos), key, hm->key_size) == 0)
                break;
        }
        else
        {
            if (SLOT(hm, pos) == HM_SLOT_UNUSED)
                return NULL;
        }

        /* Quadratic probing following p(K,i)=(i^2+i)/2. If the hash table
         * size is a power of two, this will visit every slot */
        i++;
        pos += i;
        pos = pos % hm->table_count;
    }

    return VALUE(hm, pos);
}
