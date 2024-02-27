/*
 * Traffic.c
 *
 */

#include "Traffic.h"

/* Reads in ADC value, converts to discrete 0-7 integer, adjusts traffic flow accordingly */
void TrafficFlowAdjustmentTask(void *pvParameters)
{
	uint16_t adc_value = 0, speed_adc_value = 0, current_speed_value = 0;
	uint16_t speed_change;

	while (1)
	{
		printf("Start Traffic Flow Adjustment Task\n");

		// Begin ADC
		ADC_SoftwareStartConv(ADC1);

		// Wait for conversion
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
			;
		adc_value = ADC_GetConversionValue(ADC1);

		// Convert ADC to 0-7
		speed_adc_value = adc_value / 512;
		if (speed_adc_value == 8)
			speed_adc_value = 7;

		// Calculate speed difference
		speed_change = abs(speed_adc_value - current_speed_value);

		// Update values if ADC value changes
		if (speed_change)
		{
			current_speed_value = speed_adc_value;

			// Grab Flow Semaphore
			if (xSemaphoreTake(xMutexFlow, (TickType_t)10) == pdTRUE)
			{
				g_flowrate = speed_adc_value;
				xSemaphoreGive(xMutexFlow);
				printf("Update Flow: %u\n", speed_adc_value);
			}
			else
				printf("xMutexFlow already held\n");
		}

		vTaskDelay(200);
	}
}

/* Using the flowrate from the flow task, spawn a new car in random intervals depending on current flow rate */
void TrafficCreatorTask(void *pvParameters)
{
	uint16_t car_value = 0, flowrate = 4;

	while (1)
	{
		printf("Start Traffic Creator Task\n");

		// Get Flow Semaphore
		if (xSemaphoreTake(xMutexFlow, (TickType_t)10) == pdTRUE)
		{
			flowrate = g_flowrate;
			xSemaphoreGive(xMutexFlow);
			printf("CreatorTask: Accessed xMutexFlow, updated flowrate:  %u. \n", flowrate);
		}
		else
			printf("xMutexFlow already held\n");

		float randomNum = 100.0 / (8.0 - flowrate);
		car_value = (rand() % 100) < randomNum;

		// Get Cars Semaphore
		if (xSemaphoreTake(xMutexCars, (TickType_t)10) == pdTRUE)
		{
			g_car_value = car_value;
			xSemaphoreGive(xMutexCars);
			printf("Update car: %u\n", car_value);
		}
		else
			printf("xMutexCars already held\n");

		vTaskDelay(500);
	}
}

/* Display necessary lights on road and traffic light */
void TrafficDisplayTask(void *pvParameters)
{
	uint16_t car_value = 0, light_colour = 1;
	uint16_t currentactiveprelighttraffic[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint16_t currentduringlighttraffic[3] = {0, 0, 0};
	uint16_t newactiveprelighttraffic[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	while (1)
	{
		// Get Cars Semaphore
		if (xSemaphoreTake(xMutexCars, (TickType_t)10) == pdTRUE)
		{
			car_value = g_car_value;
			xSemaphoreGive(xMutexCars);
		}
		else
			printf("xMutexCars already held\n");

		// Get Light Semaphore
		if (xSemaphoreTake(xMutexLight, (TickType_t)0) == pdTRUE)
		{
			light_colour = g_light_colour;
			xSemaphoreGive(xMutexLight);
			if (light_colour)
				printf("Updated light to Green");
			else
				printf("Updated light to Red");
		}
		else
			printf("xMutexLight already held\n");

		// Light is green
		if (light_colour)
		{
			// Spawn new car and move other cars
			ShiftRegisterValuePreLight(car_value);
			ShiftRegisterValueDuringLight(currentactiveprelighttraffic[7]);
			ShiftRegisterValuePostLight(currentduringlighttraffic[2]);

			// Shift cars
			newactiveprelighttraffic[0] = car_value;
			for (uint16_t i = 1; i != 8; i++)
				newactiveprelighttraffic[i] = currentactiveprelighttraffic[i - 1];

			currentduringlighttraffic[0] = currentactiveprelighttraffic[7];
			for (uint16_t i = 1; i != 3; i++)
				currentduringlighttraffic[i] = currentduringlighttraffic[i - 1];
		}
		// Light is red
		else
		{
			uint16_t encounteredzero = 0;

			// Find empty spots on road
			for (uint16_t i = 7; i != 0; i--)
			{
				if (currentactiveprelighttraffic[i] == 0)
				{
					encounteredzero = 1;
					newactiveprelighttraffic[0] = car_value;
				}

				// Shift cars if open spot
				if (encounteredzero == 1)
					newactiveprelighttraffic[i] = currentactiveprelighttraffic[i - 1];
				else
					newactiveprelighttraffic[i] = currentactiveprelighttraffic[i];
			}

			// Change lights
			for (int16_t i = 7; i >= 0; i--)
				ShiftRegisterValuePreLight(newactiveprelighttraffic[i]);

			ShiftRegisterValueDuringLight(0);
			currentduringlighttraffic[0] = 0;

			for (uint16_t i = 1; i != 3; i++)
				currentduringlighttraffic[i] = currentduringlighttraffic[i - 1];

			ShiftRegisterValuePostLight(currentduringlighttraffic[2]);
		}

		for (uint16_t i = 0; i != 8; i++)
			currentactiveprelighttraffic[i] = newactiveprelighttraffic[i];

		vTaskDelay(500);
	}
}

/* Update Register before intersection */
void ShiftRegisterValuePreLight(uint16_t value)
{
	if (value)
		GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);
	else
		GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);

	// Reset
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);
}

/* Update Register in intersection */
void ShiftRegisterValueDuringLight(uint16_t value)
{
	if (value)
		GPIO_SetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);
	else
		GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);

	// Reset
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);
}

/* Update Register after intersection */
void ShiftRegisterValuePostLight(uint16_t value)
{
	if (value)
		GPIO_SetBits(SHIFT_REG_3_PORT, SHIFT_REG_3_PIN);
	else
		GPIO_ResetBits(SHIFT_REG_3_PORT, SHIFT_REG_3_PIN);

	// Reset
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);
}
