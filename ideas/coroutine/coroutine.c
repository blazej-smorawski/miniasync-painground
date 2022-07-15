//
// Created by blazej-smorawski on 15.07.2022.
//
#include <libminiasync.h>

/*
 * ----==================----
 * Here's definition of timer_future
 */
struct subroutine_future_data {
};

struct subroutine_future_output {
};

FUTURE(subroutine_future, struct subroutine_future_data, struct subroutine_future_output);

static enum future_state
subroutine_future_implementation(struct future_context *ctx, struct future_notifier *notifier)
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
    int stage;
    union {
	struct subroutine_future subroutine;
    }exclusive_subroutines;
};

struct task_future_output {
};

FUTURE(task_future, struct task_future_data, struct task_future_output);

static enum future_state
caller_future_implementation(struct future_context *ctx, struct future_notifier *notifier)
{
	struct task_future_data *data = future_context_get_data(ctx);

	switch (data->stage) {
		case 0:
			printf("Stage 0\n");
			data->exclusive_subroutines.subroutine = subroutine();
			if (future_poll(&data->exclusive_subroutines.subroutine, NULL) == FUTURE_STATE_COMPLETE) {
				data->stage++;
				return FUTURE_STATE_RUNNING;
				// Poll itself?
			} else {
				return FUTURE_STATE_RUNNING;
			}
		case 1:
			printf("Stage 1\n");
			data->exclusive_subroutines.subroutine = subroutine();
			if (future_poll(&data->exclusive_subroutines.subroutine, NULL) == FUTURE_STATE_COMPLETE) {
				data->stage++;
				return FUTURE_STATE_RUNNING;
				// Poll itself?
			} else {
				return FUTURE_STATE_RUNNING;
			}
		case 2:
			printf("Task finished\n");
			return FUTURE_STATE_COMPLETE;
	}
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

int main(void){
	struct task_future new_task = task();
	do {
		printf("Polling...");
	}while(future_poll(&new_task, NULL) != FUTURE_STATE_COMPLETE);
	return 0;
}


