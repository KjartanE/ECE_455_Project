/*
 * task.h
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
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

typedef struct
{
    dd_task task;
    struct dd_task_list *next;
} dd_task_list;

typedef enum
{
    CREATE,
    COMPLETE,
    GET_LIST,
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

const task_params test_bench_params[][3] = {
    {
        // Test Bench 1
        {95, 500},
        {150, 500},
        {250, 750},
    },
    {
        // Test Bench 2
        {95, 250},
        {150, 500},
        {250, 750},
    },
    {
        // Test Bench 3
        {100, 500},
        {200, 500},
        {200, 500},
    },
};

const task_params *get_current_test_bench_params()
{
    switch (CURRENT_TEST_BENCH)
    {
    case TEST_BENCH_1:
        return test_bench_params[0];
    case TEST_BENCH_2:
        return test_bench_params[1];
    case TEST_BENCH_3:
        return test_bench_params[2];
    default:
        return test_bench_params[0]; 
    }
}

#endif /* TASK_H_ */