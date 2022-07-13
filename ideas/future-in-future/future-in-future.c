#include "libminiasync.h"

/*
 * ----==================----
 * Here's definition of callee_future
 */
struct callee_future_data {
    int number;
};

struct callee_future_output {
    int number;
};

FUTURE(callee_future, callee_future_data, callee_future_output);

static enum future_state
callee_future_implementation(struct future_context *ctx, struct future_notifier *notifier)
{
	struct callee_future_data *data = future_context_get_data(ctx);
	struct callee_future_output *output = future_context_get_output(ctx);

	int ret = printf("CALLEE SHOUTIEE\n");

	return FUTURE_STATE_COMPLETE;
}

/* It defines how to create 'async_print_fut' future */
static struct callee_future
callee(int number)
{
	struct callee_future future = {.output.number = 0};
	future.data.number = number;

	FUTURE_INIT(&future, callee_future_implementation);

	return future;
}
/*
 * ----==================----
 */

/*
 * ----==================----
 * Here's definition of caller_future
 */
struct caller_future_data {
    int number;
};

struct caller_future_output {
    int number;
};

FUTURE(caller_future, caller_future_data, caller_future_output);
/*
 * ----==================----
 */

int main(void) {
	printf("Go\n");
	return 0;
}