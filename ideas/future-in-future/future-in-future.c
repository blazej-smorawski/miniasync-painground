//
// Created by blazej-smorawski on 13.07.2022.
//
#include <libminiasync.h>
#include <time.h>
#include <timer.h>
#include <pthread.h>

/*
 * ----==================----
 * Here's definition of timer_future
 */
struct timer_future_data {
    double time;
    struct timespec start_time;
    pthread_mutex_t lock;
};

struct timer_future_output {
    int unused;
};

FUTURE(timer_future, struct timer_future_data, struct timer_future_output);

static enum future_state
timer_future_implementation(struct future_context *ctx,
	struct future_notifier *notifier)
{
	struct timer_future_data *data = future_context_get_data(ctx);
	struct timespec now_time;

	/*
	 * Probably unsafe without a mutex
	 */
	clock_gettime(CLOCK_REALTIME, &now_time);
	if (timediff(&now_time, &data->start_time) >= data->time) {
		printf("Timer done\n");
		return FUTURE_STATE_COMPLETE;
	} else {
		printf("Timer running...\n");
		return FUTURE_STATE_RUNNING;
	}
}

/* Get timer_future */
static struct timer_future
timer(int time)
{
	struct timer_future future = {
		.output.unused = 0,
		.data.time = time,
	};
	pthread_mutex_init(&future.data.lock, NULL);
	clock_gettime(CLOCK_REALTIME, &future.data.start_time);

	FUTURE_INIT(&future, timer_future_implementation);

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
    int unused;
};

FUTURE(caller_future, struct caller_future_data, struct caller_future_output);

static enum future_state
caller_future_implementation(struct future_context *ctx,
	struct future_notifier *notifier)
{
	struct caller_future_data *data = future_context_get_data(ctx);
	struct caller_future_output *output = future_context_get_output(ctx);

	static struct timer_future static_timer;

	/*
	 * Static future should be 0'ed, so if the first 64 bits are 0 we
	 * can guess it wasn't initialized yet
	 */
	if (*((long long *)(&static_timer)) == 0) {
		static_timer = timer(5);
		return FUTURE_STATE_RUNNING;
	} else {
		if (future_poll(&static_timer, NULL) == FUTURE_STATE_COMPLETE) {
			printf("[%d] Done\n", data->number);
			return FUTURE_STATE_COMPLETE;
		} else {
			printf("[%d] Running...\n", data->number);
			return FUTURE_STATE_RUNNING;
		}
	}
}

/* Get caller_future */
static struct caller_future
caller(int number)
{
	struct caller_future future = {
		.output.unused = 0,
		.data.number = number,
	};

	FUTURE_INIT(&future, caller_future_implementation);

	return future;
}

/*
 * ----==================----
 */

void *worker(void *arg)
{
	struct caller_future fut = caller((int)arg);
	FUTURE_BUSY_POLL(&fut);
	return NULL;
}

int main(void)
{
	int n = 4;
	pthread_t threads[n];

	for (long i = 0; i < n; i++) {
		pthread_create(&threads[i], NULL, worker, (void *)i);
	}

	for (long i = 0; i < n; i++) {
		pthread_join(threads[i], NULL);
	}

	/*struct runtime *r = runtime_new();
	struct caller_future callers[] = {
		caller(1),
		caller(2),
		caller(3),
		caller(4)
	};

	struct future* runnables[] = {
		FUTURE_AS_RUNNABLE(&callers[0]),
		FUTURE_AS_RUNNABLE(&callers[1]),
		FUTURE_AS_RUNNABLE(&callers[2]),
		FUTURE_AS_RUNNABLE(&callers[3]),
	};

	runtime_wait_multiple(r, runnables,4);*/
	return 0;
}