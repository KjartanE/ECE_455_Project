/*
 * task_generator.c
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 *      Author: KjartanE
 */
#include "STM_32_RTOS_Config.h"
#include "task_generator.h"
#include "task_scheduler.h"


void Task1(void *pvParameters)
{
    TickType_t curr;
    TickType_t prev = 0;
    TickType_t time_remaining = task1_exec_time / portTICK_PERIOD_MS;

    while (time_remaining > 0)
    {
        curr = xTaskGetTickCount();
        time_remaining -= (curr == prev) ? 0 : curr - prev;
        prev = curr;
    }
    delete_dd_task(1);
    vTaskDelete(NULL);
}

void Task2(void *pvParameters)
{
    TickType_t curr;
    TickType_t prev = 0;
    TickType_t time_remaining = task2_exec_time / portTICK_PERIOD_MS;

    while (time_remaining > 0)
    {
        curr = xTaskGetTickCount();
        time_remaining -= (curr == prev) ? 0 : curr - prev;
        prev = curr;
    }
    delete_dd_task(2);
    vTaskDelete(NULL);
}

void Task3(void *pvParameters)
{
    TickType_t curr;
    TickType_t prev = 0;
    TickType_t time_remaining = task3_exec_time / portTICK_PERIOD_MS;

    while (time_remaining > 0)
    {
        curr = xTaskGetTickCount();
        time_remaining -= (curr == prev) ? 0 : curr - prev;
        prev = curr;
    }
    delete_dd_task(3);
    vTaskDelete(NULL);
}

void Task1Callback(TimerHandle_t pxTimer)
{
    TaskHandle_t task;
    xTaskCreate(Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, &task);

    if (task != NULL)
    {
        TickType_t curr = xTaskGetTickCount();
        create_dd_task(task, PERIODIC, 1, task1_period + curr);
        printf("Task 1 created\n");
//        vTaskDelay(task1_period);
    }
    // Restart timer
	xTimerChangePeriod(pxTimer, pdMS_TO_TICKS(task1_period), 0);
	xTimerStart(pxTimer, 0);
}

void Task2Callback(TimerHandle_t pxTimer)
{
    TaskHandle_t task;
    xTaskCreate(Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 2, &task);

    if (task != NULL)
    {
        TickType_t curr = xTaskGetTickCount();
        create_dd_task(task, PERIODIC, 2, task2_period + curr);
        printf("Task 2 created\n");
//        vTaskDelay(task2_period);
    }

    // Restart timer
	xTimerChangePeriod(pxTimer, pdMS_TO_TICKS(task2_period), 0);
	xTimerStart(pxTimer, 0);
}

void Task3Callback(TimerHandle_t pxTimer)
{
    TaskHandle_t task;
    xTaskCreate(Task3, "Task3", configMINIMAL_STACK_SIZE, NULL, 3, &task);

    if (task != NULL)
    {
        TickType_t curr = xTaskGetTickCount();
        create_dd_task(task, PERIODIC, 3, task3_period + curr);
        printf("Task 3 created\n");
//        vTaskDelay(task3_period);
    }

    // Restart timer
	xTimerChangePeriod(pxTimer, pdMS_TO_TICKS(task3_period), 0);
	xTimerStart(pxTimer, 0);
}
