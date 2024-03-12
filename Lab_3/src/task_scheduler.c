#include "task_scheduler.h"
/*
 * scheduler.c
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 */

void SchedulerTask(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(1000);
    }
}


/****************************************************************************************
 * Receives all of the information necessary to create a new dd_task struct (excluding
 * the release time and completion time). The struct is packaged as a message and sent
 * to a queue for the DDS to receive.
 ****************************************************************************************/
void create_dd_task(TaskHandle_t handle, task_type type, uint32_t id, uint32_t absolute_deadline)
{

}


/****************************************************************************************
 * Receives the ID of the DD-Task which has completed its execution. The ID is packaged
 * as a message and sent to a queue for the DDS to receive.
 ****************************************************************************************/
void delete_dd_task(uint32_t id)
{

}


/****************************************************************************************
 * Sends a message to a queue requesting the Active Task List from the DDS. Once a
 * response is received from the DDS, the function returns the list.
 ****************************************************************************************/
**dd_task_list get_active_dd_task_list()
{

}


/****************************************************************************************
 * Sends a message to a queue requesting the Completed Task List from the DDS. Once
 * a response is received from the DDS, the function returns the list.
 ****************************************************************************************/
**dd_task_list get_completed_dd_task_list()
{

}

/****************************************************************************************
 * This function sends a message to a queue requesting the Overdue Task List from the DDS. 
 * Once a response is received from the DDS, the function returns the list.
 ****************************************************************************************/
**dd_task_list get_overdue_dd_task_list()
{

}