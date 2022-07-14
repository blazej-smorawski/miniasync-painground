//
// Created by blazej-smorawski on 14.07.2022.
//
#include "timer.h"

double timediff(const struct timespec *time1, const struct timespec *time0) {
	return (time1->tv_sec - time0->tv_sec)
	       + (time1->tv_nsec - time0->tv_nsec) / 1000000000.0;
}
