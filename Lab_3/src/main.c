/*
 * main.c
 *
 * Task Scheduler Demo ECE 455 Lab
 * Kjartan Einarsson 	V00885049
 * Levi Bare 			V00965748
 */

#include "STM_32_RTOS_Config.h"
#include "task_scheduler.h"
#include "task_monitor.h"
#include "task_generator.h"

/*-----------------------------------------------------------*/

int main(void)
{
	xTaskCreate(SchedulerTask, "Scheduler", configMINIMAL_STACK_SIZE, NULL, SCHEDULER_TASK_PRIORITY, NULL);
	xTaskCreate(MonitorTask, "Monitor", configMINIMAL_STACK_SIZE, NULL, GENERATOR_TASK_PRIORITY, NULL);
	xTaskCreate(GeneratorTask, "Generator", configMINIMAL_STACK_SIZE, NULL, MONITOR_TASK_PRIORITY, NULL);

	vTaskStartScheduler();

	return 0;
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
