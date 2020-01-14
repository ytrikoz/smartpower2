#include <unity.h>

#include <Core/CircularBuffer.h>

#define CAPACITY 1024
#define FIRST 100
#define COUNT 200

CircularBuffer<int, CAPACITY>* buffer;

void setUp(void) {
    buffer = new CircularBuffer<int, CAPACITY>();
}

void tearDown(void) {
    delete buffer;
}

void test_empty(void) {
    TEST_ASSERT_FALSE(buffer->empty());
    TEST_ASSERT_FALSE(buffer->full());
    TEST_ASSERT_EQUAL(COUNT, buffer->size());
}

void test_size(void) {
    for (size_t i = 0; i < COUNT; ++i)
        buffer->push(i + FIRST);
    TEST_ASSERT_EQUAL(COUNT, buffer->size());
}

void test_peek(void) {
    int item;

    TEST_ASSERT_TRUE(buffer->peek(item));
    TEST_ASSERT_EQUAL(FIRST, item);

    TEST_ASSERT_TRUE(buffer->peek(item));
    TEST_ASSERT_EQUAL(FIRST, item);
}

void test_push(void) {
    TEST_ASSERT_EQUAL(COUNT, buffer->size());

    for (size_t i = 0; i < CAPACITY; ++i)
        buffer->push(i);

    TEST_ASSERT_EQUAL(CAPACITY, buffer->size());
}

void test_pop(void) {
}

int main(int argc, char** argv) {
    UNITY_BEGIN();

    RUN_TEST(test_empty);
    RUN_TEST(test_push);
    RUN_TEST(test_peek);
    RUN_TEST(test_push);
    RUN_TEST(test_pop);

    UNITY_END();

    return 0;
}