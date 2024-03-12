#include "task_monitor.h"
/*
 * task_monitor.c
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 */

void MonitorTask(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(1000);
    }
}

