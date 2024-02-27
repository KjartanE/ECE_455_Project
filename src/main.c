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


	// Creating the mutex's used for accessing global variables shared between tasks.
    xMutexFlow = xSemaphoreCreateMutex();
    if( xMutexFlow == NULL )
    {
        printf("ERROR: FLOW SEMAPHORE NOT CREATED. \n"); 	/* There was insufficient FreeRTOS heap available for the semaphore to be created successfully. */
    }
    else
    {
    	xSemaphoreGive( xMutexFlow ); // need to give semaphore after it is defined
    }

    xMutexLight = xSemaphoreCreateMutex();
    if( xMutexLight == NULL )
    {
        printf("ERROR: LIGHT SEMAPHORE NOT CREATED. \n"); 	/* There was insufficient FreeRTOS heap available for the semaphore to be created successfully. */
    }
    else
    {
    	xSemaphoreGive( xMutexLight ); // need to give semaphore after it is defined
    }

	xMutexCars = xSemaphoreCreateMutex();
	if( xMutexCars == NULL )
	{
	   printf("ERROR: CARS SEMAPHORE NOT CREATED. \n"); 	/* There was insufficient FreeRTOS heap available for the semaphore to be created successfully. */
	}
	else
	{
	xSemaphoreGive( xMutexCars ); // need to give semaphore after it is defined
	}

	xTaskCreate( TrafficFlowAdjustmentTask , "FlowAdjust",configMINIMAL_STACK_SIZE ,NULL ,TRAFFIC_FLOW_TASK_PRIORITY,   NULL);
	xTaskCreate( TrafficCreatorTask        , "Creator"   ,configMINIMAL_STACK_SIZE ,NULL ,TRAFFIC_CREATE_TASK_PRIORITY, NULL);
	xTaskCreate( TrafficLightTask          , "Light"	 ,configMINIMAL_STACK_SIZE ,NULL ,TRAFFIC_LIGHT_TASK_PRIORITY,  NULL);
	xTaskCreate( TrafficDisplayTask        , "Display"   ,configMINIMAL_STACK_SIZE ,NULL ,TRAFFIC_DISPLAY_TASK_PRIORITY,NULL);

	xRedLightSoftwareTimer    = xTimerCreate("RedLightTimer"   ,   4000 / portTICK_PERIOD_MS  , pdFALSE, ( void * ) 0,	Red_LED_Controller_Callback);
	xYellowLightSoftwareTimer = xTimerCreate("YellowLightTimer",   2000 / portTICK_PERIOD_MS  , pdFALSE, ( void * ) 0,	Yellow_LED_Controller_Callback);
	xGreenLightSoftwareTimer  = xTimerCreate("GreenLightTimer" ,   8000 / portTICK_PERIOD_MS , pdFALSE, ( void * ) 0,	Green_LED_Controller_Callback);

	// Start the system with the light being green.
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);        // turn on the green light on the traffic light
	xTimerStart( xGreenLightSoftwareTimer, 0 );                       // start the green light timer
	g_light_colour = 1; 						                      // set light to green, don't care about semaphore since tasks havent been started.


	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

static void HardwareInit()
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	// GPIO Initialize
	GPIO_InitTypeDef      SHIFT_1_GPIO_InitStructure;
	GPIO_InitTypeDef      SHIFT_2_GPIO_InitStructure;
	GPIO_InitTypeDef      SHIFT_3_GPIO_InitStructure;
	GPIO_InitTypeDef      TRAFFIC_GPIO_InitStructure;

	// Enable all GPIO clocks for GPIO, reduce potential of missing one in future updates.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);


	SHIFT_1_GPIO_InitStructure.GPIO_Pin   = SHIFT_REG_1_PIN | SHIFT_REG_CLK_1_PIN;   // Shift register 1 output and clock set on same unique GPIO port.
	SHIFT_1_GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;                           // Set output mode
	SHIFT_1_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                           // Set push-pull mode
	SHIFT_1_GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                        // Disable pull-ups / pull-downs
	SHIFT_1_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;                        // Set higher speed to allow quick shifting refresh for shift register (Max for shift register itself is 25Mhz)
	GPIO_Init(SHIFT_REG_1_PORT, &SHIFT_1_GPIO_InitStructure);

	SHIFT_2_GPIO_InitStructure.GPIO_Pin   = SHIFT_REG_2_PIN | SHIFT_REG_CLK_1_PIN;   // Shift register 2 output and clock set on same unique GPIO port.
	SHIFT_2_GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;                           // Set output mode
	SHIFT_2_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                           // Set push-pull mode
	SHIFT_2_GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                        // Disable pull-ups / pull-downs
	GPIO_Init(SHIFT_REG_2_PORT, &SHIFT_2_GPIO_InitStructure);

	SHIFT_3_GPIO_InitStructure.GPIO_Pin   = SHIFT_REG_3_PIN | SHIFT_REG_CLK_3_PIN;   // Shift register 3 output and clock set on same unique GPIO port.
	SHIFT_3_GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;                           // Set output mode
	SHIFT_3_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                           // Set push-pull mode
	SHIFT_3_GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                        // Disable pull-ups / pull-downs
	GPIO_Init(SHIFT_REG_3_PORT, &SHIFT_3_GPIO_InitStructure);


	TRAFFIC_GPIO_InitStructure.GPIO_Pin   = TRAFFIC_LIGHT_RED_PIN | TRAFFIC_LIGHT_YELLOW_PIN | TRAFFIC_LIGHT_GREEN_PIN;    // Traffic light GPIO same unique GPIO port.
	TRAFFIC_GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;                                                                 // Set output mode
	TRAFFIC_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                                 // Set push-pull mode
	TRAFFIC_GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                                                              // Disable pull-ups / pull-downs
	GPIO_Init(TRAFFIC_LIGHT_PORT, &TRAFFIC_GPIO_InitStructure);

	//Init ADC
	ADC_InitTypeDef       ADC_InitStructure;
	GPIO_InitTypeDef      ADC_GPIO_InitStructure;

	// Enable GPIO and ADC clocks for ADC
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// Configure ADC1 Channel11 pin as analog input
	ADC_GPIO_InitStructure.GPIO_Pin  = ADC_PIN;                              // Using PC1, channel 11 of ADC
	ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                            // Set analog mode
	ADC_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                        // Set push-pull mode
	GPIO_Init(GPIOC, &ADC_GPIO_InitStructure);

	// ADC1 Init
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                      // Set ADC for 12 bit resolution (highest)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                          // Enable continuous scanning for ADC
	ADC_InitStructure.ADC_ExternalTrigConv = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;                                  // Perform a single conversion when start conversion is called
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_Cmd(ADC1, ENABLE );                                                     // Enable ADC1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11 , 1, ADC_SampleTime_84Cycles);
}




/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}


