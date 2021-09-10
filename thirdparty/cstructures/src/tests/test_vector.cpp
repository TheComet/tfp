#include "gmock/gmock.h"
#include "cstructures/vector.h"

#define NAME vector

using namespace ::testing;

class NAME : public Test
{
public:
    void SetUp() override
    {
        vector_init(&vec, sizeof(int));
    }

    void TearDown() override
    {
        vector_deinit(&vec);
    }

    struct cs_vector vec;
};

TEST_F(NAME, init)
{
    vector_deinit(&vec);

    vec.capacity = 45;
    vec.count = 384;
    vec.data = (uint8_t*)4859;
    vec.element_size = 183;
    vector_init(&vec, sizeof(int));

    EXPECT_THAT(vec.capacity, Eq(0u));
    EXPECT_THAT(vec.count, Eq(0u));
    EXPECT_THAT(vec.data, IsNull());
    ASSERT_EQ(sizeof(int), vec.element_size);
}

TEST_F(NAME, create_initialises_vector)
{
    struct cs_vector* v = vector_create(sizeof(int));
    EXPECT_THAT(v->capacity, Eq(0u));
    EXPECT_THAT(vector_count(v), Eq(0u));
    EXPECT_THAT(v->data, IsNull());
    ASSERT_EQ(sizeof(int), v->element_size);
    vector_free(v);
}

TEST_F(NAME, push_increments_count_and_causes_realloc_by_factor)
{
    int x = 9;

    for(int i = 0; i != CSTRUCTURES_VEC_MIN_CAPACITY; ++i)
        vector_push(&vec, &x);

    EXPECT_THAT(vector_count(&vec), Eq(CSTRUCTURES_VEC_MIN_CAPACITY));
    EXPECT_THAT(vec.capacity, Eq(CSTRUCTURES_VEC_MIN_CAPACITY));

    vector_push(&vec, &x);
    EXPECT_THAT(vector_count(&vec), Eq(CSTRUCTURES_VEC_MIN_CAPACITY + 1));
    EXPECT_THAT(vec.capacity, Eq(CSTRUCTURES_VEC_MIN_CAPACITY * CSTRUCTURES_VEC_EXPAND_FACTOR));
}

TEST_F(NAME, clear_keeps_buffer_and_resets_count)
{
    int x = 9;
    for(int i = 0; i != CSTRUCTURES_VEC_MIN_CAPACITY*2; ++i)
        vector_push(&vec, &x);

    EXPECT_THAT(vector_count(&vec), Eq(CSTRUCTURES_VEC_MIN_CAPACITY*2));
    EXPECT_THAT(vec.capacity, Eq(CSTRUCTURES_VEC_MIN_CAPACITY*2));
    vector_clear(&vec);
    EXPECT_THAT(vector_count(&vec), Eq(0u));
    EXPECT_THAT(vec.capacity, Eq(CSTRUCTURES_VEC_MIN_CAPACITY*2));
    EXPECT_THAT(vec.data, NotNull());
}

TEST_F(NAME, clear_and_compact_deletes_buffer_and_resets_count)
{
    int x = 9;
    vector_push(&vec, &x);
    vector_clear(&vec);
    vector_compact(&vec);
    EXPECT_THAT(vector_count(&vec), Eq(0u));
    EXPECT_THAT(vec.capacity, Eq(0u));
    EXPECT_THAT(vec.data, IsNull());
}

TEST_F(NAME, emplace_increments_count_and_causes_realloc_by_factor)
{
    for(int i = 0; i != CSTRUCTURES_VEC_MIN_CAPACITY; ++i)
        vector_emplace(&vec);

    EXPECT_THAT(vector_count(&vec), Eq(CSTRUCTURES_VEC_MIN_CAPACITY));
    EXPECT_THAT(vec.capacity, Eq(CSTRUCTURES_VEC_MIN_CAPACITY));

    vector_emplace(&vec);
    EXPECT_THAT(vector_count(&vec), Eq(CSTRUCTURES_VEC_MIN_CAPACITY + 1));
    EXPECT_THAT(vec.capacity, Eq(CSTRUCTURES_VEC_MIN_CAPACITY * CSTRUCTURES_VEC_EXPAND_FACTOR));
}

TEST_F(NAME, pop_returns_pushed_values)
{
    int x;

    x = 3; vector_push(&vec, &x);
    x = 2; vector_push(&vec, &x);
    x = 6; vector_push(&vec, &x);
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(6));
    x = 23; vector_push(&vec, &x);
    x = 21; vector_push(&vec, &x);
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(21));
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(23));
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(2));
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(3));

    EXPECT_THAT(vector_count(&vec), Eq(0u));
    EXPECT_THAT(vec.data, NotNull());
}

TEST_F(NAME, pop_returns_emplaced_values)
{
    int* emplaced;

    emplaced = (int*)vector_emplace(&vec); *emplaced = 53;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 24;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 73;
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(73));
    emplaced = (int*)vector_emplace(&vec); *emplaced = 28;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 72;
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(72));
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(28));
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(24));
    EXPECT_THAT(*(int*)vector_pop(&vec), Eq(53));

    EXPECT_THAT(vector_count(&vec), Eq(0u));
    EXPECT_THAT(vec.data, NotNull());
}

TEST_F(NAME, pop_empty_vector)
{
    int* emplaced;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 21;
    vector_pop(&vec);
    EXPECT_THAT(vector_pop(&vec), IsNull());
    EXPECT_THAT(vector_count(&vec), Eq(0u));
    EXPECT_THAT(vec.data, NotNull());
}

TEST_F(NAME, pop_clear_freed_vector)
{
    EXPECT_THAT(vector_pop(&vec), IsNull());
    EXPECT_THAT(vector_count(&vec), Eq(0u));
    EXPECT_THAT(vec.capacity, Eq(0u));
    EXPECT_THAT(vec.data, IsNull());
}

TEST_F(NAME, get_element_random_access)
{
    int* emplaced;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 53;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 24;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 73;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 43;
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(53));
}

TEST_F(NAME, popping_preserves_existing_elements)
{
    int* emplaced;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 53;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 24;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 73;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 43;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 24;

    vector_pop(&vec);
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(53));
}

TEST_F(NAME, erasing_by_index_preserves_existing_elements)
{
    int* emplaced;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 53;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 24;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 73;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 43;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 65;

    vector_erase_index(&vec, 1);
    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(65));

    vector_erase_index(&vec, 1);
    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(65));
}

TEST_F(NAME, erasing_by_element_preserves_existing_elements)
{
    int* emplaced;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 53;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 24;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 73;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 43;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 65;

    vector_erase_element(&vec, vector_get_element(&vec, 1));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(65));

    vector_erase_element(&vec, vector_get_element(&vec, 1));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(65));
}

TEST_F(NAME, erase_invalid_index)
{
    int* emplaced;
    vector_erase_index(&vec, 1);
    vector_erase_index(&vec, 0);

    emplaced = (int*)vector_emplace(&vec); *emplaced = 53;
    vector_erase_index(&vec, 1);
    vector_erase_index(&vec, 0);
    vector_erase_index(&vec, 0);
}

TEST_F(NAME, inserting_preserves_existing_elements)
{
    int* emplaced;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 53;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 24;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 73;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 43;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 65;

    int x = 68;
    vector_insert(&vec, 2, &x); // middle insertion

    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(68));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 4), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 5), Eq(65));

    x = 16;
    vector_insert(&vec, 0, &x); // beginning insertion

    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(16));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(68));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 4), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 5), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 6), Eq(65));

    x = 82;
    vector_insert(&vec, 7, &x); // end insertion

    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(16));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(68));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 4), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 5), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 6), Eq(65));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 7), Eq(82));

    x = 37;
    vector_insert(&vec, 7, &x); // end insertion

    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(16));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(68));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 4), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 5), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 6), Eq(65));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 7), Eq(37));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 8), Eq(82));
}

TEST_F(NAME, insert_emplacing_preserves_existing_elements)
{
    int* emplaced;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 53;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 24;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 73;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 43;
    emplaced = (int*)vector_emplace(&vec); *emplaced = 65;

    emplaced = (int*)vector_insert_emplace(&vec, 2); *emplaced = 68; // middle insertion

    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(68));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 4), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 5), Eq(65));

    emplaced = (int*)vector_insert_emplace(&vec, 0); *emplaced = 16; // beginning insertion

    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(16));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(68));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 4), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 5), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 6), Eq(65));

    emplaced = (int*)vector_insert_emplace(&vec, 7); *emplaced = 82; // end insertion

    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(16));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(68));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 4), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 5), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 6), Eq(65));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 7), Eq(82));

    emplaced = (int*)vector_insert_emplace(&vec, 7); *emplaced = 37; // end insertion

    EXPECT_THAT(*(int*)vector_get_element(&vec, 0), Eq(16));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 1), Eq(53));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 2), Eq(24));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 3), Eq(68));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 4), Eq(73));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 5), Eq(43));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 6), Eq(65));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 7), Eq(37));
    EXPECT_THAT(*(int*)vector_get_element(&vec, 8), Eq(82));
}

TEST_F(NAME, resizing_larger_than_capacity_reallocates_and_updates_size)
{
    int* old_ptr;

    old_ptr = (int*)vector_emplace(&vec);
    *old_ptr = 42;
    vector_resize(&vec, CSTRUCTURES_VEC_MIN_CAPACITY * 1024 * 1024);
    int* new_ptr = (int*)vector_get_element(&vec, 0);
    EXPECT_THAT(old_ptr, Ne(new_ptr));
    EXPECT_THAT(*new_ptr, Eq(42));
    EXPECT_THAT(vec.capacity, Eq(CSTRUCTURES_VEC_MIN_CAPACITY * 1024 * 1024));
    EXPECT_THAT(vector_count(&vec), Eq(CSTRUCTURES_VEC_MIN_CAPACITY * 1024 * 1024));
}

TEST_F(NAME, resizing_smaller_than_capacity_updates_size_but_not_capacity)
{
    int* emplaced;
    emplaced = (int*)vector_emplace(&vec);
    vector_resize(&vec, 64);

    EXPECT_THAT(vec.capacity, Eq(64u));
    EXPECT_THAT(vector_count(&vec), Eq(64u));

    vector_resize(&vec, 8);

    EXPECT_THAT(vec.capacity, Eq(64u));
    EXPECT_THAT(vector_count(&vec), Eq(8u));
}

TEST_F(NAME, for_each_zero_elements)
{
    int counter = 0;
    VECTOR_FOR_EACH(&vec, int, value)
        counter++;
    VECTOR_END_EACH

    EXPECT_THAT(counter, Eq(0));
}

TEST_F(NAME, for_each_one_element)
{
    *(int*)vector_emplace(&vec) = 1;

    int counter = 0;
    VECTOR_FOR_EACH(&vec, int, value)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(1)));
    VECTOR_END_EACH

    EXPECT_THAT(counter, Eq(1));
}

TEST_F(NAME, for_each_three_elements)
{
    *(int*)vector_emplace(&vec) = 1;
    *(int*)vector_emplace(&vec) = 2;
    *(int*)vector_emplace(&vec) = 3;

    int counter = 0;
    VECTOR_FOR_EACH(&vec, int, value)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(counter)));
    VECTOR_END_EACH

    EXPECT_THAT(counter, Eq(3));
}

TEST_F(NAME, for_each_r_zero_elements)
{
    int counter = 0;
    VECTOR_FOR_EACH_R(&vec, int, value)
        counter++;
    VECTOR_END_EACH

    EXPECT_THAT(counter, Eq(0));
}

TEST_F(NAME, for_each_r_one_element)
{
    *(int*)vector_emplace(&vec) = 1;

    int counter = 0;
    VECTOR_FOR_EACH_R(&vec, int, value)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(1)));
    VECTOR_END_EACH

    EXPECT_THAT(counter, Eq(1));
}

TEST_F(NAME, for_each_r_three_elements)
{
    *(int*)vector_emplace(&vec) = 1;
    *(int*)vector_emplace(&vec) = 2;
    *(int*)vector_emplace(&vec) = 3;

    int counter = 0;
    VECTOR_FOR_EACH_R(&vec, int, value)
        EXPECT_THAT(value, Pointee(Eq(3-counter)));
        counter++;
    VECTOR_END_EACH

    EXPECT_THAT(counter, Eq(3));
}

TEST_F(NAME, for_each_range_zero_elements)
{
    int counter = 0;
    VECTOR_FOR_EACH_RANGE(&vec, int, value, 0, 0)
        counter++;
    VECTOR_END_EACH

    EXPECT_THAT(counter, Eq(0));
}

TEST_F(NAME, for_each_range_one_element)
{
    *(int*)vector_emplace(&vec) = 1;

    int counter = 0;
    VECTOR_FOR_EACH_RANGE(&vec, int, value, 0, 1)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(1)));
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(1));

    counter = 0;
    VECTOR_FOR_EACH_RANGE(&vec, int, value, 0, 0)
        counter++;
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(0));

    counter = 0;
    VECTOR_FOR_EACH_RANGE(&vec, int, value, 1, 1)
        counter++;
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(0));
}

TEST_F(NAME, for_each_range_five_elements)
{
    *(int*)vector_emplace(&vec) = 1;
    *(int*)vector_emplace(&vec) = 2;
    *(int*)vector_emplace(&vec) = 3;
    *(int*)vector_emplace(&vec) = 4;
    *(int*)vector_emplace(&vec) = 5;

    int counter = 0;
    VECTOR_FOR_EACH_RANGE(&vec, int, value, 0, 5)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(counter)));
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(5));

    counter = 0;
    VECTOR_FOR_EACH_RANGE(&vec, int, value, 0, 3)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(counter)));
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(3));

    counter = 0;
    VECTOR_FOR_EACH_RANGE(&vec, int, value, 2, 5)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(counter+2)));
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(3));
}

TEST_F(NAME, for_each_range_r_zero_elements)
{
    int counter = 0;
    VECTOR_FOR_EACH_RANGE_R(&vec, int, value, 0, 0)
        counter++;
    VECTOR_END_EACH

    EXPECT_THAT(counter, Eq(0));
}

TEST_F(NAME, for_each_range_r_one_element)
{
    *(int*)vector_emplace(&vec) = 1;

    int counter = 0;
    VECTOR_FOR_EACH_RANGE_R(&vec, int, value, 0, 1)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(1)));
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(1));

    counter = 0;
    VECTOR_FOR_EACH_RANGE_R(&vec, int, value, 0, 0)
        counter++;
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(0));

    counter = 0;
    VECTOR_FOR_EACH_RANGE_R(&vec, int, value, 1, 1)
        counter++;
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(0));
}

TEST_F(NAME, for_each_range_r_five_elements)
{
    *(int*)vector_emplace(&vec) = 1;
    *(int*)vector_emplace(&vec) = 2;
    *(int*)vector_emplace(&vec) = 3;
    *(int*)vector_emplace(&vec) = 4;
    *(int*)vector_emplace(&vec) = 5;

    int counter = 0;
    VECTOR_FOR_EACH_RANGE_R(&vec, int, value, 0, 5)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(6-counter)));
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(5));

    counter = 0;
    VECTOR_FOR_EACH_RANGE_R(&vec, int, value, 0, 3)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(4-counter)));
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(3));

    counter = 0;
    VECTOR_FOR_EACH_RANGE_R(&vec, int, value, 2, 5)
        counter++;
        EXPECT_THAT(value, Pointee(Eq(6-counter)));
    VECTOR_END_EACH
    EXPECT_THAT(counter, Eq(3));
}
