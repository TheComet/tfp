#pragma once

#include "cstructures/config.h"
#include "cstructures/hash.h"

#define HM_SLOT_UNUSED    ((cs_hash32)0)
#define HM_SLOT_RIP       ((cs_hash32)1)
#define HM_SLOT_INVALID   ((cs_hash32)-1)
#define HM_REHASH_AT_PERCENT   70
#define HM_DEFAULT_TABLE_COUNT 128
#define HM_EXPAND_FACTOR 3

C_BEGIN

enum cs_varhashmap_status
{
    HM_EXISTS = 1,
    HM_OK = 0,
    HM_OOM = -1
};

struct cs_varhashmap
{
    uint32_t     table_count;
    uint32_t     value_size;
    uint32_t     slots_used;
    cs_hash32    (*hash)(const void* data);
    int          (*compare_keys)(const void* a, const void* b);
    void*        storage;
};

/*!
 * @brief Allocates and initializes a new hashmap.
 * @param[out] hm A pointer to the new hashmap is written to this parameter.
 * Example:
 * ```cpp
 * struct hashmap_t* hm;
 * if (hashmap_create(&hm, sizeof(key_t), sizeof(value_t)) != CSTRUCTURES_OK)
 *     handle_error();
 * ```
 * @param[in] key_size Specifies how many bytes of the "key" parameter to hash
 * in the hashmap_insert() call. Due to performance reasons, all keys are
 * identical in size. If you wish to use strings for keys, then you need to
 * specify the maximum possible string length here, and make sure you never
 * use strings that are longer than that (hashmap_insert_key contains a safety
 * check in debug mode for this case).
 * @note This parameter must be larger than 0.
 * @param[in] value_size Specifies how many bytes long the value type is. When
 * calling hashmap_insert(), value_size number of bytes are copied from the
 * memory pointed to by value into the hashmap.
 * @note This parameter may be 0.
 * @return If successful, returns CSTRUCTURES_OK. If allocation fails,
 * CSTRUCTURES_ERR_OUT_OF_MEMORY is returned.
 */
CSTRUCTURES_PRIVATE_API enum cs_varhashmap_status
varhashmap_create(
        struct cs_varhashmap** hm,
        uint32_t value_size,
        cs_hash32 (*hash_func)(const void* data),
        int (*compare_keys)(const void* a, const void* b));

CSTRUCTURES_PRIVATE_API enum cs_varhashmap_status
varhashmap_create_with_options(
        struct cs_varhashmap** hm,
        uint32_t value_size,
        cs_hash32 (*hash_func)(const void* data),
        int (*compare_keys_func)(const void* a, const void* b),
        uint32_t table_count);

/*!
 * @brief Initializes a new hashmap. See hashmap_create() for details on
 * parameters and return values.
 */
CSTRUCTURES_PRIVATE_API enum cs_varhashmap_status
varhashmap_init(
        struct cs_varhashmap* hm,
        uint32_t value_size,
        cs_hash32 (*hash_func)(const void* data),
        int (*compare_keys_func)(const void* a, const void* b));

CSTRUCTURES_PRIVATE_API enum cs_varhashmap_status
varhashmap_init_with_options(
        struct cs_varhashmap* hm,
        uint32_t value_size,
        cs_hash32 (*hash_func)(const void* data),
        int (*compare_keys_func)(const void* a, const void* b),
        uint32_t table_count);

/*!
 * @brief Cleans up internal resources without freeing the hashmap object itself.
 */
CSTRUCTURES_PRIVATE_API void
varhashmap_deinit(struct cs_varhashmap* hm);

/*!
 * @brief Cleans up all resources and frees the hashmap.
 */
CSTRUCTURES_PRIVATE_API void
varhashmap_free(struct cs_varhashmap* hm);

CSTRUCTURES_PRIVATE_API enum cs_varhashmap_status
varhashmap_reserve(struct cs_varhashmap* hm,
                uint32_t table_count);

CSTRUCTURES_PRIVATE_API enum cs_varhashmap_status
varhashmap_insert(
        struct cs_varhashmap* hm,
        const void* key,
        const void* value);

CSTRUCTURES_PRIVATE_API void*
varhashmap_erase(struct cs_varhashmap* hm, const void* key);

CSTRUCTURES_PRIVATE_API void*
varhashmap_find(const struct cs_varhashmap* hm, const void* key);

#define varhashmap_count(hm) ((hm)->slots_used)

#define VARHASHMAP_FOR_EACH(hm, key_t, value_t, key, value) { \
    key_t* key; \
    value_t* value; \
    cs_hash32 pos_##value; \
    for (pos_##value = 0; \
        pos_##value != (hm)->table_count && \
            ((key = (key_t*)((uint8_t*)(hm)->storage + (sizeof(cs_hash32) + (hm)->key_size) * pos_##value + sizeof(cs_hash32))) || 1) && \
            ((value = (value_t*)((uint8_t*)(hm)->storage + (sizeof(cs_hash32) + (hm)->key_size) * (hm)->table_count + (hm)->value_size * pos_##value)) || 1); \
        ++pos_##value) \
    { \
        cs_hash32 slot_##value = *(cs_hash32*)((uint8_t*)(hm)->storage + (sizeof(cs_hash32) + (hm)->key_size) * pos_##value); \
        if (slot_##value == HM_SLOT_UNUSED || slot_##value == HM_SLOT_RIP || slot_##value == HM_SLOT_INVALID) \
            continue; \

#define VARHASHMAP_END_EACH }}

C_END
