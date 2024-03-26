/*
 * STMRTOSconfig.h
 *
 */

#ifndef STMRTOSCONFIG_H_
#define STMRTOSCONFIG_H_

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stm32f4_discovery.h"

/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

// Task priorities
#define SCHEDULER_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define GENERATOR_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define MONITOR_TASK_PRIORITY (tskIDLE_PRIORITY)

// Test Bench Parameters
extern uint32_t task1_period, task1_exec_time,
	task2_period, task2_exec_time,
	task3_period, task3_exec_time;

extern QueueHandle_t *messageRequestQueue;
extern QueueHandle_t *messageResponseQueue;

//TimerHandle_t task1Timer, task2Timer, task3Timer;

#define TEST_BENCH_1 1
#define TEST_BENCH_2 2
#define TEST_BENCH_3 3

#define CURRENT_TEST_BENCH TEST_BENCH_1

#endif /* STMRTOSCONFIG_H_ */
