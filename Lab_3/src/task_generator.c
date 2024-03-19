#include "task_generator.h"
#include "task.h"
#include "task_scheduler.h"
/*
 * task_generator.c
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 */

void Task1(void *pvParameters)
{
    TickType_t prev = 0;
    TickType_t time_remaining = task1_exec_time / portTICK_PERIOD_MS;

    while (time_remaining > 0)
    {
        curr = xTaskGetTickCount();
        time_remaining -= (curr == = prev) ? 0 : curr - prev;
        prev = curr;
    }
    delete_dd_task(1);
}

void Task2(void *pvParameters)
{
    TickType_t prev = 0;
    TickType_t time_remaining = task2_exec_time / portTICK_PERIOD_MS;

    while (time_remaining > 0)
    {
        curr = xTaskGetTickCount();
        time_remaining -= (curr == = prev) ? 0 : curr - prev;
        prev = curr;
    }
    delete_dd_task(2);
}

void Task3(void *pvParameters)
{
    TickType_t prev = 0;
    TickType_t time_remaining = task3_exec_time / portTICK_PERIOD_MS;

    while (time_remaining > 0)
    {
        curr = xTaskGetTickCount();
        time_remaining -= (curr == = prev) ? 0 : curr - prev;
        prev = curr;
    }
    delete_dd_task(3);
}

void Task1Generator(void *pvParameters)
{
    Task_handle_t task;
    xTaskCreate(Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, &task);

    if (task != NULL)
    {
        TickType_t curr = xTaskGetTickCount();
        create_dd_task(task, PERIODIC, 1, task1_period + curr);
        printf("Task 1 created\n");
        vTaskDelay(task1_period);
    }
}

void Task2Generator(void *pvParameters)
{
    Task_handle_t task;
    xTaskCreate(Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 2, &task);

    if (task != NULL)
    {
        TickType_t curr = xTaskGetTickCount();
        create_dd_task(task, PERIODIC, 2, task2_period + curr);
        printf("Task 2 created\n");
        vTaskDelay(task2_period);
    }
}

void Task3Generator(void *pvParameters)
{
    Task_handle_t task;
    xTaskCreate(Task3, "Task3", configMINIMAL_STACK_SIZE, NULL, 3, &task);

    if (task != NULL)
    {
        TickType_t curr = xTaskGetTickCount();
        create_dd_task(task, PERIODIC, 3, task3_period + curr);
        printf("Task 3 created\n");
        vTaskDelay(task3_period);
    }
}
