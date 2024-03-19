#include "task_scheduler.h"
#include <stdint.h>
#include "task.h"
#include "STM_32_RTOS_Config.h"

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
    dd_task *new_task = (dd_task *)pvPortMalloc(sizeof(dd_task));
    if (newTask == NULL)
        return;
    new_task->handle = handle;
    new_task->type = type;
    new_task->id = id;
    new_task->release_time = 0;
    new_task->absolute_deadline = absolute_deadline;
    new_task->completion_time = 0;

    add_task_to_queue(new_task);
}

/****************************************************************************************
 * Adds a new task to the active task list. The task is inserted in order of absolute
 * deadlines.
 ****************************************************************************************/
void add_task_to_active_list(dd_task *new_task)
{
    dd_task_list *newList = (struct dd_task_list *)pvPortMalloc(sizeof(struct dd_task_list));
    if (newList == NULL)
        return;

    newList->task = *new_task;
    newList->next = NULL;

    // insert in order of absolute deadlines
    if (active_task_list == NULL || newTask->absolute_deadline < active_task_list->task.absolute_deadline)
    {
        newListNode->next = active_task_list;
        active_task_list = newListNode;
    }
    else
    {
        dd_task_list *curr = active_task_list;
        while (curr->next != NULL && curr->next->task.absolute_deadline <= newTask->absolute_deadline)
        {
            curr = curr->next;
        }

        newListNode->next = curr->next;
        curr->next = newListNode;
    }
}

/****************************************************************************************
 * Receives the ID of the DD-Task which has completed its execution. The ID is packaged
 * as a message and sent to a queue for the DDS to receive.
 ****************************************************************************************/
void delete_dd_task(uint32_t id){

};

/****************************************************************************************
 * Sends a message to a queue requesting the Active Task List from the DDS. Once a
 * response is received from the DDS, the function returns the list.
 ****************************************************************************************/
**dd_task_list get_active_dd_task_list(){

};

/****************************************************************************************
 * Sends a message to a queue requesting the Completed Task List from the DDS. Once
 * a response is received from the DDS, the function returns the list.
 ****************************************************************************************/
**dd_task_list get_completed_dd_task_list(){

};

/****************************************************************************************
 * This function sends a message to a queue requesting the Overdue Task List from the DDS.
 * Once a response is received from the DDS, the function returns the list.
 ****************************************************************************************/
**dd_task_list get_overdue_dd_task_list(){

};