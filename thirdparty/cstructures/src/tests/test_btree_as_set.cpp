#include "gmock/gmock.h"
#include "cstructures/btree.h"

#define NAME btree_set

using namespace testing;

struct data_t
{
    float x, y, z;
};

TEST(NAME, init_sets_correct_values)
{
    struct cs_btree btree;
    btree.count = 4;
    btree.capacity = 56;
    btree.data = (uint8_t*)4783;
    btree.value_size = 283;

    ASSERT_THAT(btree_init(&btree, 0), Eq(BTREE_OK));
    EXPECT_THAT(btree.count, Eq(0u));
    EXPECT_THAT(btree.capacity, Eq(0u));
    EXPECT_THAT(btree.count, Eq(0u));
    EXPECT_THAT(btree.data, IsNull());
    EXPECT_THAT(btree.value_size, Eq(0));
}

TEST(NAME, create_initializes_btree)
{
    struct cs_btree* btree;
    ASSERT_THAT(btree_create(&btree, 0), Eq(BTREE_OK));
    EXPECT_THAT(btree_capacity(btree), Eq(0u));
    EXPECT_THAT(btree_count(btree), Eq(0u));
    EXPECT_THAT(btree->data, IsNull());
    EXPECT_THAT(btree->value_size, Eq(0));
    btree_free(btree);
}

TEST(NAME, insertion_forwards)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    ASSERT_THAT(btree_insert_new(&btree, 0, NULL), Eq(BTREE_OK));  ASSERT_THAT(btree_count(&btree), Eq(1u));
    ASSERT_THAT(btree_insert_new(&btree, 1, NULL), Eq(BTREE_OK));  ASSERT_THAT(btree_count(&btree), Eq(2u));
    ASSERT_THAT(btree_insert_new(&btree, 2, NULL), Eq(BTREE_OK));  ASSERT_THAT(btree_count(&btree), Eq(3u));
    ASSERT_THAT(btree_insert_new(&btree, 3, NULL), Eq(BTREE_OK));  ASSERT_THAT(btree_count(&btree), Eq(4u));
    ASSERT_THAT(btree_insert_new(&btree, 4, NULL), Eq(BTREE_OK));  ASSERT_THAT(btree_count(&btree), Eq(5u));

    EXPECT_THAT(btree_key_exists(&btree, 0), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 1), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 2), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 3), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 4), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 5), IsFalse());

    btree_deinit(&btree);
}

TEST(NAME, clear_keeps_underlying_buffer)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    btree_insert_new(&btree, 0, NULL);
    btree_insert_new(&btree, 1, NULL);
    btree_insert_new(&btree, 2, NULL);

    // this should delete all entries but keep the underlying buffer
    btree_clear(&btree);

    EXPECT_THAT(btree_count(&btree), Eq(0u));
    EXPECT_THAT(btree.data, NotNull());
    EXPECT_THAT(btree_capacity(&btree), Ne(0u));

    btree_deinit(&btree);
}

TEST(NAME, compact_when_no_buffer_is_allocated_doesnt_crash)
{
    struct cs_btree btree;
    btree_init(&btree, 0);
    btree_compact(&btree);
    btree_deinit(&btree);
}

TEST(NAME, compact_reduces_capacity_and_keeps_elements_in_tact)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    for (int i = 0; i != CSTRUCTURES_BTREE_MIN_CAPACITY * 3; ++i)
        ASSERT_THAT(btree_insert_new(&btree, i, NULL), Eq(BTREE_OK));
    for (int i = 0; i != CSTRUCTURES_BTREE_MIN_CAPACITY; ++i)
        btree_erase(&btree, i);

    cs_btree_size old_capacity = btree_capacity(&btree);
    btree_compact(&btree);
    EXPECT_THAT(btree_capacity(&btree), Lt(old_capacity));
    EXPECT_THAT(btree_count(&btree), Eq(CSTRUCTURES_BTREE_MIN_CAPACITY * 2));
    EXPECT_THAT(btree_capacity(&btree), Eq(CSTRUCTURES_BTREE_MIN_CAPACITY * 2));
    EXPECT_THAT(btree.data, NotNull());

    btree_deinit(&btree);
}

TEST(NAME, clear_and_compact_deletes_underlying_buffer)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    btree_insert_new(&btree, 0, NULL);
    btree_insert_new(&btree, 1, NULL);
    btree_insert_new(&btree, 2, NULL);

    // this should delete all entries + free the underlying buffer
    btree_clear(&btree);
    btree_compact(&btree);

    ASSERT_THAT(btree_count(&btree), Eq(0u));
    ASSERT_THAT(btree.data, IsNull());
    ASSERT_THAT(btree_capacity(&btree), Eq(0u));

    btree_deinit(&btree);
}

TEST(NAME, insert_new_existing_keys_fails)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    EXPECT_THAT(btree_insert_new(&btree, 0, NULL), Eq(BTREE_OK));
    EXPECT_THAT(btree_insert_new(&btree, 0, NULL), Eq(BTREE_EXISTS));
}

TEST(NAME, key_exists_returns_false_if_key_doesnt_exist)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    btree_insert_new(&btree, 3, NULL);
    btree_insert_new(&btree, 8, NULL);
    btree_insert_new(&btree, 2, NULL);

    EXPECT_THAT(btree_key_exists(&btree, 4), IsFalse());

    btree_deinit(&btree);
}

TEST(NAME, key_exists_returns_true_if_key_does_exist)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    btree_insert_new(&btree, 3, NULL);
    btree_insert_new(&btree, 8, NULL);
    btree_insert_new(&btree, 2, NULL);

    EXPECT_THAT(btree_key_exists(&btree, 3), IsTrue());

    btree_deinit(&btree);
}

TEST(NAME, find_unused_key_actually_returns_an_unused_key)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    cs_btree_key kb, kc, kd;
    btree_insert_new(&btree, 2, NULL);
    btree_insert_new(&btree, kb = btree_find_unused_key(&btree), NULL);
    btree_insert_new(&btree, kc = btree_find_unused_key(&btree), NULL);
    btree_insert_new(&btree, kd = btree_find_unused_key(&btree), NULL);

    EXPECT_THAT(btree_count(&btree), Eq(4u));
    EXPECT_THAT(btree_key_exists(&btree, 2), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, kb), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, kc), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, kd), IsTrue());
    EXPECT_THAT(kb, AllOf(Ne(kc), Ne(kd)));
    EXPECT_THAT(kc, Ne(kd));

    btree_deinit(&btree);
}

TEST(NAME, erase_by_key)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    int a=56, b=45, c=18, d=27, e=84;
    btree_insert_new(&btree, 0, NULL);
    btree_insert_new(&btree, 1, NULL);
    btree_insert_new(&btree, 2, NULL);
    btree_insert_new(&btree, 3, NULL);
    btree_insert_new(&btree, 4, NULL);

    EXPECT_THAT(btree_erase(&btree, 2), Eq(BTREE_OK));

    // 4
    EXPECT_THAT(btree_key_exists(&btree, 0), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 1), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 2), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 3), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 4), IsTrue());

    EXPECT_THAT(btree_erase(&btree, 4), Eq(BTREE_OK));

    // 3
    EXPECT_THAT(btree_key_exists(&btree, 0), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 1), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 2), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 3), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 4), IsFalse());

    EXPECT_THAT(btree_erase(&btree, 0), Eq(BTREE_OK));

    // 2
    EXPECT_THAT(btree_key_exists(&btree, 0), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 1), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 2), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 3), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 4), IsFalse());

    EXPECT_THAT(btree_erase(&btree, 1), Eq(BTREE_OK));

    // 1
    EXPECT_THAT(btree_key_exists(&btree, 0), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 1), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 2), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 3), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 4), IsFalse());

    EXPECT_THAT(btree_erase(&btree, 3), Eq(BTREE_OK));

    // 0
    EXPECT_THAT(btree_key_exists(&btree, 0), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 1), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 2), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 3), IsFalse());
    EXPECT_THAT(btree_key_exists(&btree, 4), IsFalse());

    EXPECT_THAT(btree_erase(&btree, 0), Eq(BTREE_NOT_FOUND));
    EXPECT_THAT(btree_erase(&btree, 1), Eq(BTREE_NOT_FOUND));
    EXPECT_THAT(btree_erase(&btree, 2), Eq(BTREE_NOT_FOUND));
    EXPECT_THAT(btree_erase(&btree, 3), Eq(BTREE_NOT_FOUND));
    EXPECT_THAT(btree_erase(&btree, 4), Eq(BTREE_NOT_FOUND));

    btree_deinit(&btree);
}

TEST(NAME, reinsertion_forwards)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    ASSERT_THAT(btree_insert_new(&btree, 0, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 1, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 2, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 3, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 4, NULL), Eq(BTREE_OK));

    ASSERT_THAT(btree_erase(&btree, 4), Eq(BTREE_OK));
    ASSERT_THAT(btree_erase(&btree, 3), Eq(BTREE_OK));
    ASSERT_THAT(btree_erase(&btree, 2), Eq(BTREE_OK));

    ASSERT_THAT(btree_insert_new(&btree, 2, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 3, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 4, NULL), Eq(BTREE_OK));

    EXPECT_THAT(btree_key_exists(&btree, 0), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 1), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 2), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 3), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 4), IsTrue());

    btree_deinit(&btree);
}

TEST(NAME, reinsertion_backwards)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    ASSERT_THAT(btree_insert_new(&btree, 0, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 1, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 2, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 3, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 4, NULL), Eq(BTREE_OK));

    ASSERT_THAT(btree_erase(&btree, 0), Eq(BTREE_OK));
    ASSERT_THAT(btree_erase(&btree, 1), Eq(BTREE_OK));
    ASSERT_THAT(btree_erase(&btree, 2), Eq(BTREE_OK));

    ASSERT_THAT(btree_insert_new(&btree, 2, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 1, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 0, NULL), Eq(BTREE_OK));

    EXPECT_THAT(btree_key_exists(&btree, 0), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 1), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 2), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 3), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 4), IsTrue());

    btree_deinit(&btree);
}

TEST(NAME, reinsertion_random)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    ASSERT_THAT(btree_insert_new(&btree, 26, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 44, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 82, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 41, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 70, NULL), Eq(BTREE_OK));

    ASSERT_THAT(btree_erase(&btree, 44), Eq(BTREE_OK));
    ASSERT_THAT(btree_erase(&btree, 70), Eq(BTREE_OK));
    ASSERT_THAT(btree_erase(&btree, 26), Eq(BTREE_OK));

    ASSERT_THAT(btree_insert_new(&btree, 26, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 70, NULL), Eq(BTREE_OK));
    ASSERT_THAT(btree_insert_new(&btree, 44, NULL), Eq(BTREE_OK));

    EXPECT_THAT(btree_key_exists(&btree, 26), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 44), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 82), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 41), IsTrue());
    EXPECT_THAT(btree_key_exists(&btree, 70), IsTrue());

    btree_deinit(&btree);
}

TEST(NAME, iterate_with_no_items)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    int counter = 0;
    BTREE_KEYS_FOR_EACH(&btree, key)
        ++counter;
    BTREE_END_EACH
    ASSERT_THAT(counter, Eq(0));

    btree_deinit(&btree);
}


TEST(NAME, iterate_5_random_items)
{
    struct cs_btree btree;
    btree_init(&btree, 0);

    btree_insert_new(&btree, 243, NULL);
    btree_insert_new(&btree, 256, NULL);
    btree_insert_new(&btree, 456, NULL);
    btree_insert_new(&btree, 468, NULL);
    btree_insert_new(&btree, 969, NULL);

    int counter = 0;
    BTREE_KEYS_FOR_EACH(&btree, key)
        switch(counter)
        {
            case 0 : ASSERT_THAT(key, Eq(243u)); break;
            case 1 : ASSERT_THAT(key, Eq(256u)); break;
            case 2 : ASSERT_THAT(key, Eq(456u)); break;
            case 3 : ASSERT_THAT(key, Eq(468u)); break;
            case 4 : ASSERT_THAT(key, Eq(969u)); break;
            default: ASSERT_THAT(1, Eq(0)); break;
        }
        ++counter;
    BTREE_END_EACH
    ASSERT_THAT(counter, Eq(5));

    btree_deinit(&btree);
}
