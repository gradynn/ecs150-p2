#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

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
	queue_t queue;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	queue = queue_create();
	queue_enqueue(queue, &data);
	queue_dequeue(queue, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

int main(void)
{
	test_create();
	test_queue_simple();

	return 0;
}
