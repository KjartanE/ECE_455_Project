/*
 * task_generator.h
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 */
#include "STM_32_RTOS_Config.h"
#ifndef TASK_H_
#define TASK_H_

enum task_type {
    PERIODIC,
    APERIODIC
};

struct dd_task {
    TaskHandle_t handle;
    task_type type;
    uint32_t id;
    uint32_t release_time;
    uint32_t absolute_deadline;
    uint32_t completion_time;
};

struct dd_task_list {
    dd_task task;
    struct dd_task_list *next;
}


#endif /* TASK_H_ */