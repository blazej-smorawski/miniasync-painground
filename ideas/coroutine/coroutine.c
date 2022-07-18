//
// Created by blazej-smorawski on 15.07.2022.
//
#include <libminiasync.h>

/*
 * ----==================----
 * Macros:
 */

enum {
    COUNTER_BASE = __COUNTER__
};

#define LOCAL_COUNTER (__COUNTER__ - COUNTER_BASE)


#define SUBROUTINES(sub) \
        int stage; \
        char code_done;\
        union {\
                struct sub ## _future sub;\
        }exclusive_subroutines;\

#define COROUTINE \
        switch (data->stage){ \
                case 0:\
                if(!data->code_done) \
                {               \
                        do\

#define SUBROUTINE(sub, call) \
                        while(0);\
                        data->exclusive_subroutines.sub = call;\
                        data->code_done=1;   \
                }\
                if (future_poll(&data->exclusive_subroutines.sub,\
                        NULL) == FUTURE_STATE_COMPLETE) {\
                        data->stage++;\
                        data->code_done=0;\
                }\
                return FUTURE_STATE_RUNNING;\
                case LOCAL_COUNTER:\
                if(!data->code_done)\
                {\
                        do\

#define END_COROUTINE \
                        {}while(0);\
                } \
        }\
return FUTURE_STATE_COMPLETE; \

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
    SUBROUTINES(subroutine)
};

struct task_future_output {
};

FUTURE(task_future,
	struct task_future_data, struct task_future_output);

static enum future_state
caller_future_implementation(struct future_context *ctx,
	struct future_notifier *notifier)
{
	struct task_future_data *data = future_context_get_data(ctx);

	COROUTINE
	{
		printf("Stage 0\n");
		int x = 5;
	}
	SUBROUTINE(subroutine, subroutine(3))
	{
		printf("Stage 1\n");
	}
	SUBROUTINE(subroutine, subroutine(1))
	{
		printf("Stage 2\n");
	}
	SUBROUTINE(subroutine, subroutine(5))
	END_COROUTINE
}

/* Get caller_future */
static struct task_future
task()
{
	struct task_future future = {
		.data.stage = 0,
	};

	FUTURE_INIT(&future, caller_future_implementation);

	return future;
}

/*
 * ----==================----
 */

int main(void)
{
	struct task_future new_task = task();
	do {
		printf("Polling...");
	} while (future_poll(&new_task, NULL) != FUTURE_STATE_COMPLETE);
	return 0;
}


