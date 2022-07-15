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


#define SUBROUTINES(sub) int stage;\
union {\
struct sub ## _future sub;\
}exclusive_subroutines;\

#define COROUTINE switch (data->stage){ \
case 0:\

#define SUBROUTINE(sub) data->exclusive_subroutines.sub = sub();\
if (future_poll(&data->exclusive_subroutines.sub,\
NULL) == FUTURE_STATE_COMPLETE) {\
data->stage++;}\
return FUTURE_STATE_RUNNING;\
case LOCAL_COUNTER:\

#define END_COROUTINE } \
return FUTURE_STATE_COMPLETE;\
/*
 * ----==================----
 */

/*
 * ----==================----
 * Here's definition of subroutine_future
 */
struct subroutine_future_data {
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

	printf("Subroutine call\n");

	return FUTURE_STATE_COMPLETE;
}

/* Get timer_future */
static struct subroutine_future
subroutine(void)
{
	struct subroutine_future future = {};

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
			printf("Stage 0\n");
			SUBROUTINE(subroutine)
			printf("Stage 1\n");
			SUBROUTINE(subroutine)
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


