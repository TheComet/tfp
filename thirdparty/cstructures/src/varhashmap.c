#include "cstructures/varhashmap.h"
#include "cstructures/memory.h"
#include <string.h>
#include <assert.h>

#define SLOT(hm, pos)  (*(cs_hash32*)((uint8_t*)hm->storage + (sizeof(cs_hash32) + sizeof(void*)) * pos))
#define KEY(hm, pos)   (*(const void**)((uint8_t*)hm->storage + (sizeof(cs_hash32) + sizeof(void*)) * pos + sizeof(cs_hash32)))
#define VALUE(hm, pos) ((void*)((uint8_t*)hm->storage + (sizeof(cs_hash32) + sizeof(void*)) * hm->table_count + hm->value_size * pos))

/* ------------------------------------------------------------------------- */
/*
 * Need to account for the possibility that our hash function will produce
 * reserved values. In this case, return a value that is not reserved in a
 * predictable way.
 */
static cs_hash32
hash_wrapper(const struct cs_varhashmap* hm, const void* data)
{
    cs_hash32 hash = hm->hash(data);
    if (hash == HM_SLOT_UNUSED || hash == HM_SLOT_RIP || hash == HM_SLOT_INVALID)
        return 2;
    return hash;
}

/* ------------------------------------------------------------------------- */
static void*
malloc_and_init_storage(cs_hash32 value_size, cs_hash32 table_count)
{
    /* Store the hashes, keys and values in one contiguous chunk of memory */
    void* storage = MALLOC((sizeof(cs_hash32) + sizeof(void*) + value_size) * table_count);
    if (storage == NULL)
        return NULL;

    /* Initialize hash table -- NOTE: Only works if HM_SLOT_UNUSED is 0 */
    memset(storage, 0, (sizeof(cs_hash32) + sizeof(void*)) * table_count);
    return storage;
}

/* ------------------------------------------------------------------------- */
static int
resize_rehash(struct cs_varhashmap* hm, cs_hash32 new_table_count)
{
    struct cs_varhashmap new_hm;
    cs_hash32 i;

    memcpy(&new_hm, hm, sizeof(struct cs_varhashmap));
    new_hm.table_count = new_table_count;
    new_hm.slots_used = 0;
    new_hm.storage = malloc_and_init_storage(hm->value_size, new_table_count);
    if (new_hm.storage == NULL)
        return -1;

    for (i = 0; i != hm->table_count; ++i)
    {
        if (SLOT(hm, i) == HM_SLOT_UNUSED || SLOT(hm, i) == HM_SLOT_RIP)
            continue;
        if (varhashmap_insert(&new_hm, KEY(hm, i), VALUE(hm, i)) != HM_OK)
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
enum cs_varhashmap_status
varhashmap_create(
        struct cs_varhashmap** hm,
        uint32_t value_size,
        cs_hash32 (*hash_func)(const void* data),
        int (*compare_keys_func)(const void* a, const void* b))
{
    return varhashmap_create_with_options(
            hm,
            value_size,
            hash_func,
            compare_keys_func,
            HM_DEFAULT_TABLE_COUNT);
}

/* ------------------------------------------------------------------------- */
CSTRUCTURES_PRIVATE_API enum cs_varhashmap_status
varhashmap_create_with_options(
        struct cs_varhashmap** hm,
        uint32_t value_size,
        cs_hash32 (*hash_func)(const void* data),
        int (*compare_keys_func)(const void* a, const void* b),
        uint32_t table_count)
{
    *hm = MALLOC(sizeof(**hm));
    if (*hm == NULL)
        return HM_OOM;

    return varhashmap_init_with_options(
            *hm,
            value_size,
            hash_func,
            compare_keys_func,
            table_count);
}

/* ------------------------------------------------------------------------- */
enum cs_varhashmap_status
varhashmap_init(
        struct cs_varhashmap* hm,
        uint32_t value_size,
        cs_hash32 (*hash_func)(const void* data),
        int (*compare_keys_func)(const void* a, const void* b))
{
    return varhashmap_init_with_options(
            hm,
            value_size,
            hash_func,
            compare_keys_func,
            HM_DEFAULT_TABLE_COUNT);
}

/* ------------------------------------------------------------------------- */
enum cs_varhashmap_status
varhashmap_init_with_options(
        struct cs_varhashmap* hm,
        uint32_t value_size,
        cs_hash32 (*hash_func)(const void* data),
        int (*compare_keys_func)(const void* a, const void* b),
        uint32_t table_count)
{
    assert(hm);
    assert(hash_func);
    assert(compare_keys_func);
    assert(table_count > 0);

    hm->value_size = value_size;
    hm->hash = hash_func;
    hm->slots_used = 0;
    hm->table_count = table_count;
    hm->storage = malloc_and_init_storage(hm->value_size, hm->table_count);
    if (hm->storage == NULL)
        return HM_OOM;

    return HM_OK;
}

/* ------------------------------------------------------------------------- */
void
varhashmap_deinit(struct cs_varhashmap* hm)
{
    FREE(hm->storage);
}

/* ------------------------------------------------------------------------- */
void
varhashmap_free(struct cs_varhashmap* hm)
{
    varhashmap_deinit(hm);
    FREE(hm);
}

/* ------------------------------------------------------------------------- */
enum cs_varhashmap_status
varhashmap_insert(struct cs_varhashmap* hm, const void* key, const void* value)
{
    cs_hash32 hash, pos, i, last_tombstone;

    /* NOTE: Rehashing may change table count, make sure to compute hash after this */
    if (hm->slots_used * 100 / hm->table_count >= HM_REHASH_AT_PERCENT)
        if (resize_rehash(hm, hm->table_count * HM_EXPAND_FACTOR) != 0)
            return HM_OOM;

    /* Init values */
    hash = hash_wrapper(hm, key);
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
            if (hm->compare_keys(KEY(hm, pos), key) == 0)
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
    }

    /* It's safe to insert new values at the end of a probing sequence */
    if (last_tombstone != HM_SLOT_INVALID)
        pos = last_tombstone;

    /* Store hash, key and value */
    SLOT(hm, pos) = hash;
    KEY(hm, pos) = key;
    if (value)  /* value may be NULL, and memcpy() with a NULL source is undefined, even if len is 0 */
        memcpy(VALUE(hm, pos), value, hm->value_size);

    hm->slots_used++;

    return HM_OK;
}

/* ------------------------------------------------------------------------- */
void*
varhashmap_erase(struct cs_varhashmap* hm, const void* key)
{
    cs_hash32 hash = hash_wrapper(hm, key);
    cs_hash32 pos = hash % hm->table_count;
    cs_hash32 i = 0;

    while (1)
    {
        if (SLOT(hm, pos) == hash)
        {
            if (hm->compare_keys(KEY(hm, pos), key) == 0)
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

    hm->slots_used--;

    SLOT(hm, pos) = HM_SLOT_RIP;
    return VALUE(hm, pos);
}

/* ------------------------------------------------------------------------- */
void*
varhashmap_find(const struct cs_varhashmap* hm, const void* key)
{
    cs_hash32 hash = hash_wrapper(hm, key);
    cs_hash32 pos = hash % hm->table_count;
    cs_hash32 i = 0;
    while (1)
    {
        if (SLOT(hm, pos) == hash)
        {
            if (hm->compare_keys(KEY(hm, pos), key) == 0)
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
