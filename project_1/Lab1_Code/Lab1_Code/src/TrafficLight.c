#include "TrafficLight.h"


/*-----------------------------------------------------------*/
void Green_LED_Controller_Callback( xTimerHandle xTimer )
{
	printf("Green LED Callback. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN);

	if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE )
	{
		g_light_colour = 0;
		xSemaphoreGive( xMutexLight );
		printf("Green_LED_Controller_Callback: Updated light colour to yellow. \n");
	}else
	{
		printf("Green_LED_Controller_Callback: xMutexLight unavailable \n");
	}
	xTimerStart( xYellowLightSoftwareTimer, 0 );
}


/*-----------------------------------------------------------*/
void Yellow_LED_Controller_Callback( xTimerHandle xTimer )
{
	printf('Yellow LED Callback.');
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN);
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN);

	xTimerStart( xRedLightSoftwareTimer, 0 );
}


/*-----------------------------------------------------------*/
void Red_LED_Controller_Callback( xTimerHandle xTimer )
{
	printf('Red LED Callback.');
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN);
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);

	if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE )
	{
		g_light_colour = 1;
		xSemaphoreGive( xMutexLight );
		printf("Red_LED_Controller_Callback: Updated light colour to green. \n");
	}else
	{
		printf("Red_LED_Controller_Callback: xMutexLight unavailable \n");
	}
	xTimerStart( xGreenLightSoftwareTimer, 0 );
}


/*-----------------------------------------------------------*/
void TrafficLightTask(void *pvParameters)
{
	uint16_t new_speed = 7;
	uint16_t current_speed = 0;

	while(1)
		{
			// Update local flow/speed variable
			if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE )
		    {
				new_speed = g_flowrate;
				xSemaphoreGive( xMutexFlow );
				printf("LightTask: Accessed xMutexFlow, updated local flowrate:  %u.\n", new_speed );
		    }
			else
			{
				printf("LightTask: xMutexFlow unavailable \n");
			}


			if(current_speed !=  new_speed) // speed changed, changed timer
			{
				if(xTimerIsTimerActive( xGreenLightSoftwareTimer ))
				{
					xTimerStop(xGreenLightSoftwareTimer, 0);                                                                      // stop the green timer to change the period
					xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed))  / portTICK_PERIOD_MS, 0 );  // starts the green timer
					xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed)) / portTICK_PERIOD_MS, 0 );     // starts the red timer    (don't want it started)
					xTimerStop(xRedLightSoftwareTimer, 0);                                                                        // stop the red timer
				}
				else if(xTimerIsTimerActive( xYellowLightSoftwareTimer ))
				{
					xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed))  / portTICK_PERIOD_MS, 0 );  // starts the green timer (don't want it started)
					xTimerStop(xGreenLightSoftwareTimer, 0);                                                                      // stop the green timer
					xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed)) / portTICK_PERIOD_MS, 0 );     // starts the red timer    (don't want it started)
					xTimerStop(xRedLightSoftwareTimer, 0);                                                                        // stop the red timer
				}
				else if(xTimerIsTimerActive( xRedLightSoftwareTimer ))
				{
					xTimerStop(xRedLightSoftwareTimer, 0);																		  // stop the red timer to change the period
					xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed))  / portTICK_PERIOD_MS, 0 );  // starts the green timer (don't want it started)
					xTimerStop(xGreenLightSoftwareTimer, 0);																	  // stop the green timer since red is on
					xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed)) / portTICK_PERIOD_MS, 0 );     // starts the red timer again
				}
			} // end if(current_speed_value !=  new_speed_value)

			current_speed = new_speed; // update speed value

			vTaskDelay(500);
		}
	} // end Traffic_Light_Task
