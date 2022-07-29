//
// Created by blazej-smorawski on 26.07.2022.
//
#include <libminiasync.h>
#include <stdlib.h>
#include <string.h>

/*
 * ----==================----
 * Macros:
 */

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE(Unique_, __LINE__) TOKENPASTE2(Unique_, __LINE__)

enum {
	COUNTER_BASE = __COUNTER__
};

#define LOCAL_COUNTER (__COUNTER__ - COUNTER_BASE)


#define SUBROUTINES(sub) \
        void *stage;     \
	void *stack_copy;\
        union {\
                struct sub ## _future sub;\
        }exclusive_subroutines\

#define async \
	int stack_size = 0;\
	void *stack_start = 0;\
        if(data->stage)\
            goto *data->stage;\

#define STACK_SIZE &stack_start - UNIQUE(&stack_end,__LINE__)

#define await(sub, call) \
                do{\
                    data->exclusive_subroutines.sub = call;\
		    UNIQUE(void *stack_end,__LINE__) = 0; data->stack_copy = malloc(STACK_SIZE); memcpy(data->stack_copy, &UNIQUE(stack_end,__LINE__), STACK_SIZE); UNIQUE(await,__LINE__): if (future_poll(FUTURE_AS_RUNNABLE(&data->exclusive_subroutines.sub), NULL) != FUTURE_STATE_COMPLETE) { data->stage = && UNIQUE(await,__LINE__); return FUTURE_STATE_RUNNING;} memcpy(&UNIQUE(stack_end,__LINE__), data->stack_copy, STACK_SIZE);\
                    }while(0)\

/*
 * ----==================----
 */

/*
 * ----==================----
 * Here's definition of subroutine_future
 */
struct subroutine_future_data {
	int counter;
};

struct subroutine_future_output {
};

FUTURE(subroutine_future,
	struct subroutine_future_data, struct subroutine_future_output);

static enum future_state
subroutine_future_implementation(struct future_context *ctx,
	struct future_notifier *notifier)
{
	struct subroutine_future_data *data = future_context_get_data(ctx);

	while (data->counter != 0) {
		printf("Subroutine counter: %d\n", data->counter--);
		return FUTURE_STATE_RUNNING;
	}

	return FUTURE_STATE_COMPLETE;
}

static struct subroutine_future
subroutine(int counter)
{
	struct subroutine_future future = {.data.counter = counter};

	FUTURE_INIT(&future, subroutine_future_implementation);

	return future;
}
/*
 * ----==================----
 */

/*
 * ----==================----
 * Here's definition of task_future
 */
struct task_future_data {
	int counter;
	SUBROUTINES(subroutine);
};

struct task_future_output {
};

FUTURE(task_future,
	struct task_future_data, struct task_future_output);

static enum future_state
task_future_implementation(struct future_context *ctx,
	struct future_notifier *notifier)
{
	struct task_future_data *data = future_context_get_data(ctx);

	async
	printf("Heleloe! 5 btw....");
	int int_o_stack1 = 72;
	int int_o_stack2 = 73;
	int int_o_stack3 = 74;
	UNIQUE(void *stack_end,1) = 0;
	#define STACK_SIZE &stack_start - UNIQUE(&stack_end,1)
	data->stack_copy = malloc(STACK_SIZE);
	int n = STACK_SIZE;
	memcpy(data->stack_copy, &UNIQUE(stack_end,1), STACK_SIZE);
	memcpy(&UNIQUE(stack_end,1), data->stack_copy, STACK_SIZE);\
	await(subroutine, subroutine(5));
	int int_o_stack = 72;
	if (1 == 5) {
		printf("heh...");
		await(subroutine, subroutine(1));
	} else {
		printf("nah...");
		await(subroutine, subroutine(3));
		for (data->counter = 0; data->counter < 3; data->counter++) {
			printf("int_o_stack = %d...", int_o_stack);
			await(subroutine, subroutine(2));
		}
	}
	return FUTURE_STATE_COMPLETE;
}

static struct task_future
task()
{
	struct task_future future = {
		.data.stage = NULL,
	};

	FUTURE_INIT(&future, task_future_implementation);

	return future;
}

/*
 * ----==================----
 */

void rip_stack(void) {
	char ded[128] = {"DED"};
	ded[1] = 'D';
}

int main(void)
{
	struct task_future new_task = task();
	do {
		printf("Polling...");
		rip_stack();
	} while (future_poll(FUTURE_AS_RUNNABLE(&new_task), NULL) !=
		FUTURE_STATE_COMPLETE);
	return 0;
}


