/*
 * main.c
 *
 * Task Scheduler Demo ECE 455 Lab
 * Kjartan Einarsson 	V00885049
 * Levi Bare 			V00965748
 */

#include "STM_32_RTOS_Config.h"


/*-----------------------------------------------------------*/

QueueHandle_t *messageRequestQueue, *messageResponseQueue;
uint32_t task1_period, task1_exec_time,
	task2_period, task2_exec_time,
	task3_period, task3_exec_time;

void get_test_params(void);
task_params *get_current_test_bench_params(void);

int main(void)
{
	get_test_params();

	messageRequestQueue = xQueueCreate(10, sizeof(dd_message));
	messageResponseQueue = xQueueCreate(10, sizeof(dd_message));

	if (messageRequestQueue == NULL || messageResponseQueue == NULL)
		return 0;

	xTaskCreate(SchedulerTask, "Scheduler", configMINIMAL_STACK_SIZE, NULL, SCHEDULER_TASK_PRIORITY, NULL);
	xTaskCreate(MonitorTask, "Monitor", configMINIMAL_STACK_SIZE, NULL, GENERATOR_TASK_PRIORITY, NULL);

	xTaskCreate(Task1Generator, "Task 1 Generator", configMINIMAL_STACK_SIZE, NULL, GENERATOR_TASK_PRIORITY, NULL);
	xTaskCreate(Task2Generator, "Task 2 Generator", configMINIMAL_STACK_SIZE, NULL, GENERATOR_TASK_PRIORITY, NULL);
	xTaskCreate(Task3Generator, "Task 3 Generator", configMINIMAL_STACK_SIZE, NULL, GENERATOR_TASK_PRIORITY, NULL);

	vTaskStartScheduler();

	return 0;
}

void get_test_params(void)
{
	task_params *test_params = get_current_test_bench_params();
	task1_period = test_params[0].period;
	task1_exec_time = test_params[0].exec_time;
	task2_period = test_params[1].period;
	task2_exec_time = test_params[1].exec_time;
	task3_period = test_params[2].period;
	task3_exec_time = test_params[2].exec_time;
}

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

task_params *get_current_test_bench_params(void)
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

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if (xFreeStackSpace > 100)
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
