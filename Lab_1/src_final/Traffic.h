/*
 * Traffic.h
 *
 */

#ifndef TRAFFIC_H_
#define TRAFFIC_H_

#include "STM_32_RTOS_Config.h"

void TrafficDisplayTask( void *pvParameters );
void TrafficFlowAdjustmentTask( void *pvParameters );
void TrafficCreatorTask( void *pvParameters );

// Helper function declarations
void ShiftRegisterValuePreLight( uint16_t value );
void ShiftRegisterValuePostLight( uint16_t value );
void ShiftRegisterValueDuringLight( uint16_t value );
#endif /* TRAFFIC_H_ */

