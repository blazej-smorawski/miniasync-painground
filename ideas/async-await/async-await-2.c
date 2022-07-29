//
// Created by blazej-smorawski on 26.07.2022.
//
#include <libminiasync.h>

/*
 * ----==================----
 * Macros:
 */

#define get_rbp(p) \
  asm volatile("movq %%rbp, %0" : "=r"(p))
#define set_rbp(p) \
  asm volatile("movq %0, %%rbp" : : "r"(p))

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE(Unique_, __LINE__) TOKENPASTE2(Unique_, __LINE__)

#define STACK_SIZE 64

#define AWAIT_CALLS(sub) \
        void *stage; \
        union {\
                struct sub ## _future sub;\
        }exclusive_subroutines;           \
	\
	void *stack[STACK_SIZE];        \
	void *rbp;              \
	void *rip;               \
	void *shadow_space[4];  \

#define async_begin \
        void **stack_base_ptr;\
	get_rbp(stack_base_ptr);\
	struct task_future_data *data = future_context_get_data(ctx);\
	move(ctx);\
	move(notifier);\
	move(data);\
	move(stack_base_ptr);\
	set_rbp(&(((struct task_future_data*)future_context_get_data(ctx))->stack[STACK_SIZE])); \
	if(data->stage)\
            goto *data->stage;\

#define await(sub, call) \
                do{\
                    data->exclusive_subroutines.sub = call;\
                    UNIQUE(await,__LINE__): if (future_poll(FUTURE_AS_RUNNABLE(&data->exclusive_subroutines.sub), NULL) != FUTURE_STATE_COMPLETE) { data->stage = && UNIQUE(await,__LINE__); set_rbp(stack_base_ptr); return FUTURE_STATE_RUNNING;} \
                    }while(0)                              \

#define async_end \
        set_rbp(stack_base_ptr);
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

/* Get timer_future */
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
 * Here's definition of caller_future
 */
struct task_future_data {
	int counter;
	AWAIT_CALLS(subroutine);
};

struct task_future_output {
};

FUTURE(task_future,
	struct task_future_data, struct task_future_output);

#define move(var) data->stack[STACK_SIZE - (stack_base_ptr - (void**)&var)] = var

static enum future_state
task_future_implementation(struct future_context *ctx,
	struct future_notifier *notifier)
{
	async_begin
	printf("Halo! 5 btw....");
	int int_o_stack = 71;
	await(subroutine, subroutine(5));
	if (1 == 5) {
		printf("heh...");
		await(subroutine, subroutine(1));
	} else {
		printf("nah...");
		await(subroutine, subroutine(3));
		for (data->counter = 0; data->counter < 3; data->counter++) {
			printf("int_o_stack=%d...",int_o_stack);
			await(subroutine, subroutine(2));
		}
	}
	async_end
	return FUTURE_STATE_COMPLETE;
}

/* Get caller_future */
static struct task_future
task()
{
	struct task_future future = {
		.data.stage = NULL,
		.data.stack = {0xDE,0xAD,0xBE,0xEF},
	};

	FUTURE_INIT(&future, task_future_implementation);

	return future;
}

/*
 * ----==================----
 */

void rip_stack() {
	char rip[128];
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


