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
#define SCHEDULER_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1 )
#define GENERATOR_TASK_PRIORITY 	( tskIDLE_PRIORITY )
#define MONITOR_TASK_PRIORITY       ( tskIDLE_PRIORITY )

#endif /* STMRTOSCONFIG_H_ */
