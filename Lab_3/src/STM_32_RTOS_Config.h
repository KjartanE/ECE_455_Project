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

typedef enum
{
	PERIODIC,
	APERIODIC
} task_type;

typedef struct
{
	TaskHandle_t handle;
	task_type type;
	uint32_t id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
} dd_task;

typedef struct
{
	dd_task task;
	struct dd_task_list *next;
} dd_task_list;

typedef enum
{
	CREATE,
	COMPLETE,
	GET_ACTIVE_LIST,
	GET_CURRENT_LIST,
	GET_OVERDUE_LIST
} message_type;

typedef struct
{
	message_type type;
	dd_task task;
	uint32_t id;
} dd_message;

typedef struct
{
	uint32_t exec_time;
	uint32_t period;
} task_params;

#define TEST_BENCH_1 1
#define TEST_BENCH_2 2
#define TEST_BENCH_3 3

#define CURRENT_TEST_BENCH TEST_BENCH_1

extern task_params test_bench_params[][3];
extern QueueHandle_t *messageRequestQueue;
extern QueueHandle_t *messageResponseQueue;

#endif /* STMRTOSCONFIG_H_ */
