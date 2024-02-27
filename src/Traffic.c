/*
 * Traffic.c
 *
 */

#include "Traffic.h"

void TrafficFlowAdjustmentTask ( void *pvParameters )
{
    uint16_t adc_value = 0;            // Start with adc_value as 0, until it is read
    uint16_t speed_adc_value = 0;
    uint16_t current_speed_value = 0;
    uint16_t change_in_speed;

	while(1)
	{
		printf("FlowAdjustmentTask: Starting ADC conversion for new flowrate. \n ");

		ADC_SoftwareStartConv(ADC1);		            // Start conversion of ADC value
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	// Wait for ADC to finish conversion
		adc_value = ADC_GetConversionValue(ADC1);       // Grab ADC value

		speed_adc_value = adc_value/512;                // Take the adc_value (range between 0 to 4096) and divide by 512 to get 8 discrete steps
		if(speed_adc_value == 8)
		{
			speed_adc_value = 7;                        // As the speed value of 8 is only for a value of adc_value of 4096 exactly, set it to 7. Gives a range of 0-7
		}
        change_in_speed = abs(speed_adc_value - current_speed_value);   // Check if speed changed.

	    if(change_in_speed !=  0) 	// will only do queue stuff if the speed changed.
	    {
	    	current_speed_value = speed_adc_value; // save previous speed value for future task calls.

			if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
		    {
				g_flowrate = speed_adc_value;                               // Update global variable with new discrete speed step
				xSemaphoreGive( xMutexFlow );                               // Updated global flow variable, release mutex.
				printf("FlowAdjustmentTask: Accessed xMutexFlow, updated flowrate:  %u, (ADC Value: %u). \n", speed_adc_value, adc_value );
		    }
			else{
				printf("FlowTask: xMutexFlow unavailable \n");
			}

	    } // end if speed changed

        vTaskDelay(200);
	}
} // end Traffic_Flow_Adjustment_Task


/*  Traffic creator task: This task generates random traffic with a rate that is
	based on the potentiometer value reading. This value is received from the traffic
	flow adjustment task. The created traffic is sent to the task that displays the flow
	of the cars on the road.
 */
void TrafficCreatorTask ( void *pvParameters )
{
	uint16_t flowrate = 4;
	uint16_t car_value = 0;

	while(1)
	{
		printf("CreatorTask: Starting flowrate:  %u. \n", flowrate );
		if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
		{
			flowrate = g_flowrate;
			xSemaphoreGive( xMutexFlow );
			printf("CreatorTask: Accessed xMutexFlow, updated flowrate:  %u. \n", flowrate );
		}
		else
		{
			printf("CreatorTask: xMutexFlow unavailable \n");
		}


		/*
		 * compute the value for the display (0/1)
		 * received should be a value 1-8
		 * generate random number range[0:100]
		 * if the random number is below 100/(8 - value from traffic flow task) create a car
		 * if the value from traffic flow task is high, there is a higher probability of a car being created
		*/
		car_value = (rand() % 100 ) < 100/(8 - flowrate);

		printf("CreatorTask: car value updated to:  %u  \n", car_value);


		if( xSemaphoreTake( xMutexCars, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
		{
			g_car_value = car_value;
			xSemaphoreGive( xMutexCars );
			printf("CreatorTask: Accessed xMutexCars, updated g_car_value:  %u. \n", car_value );
		}
		else
		{
			printf("CreatorTask: xMutexCars unavailable \n");
		}

		vTaskDelay(500);
	}
} // end Traffic_Creator_Task


/*  Traffic light task: This task controls the timing of the traffic light. This timing is
	affected by the load of the traffic which is received from the traffic flow
	adjustment task.
*/
void TrafficDisplayTask ( void *pvParameters )
{
	//get value from traffic creator
	uint16_t car_value = 0;                            // start first car as no car
	uint16_t light_colour = 1;                         // start light as green
    uint16_t currentactiveprelighttraffic[8] = {0};    // 0 is newest element, 7 is at the traffic light
    uint16_t newactiveprelighttraffic[8] = {0};
    uint16_t currentduringlighttraffic[3] = {0};

	while(1)
	{
		// Update local car and light variables
		if( xSemaphoreTake( xMutexCars, ( TickType_t ) 10 ) == pdTRUE )       // get current car value
		{
			car_value = g_car_value;                                          // Update local variable with new car value (1 = car present, 0 = no car)
			xSemaphoreGive( xMutexCars );                                     // Return the semaphore as we are done reading the car value global variable
			printf("DisplayTask: Accessed xMutexCars, updated local car_value:  %u. \n", car_value );
		}
		else
		{
			printf("DisplayTask: xMutexCars unavailable \n");
		}

		if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE )       // can't wait for semaphore in callback, so ticks to wait is 0
	    {
			light_colour = g_light_colour;					                  // Update local variable with new light colour (1 = green, 0 = red)
			xSemaphoreGive( xMutexLight );                                    // Return the semaphore as we are done reading the light colour global variable
			printf("DisplayTask: Updated light colour: %u. (1 = green, 0 = red) \n", light_colour);
	    }
		else
		{
			printf("DisplayTask: xMutexLight unavailable \n");
		}


		if(light_colour == 1)		                                          // light is green, shift values normally
		{
			printf("DisplayTask: Light is green, shifting normally. \n ");

			ShiftRegisterValuePreLight(car_value);                            // Add the new car value on the road
			ShiftRegisterValueDuringLight(currentactiveprelighttraffic[7]);     // Shift the car passing through the light to past the light
			ShiftRegisterValuePostLight(currentduringlighttraffic[2]);

			newactiveprelighttraffic[0] = car_value;                          // Update the active car list with the new car value
			for (uint16_t i = 1; i != 8; i++)                                 // Shift the values from the current list to the active list
			{
				newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
			}

			currentduringlighttraffic[0] = currentactiveprelighttraffic[7];
			for (uint16_t i = 1; i != 3; i++)                                 // Shift the values from the current list to the active list
			{
				currentduringlighttraffic[i] = currentduringlighttraffic[i-1];
			}
		}
		else if(light_colour == 0)		                                               // light is red, perform full shift-register refresh
		{
			printf("DisplayTask: Light is red, doing fast shift. \n ");

			// need to account for new value, and not push off cars. Prepare data.
			uint16_t encounteredzero = 0;

			for (uint16_t i = 7; i != 0; i--)                                          // Search through the traffic list by decrementing, looking for the first 0
			{
	            if(currentactiveprelighttraffic[i] == 0)                               // Find a zero in the active traffic. If it exists, set the encounteredzero flag
	            {
	            	encounteredzero = 1;                                               // Set encounteredzero flag high
	            	newactiveprelighttraffic[0] = car_value;                           // A zero exists in the active traffic (traffic isn't full), so shifting new car on.
	            }

	            if(encounteredzero == 1)                                               // If zero is found, shift the remaining values normally.
	            {
	            	newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
	            }
	            else                                                                   // Zero not found yet, so no shifting occurs as cars don't move on a red light
	            {
	            	newactiveprelighttraffic[i] = currentactiveprelighttraffic[i];
	            }

			}// end data preparing loop


			for (int16_t i = 7; i >= 0 ; i--)                             // data is prepared, shift out new data (i = 7 is car closest to the light)
			{
				ShiftRegisterValuePreLight(newactiveprelighttraffic[i] );
			}

			// cycle cars during light
			ShiftRegisterValueDuringLight(0);
			currentduringlighttraffic[0] = 0;

			for (uint16_t i = 1; i != 3; i++)                                 // Shift the values from the current list to the active list
			{
				currentduringlighttraffic[i] = currentduringlighttraffic[i-1];
			}

			ShiftRegisterValuePostLight(currentduringlighttraffic[2]);                                // Ensure to move the cars after the traffic light, even though no new ones can pass.
		}


		// update current values for next iteration
		for(uint16_t i = 0; i != 8; i++)
		{
			currentactiveprelighttraffic[i] = newactiveprelighttraffic[i];
		}

		vTaskDelay(500);
	}
} // end Traffic_Display_Task

void ShiftRegisterValuePreLight( uint16_t value )
{
	if (value == 0)                                             // no car present
		GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);	    // set output low
	else                                                        // car on the road at this location
		GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);        // set output high
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);        // set clock high
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);      // set clock low again
}

void ShiftRegisterValueDuringLight( uint16_t value )
{
	if (value == 0)                                             // no car present
		GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);	    // set output low
	else                                                        // car on the road at this location
		GPIO_SetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);        // set output high
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);        // set clock high
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);      // set clock low again
}


void ShiftRegisterValuePostLight( uint16_t value )
{
	if (value == 0)                                             // no car present
		GPIO_ResetBits(SHIFT_REG_3_PORT, SHIFT_REG_3_PIN);	    // set output low
	else                                                        // car on the road at this location
		GPIO_SetBits(SHIFT_REG_3_PORT, SHIFT_REG_3_PIN);        // set output high
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);        // set clock high
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);      // set clock low again
}

