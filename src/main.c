/*
 * main.c
 */

#include "STM_32_RTOS_Config.h"
#include "TrafficLight.h"
#include "Traffic.h"

static void HardwareInit();

/*-----------------------------------------------------------*/

int main(void)
{
	HardwareInit();

	xMutexFlow = xSemaphoreCreateMutex();
	xMutexLight = xSemaphoreCreateMutex();
	xMutexCars = xSemaphoreCreateMutex();

	xQueueFlowRate = xQueueCreate(10, sizeof(uint16_t));
	xQueueLightColour = xQueueCreate(10, sizeof(uint16_t));
	xQueueCarValue = xQueueCreate(10, sizeof(uint16_t));

	// Give mutexes if available
	if (xMutexFlow)
		xSemaphoreGive(xMutexFlow);
	else
		printf("Error: Flow Semaphore Unsuccessful. \n");

	if (xMutexLight)
		xSemaphoreGive(xMutexLight);
	else
		printf("Error: Light Semaphore Unsuccessful. \n");

	if (xMutexCars)
		xSemaphoreGive(xMutexCars);
	else
		printf("Error: Cars Semaphore Unsuccessful. \n");

	// Create necessary tasks
	xTaskCreate(TrafficCreatorTask, "Creator", configMINIMAL_STACK_SIZE, NULL, TRAFFIC_CREATE_TASK_PRIORITY, NULL);
	xTaskCreate(TrafficLightTask, "Light", configMINIMAL_STACK_SIZE, NULL, TRAFFIC_LIGHT_TASK_PRIORITY, NULL);
	xTaskCreate(TrafficFlowAdjustmentTask, "FlowAdjust", configMINIMAL_STACK_SIZE, NULL, TRAFFIC_FLOW_TASK_PRIORITY, NULL);
	xTaskCreate(TrafficDisplayTask, "Display", configMINIMAL_STACK_SIZE, NULL, TRAFFIC_DISPLAY_TASK_PRIORITY, NULL);

	// Create traffic light timers
	xRedLightSoftwareTimer = xTimerCreate("RedLightTimer", 4000 / portTICK_PERIOD_MS, pdFALSE, (void *)0, Red_LED_Controller_Callback);
	xYellowLightSoftwareTimer = xTimerCreate("YellowLightTimer", 2000 / portTICK_PERIOD_MS, pdFALSE, (void *)0, Yellow_LED_Controller_Callback);
	xGreenLightSoftwareTimer = xTimerCreate("GreenLightTimer", 8000 / portTICK_PERIOD_MS, pdFALSE, (void *)0, Green_LED_Controller_Callback);

	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN); // Turn traffic light to green
	g_light_colour = 1;
	xTimerStart(xGreenLightSoftwareTimer, 0); // Start traffic light timer

	vTaskStartScheduler(); // Begin task scheduler

	return 0;
}

/* Setup and initialize GPIO and ADC hardware */
static void HardwareInit()
{
	NVIC_SetPriorityGrouping(0);

	// GPIO Initialization
	GPIO_InitTypeDef SHIFT_1_GPIO_InitStructure;
	GPIO_InitTypeDef SHIFT_2_GPIO_InitStructure;
	GPIO_InitTypeDef SHIFT_3_GPIO_InitStructure;
	GPIO_InitTypeDef TRAFFIC_GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// Initialize ADC
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef ADC_GPIO_InitStructure;

	// Output Pin
	SHIFT_1_GPIO_InitStructure.GPIO_Pin = SHIFT_REG_1_PIN | SHIFT_REG_CLK_1_PIN;
	SHIFT_2_GPIO_InitStructure.GPIO_Pin = SHIFT_REG_2_PIN | SHIFT_REG_CLK_2_PIN;
	SHIFT_3_GPIO_InitStructure.GPIO_Pin = SHIFT_REG_3_PIN | SHIFT_REG_CLK_3_PIN;
	TRAFFIC_GPIO_InitStructure.GPIO_Pin = TRAFFIC_LIGHT_RED_PIN | TRAFFIC_LIGHT_YELLOW_PIN | TRAFFIC_LIGHT_GREEN_PIN;
	ADC_GPIO_InitStructure.GPIO_Pin = ADC_PIN;

	// Output mode
	SHIFT_1_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	SHIFT_2_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	SHIFT_3_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	TRAFFIC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;

	// Push-pull mode
	SHIFT_1_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	SHIFT_2_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	SHIFT_3_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	TRAFFIC_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	// Disable pull ups and downs
	SHIFT_1_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	SHIFT_2_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	SHIFT_3_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	TRAFFIC_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	ADC_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	// GPIO Speed for shift registers
	SHIFT_1_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	// Initialize GPIO
	GPIO_Init(SHIFT_REG_1_PORT, &SHIFT_1_GPIO_InitStructure);
	GPIO_Init(SHIFT_REG_2_PORT, &SHIFT_2_GPIO_InitStructure);
	GPIO_Init(SHIFT_REG_3_PORT, &SHIFT_3_GPIO_InitStructure);
	GPIO_Init(TRAFFIC_LIGHT_PORT, &TRAFFIC_GPIO_InitStructure);
	GPIO_Init(GPIOC, &ADC_GPIO_InitStructure);

	// Initialize RCC
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// Initialize ADC1
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_84Cycles);
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
