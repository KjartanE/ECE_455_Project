/*
 * task.h
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 *      Author: KjartanE
 */
#include "STM_32_RTOS_Config.h"

#ifndef TASK_H_
#define TASK_H_

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

typedef struct dd_task_list dd_task_list;

struct dd_task_list
{
    dd_task task;
    dd_task_list *next;
};

typedef enum
{
    CREATE,
    COMPLETE,
	GET_ACTIVE_LIST,
	GET_COMPLETED_LIST,
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

#endif /* TASK_H_ */
