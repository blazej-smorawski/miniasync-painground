# future-in-future #
Simple test of a future using static future that is shared across other futures of the same type.
I used this for a group of futures that is running with shared timer.

```c
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
```

So, if the timer was not started yet a future starts it and then all the instances
will run for the time specified in the timer.