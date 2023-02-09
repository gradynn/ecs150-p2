#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>
//#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Retrieve length */
void test_queue_length(void)
{
    int data = 3, *ptr;
    queue_t q;

    fprintf(stderr, "*** TEST queue_length ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    queue_enqueue(q, &data);
    TEST_ASSERT(queue_length(q) == 2);
    queue_dequeue(q, (void**)&ptr);   
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(queue_length(q) == 0);
}

/* Delete an emtpy queue */
void test_queue_destroy_empty(void)
{
    queue_t q;

    fprintf(stderr, "*** TEST queue_delete_empty ***\n");

    q = queue_create();
    TEST_ASSERT(queue_destroy(q) == 0);
}

/* Delete a non-empty queue */
void test_queue_destroy_nonempty(void)
{
    int data = 3;
    queue_t q;

    fprintf(stderr, "*** TEST queue_delete_nonempty ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    TEST_ASSERT(queue_destroy(q) == -1);
}

/* Delete a NULL queue */
void test_queue_destroy_null(void)
{
    fprintf(stderr, "*** TEST queue_delete_null ***\n");

    TEST_ASSERT(queue_destroy(NULL) == -1);
}

/* Delete value within queue */
void test_queue_delete_value(void)
{
    int data = 3;
    queue_t q;

    fprintf(stderr, "*** TEST queue_delete_value ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    TEST_ASSERT(queue_delete(q, &data) == 0);
}

/* Iterate over queue with function */
static void iterator_inc(queue_t q, void *data)
    {
        int *a = (int*)data;

        if (*a == 42)
            queue_delete(q, (void*)data);
        else
            *a += 1;
    } 

void test_queue_iterate(void)
{
    int data = 3;
    queue_t q;

    fprintf(stderr, "*** TEST queue_iterate ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    queue_enqueue(q, &data);
    queue_enqueue(q, &data);
    TEST_ASSERT(queue_iterate(q, iterator_inc) == 0);
}

int main(void)
{
	test_create();
	test_queue_simple();
    test_queue_length();
    test_queue_destroy_empty();
    test_queue_destroy_nonempty();
    test_queue_destroy_null();
    test_queue_delete_value();
    test_queue_iterate();

	return 0;
}
