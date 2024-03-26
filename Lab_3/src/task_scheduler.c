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

static dd_task_list *active_task_list = NULL;
static dd_task_list *completed_task_list = NULL;
static dd_task_list *overdue_task_list = NULL;

void SchedulerTask(void *pvParameters)
{
    active_task_list = NULL;
    completed_task_list = NULL;
    overdue_task_list = NULL;

    read_message_from_queue(messageRequestQueue);
}

/****************************************************************************************
 * Receives all of the information necessary to create a new dd_task struct (excluding
 * the release time and completion time). The struct is packaged as a message and sent
 * to a queue for the DDS to receive.
 ****************************************************************************************/
void create_dd_task(TaskHandle_t handle, task_type type, uint32_t id, uint32_t absolute_deadline)
{
    vTaskSuspend(handle);
    dd_task *new_task = (dd_task *)pvPortMalloc(sizeof(dd_task));
    if (new_task == NULL)
        return;
    new_task->handle = handle;
    new_task->type = type;
    new_task->id = id;
    new_task->release_time = 0;
    new_task->absolute_deadline = absolute_deadline;
    new_task->completion_time = 0;

    add_message_to_queue(CREATE, new_task, 0, messageRequestQueue);
}

/****************************************************************************************
 * Receives the ID of the DD-Task which has completed its execution. The ID is packaged
 * as a message and sent to a queue for the DDS to receive.
 ****************************************************************************************/
void delete_dd_task(uint32_t id)
{
    dd_task_list *curr = active_task_list;
    TaskHandle_t taskHandle = NULL;

    while (curr != NULL)
    {
        if (curr->task.id == id)
        {
            taskHandle = curr->task.handle;
            break;
        }
        curr = curr->next;
    }

    if (taskHandle != NULL)
    {
        add_message_to_queue(COMPLETE, NULL, id, messageRequestQueue);

        uint32_t deletedTaskId;
        if (xQueueReceive(messageResponseQueue, &deletedTaskId, portMAX_DELAY) == pdPASS)
        {
            if (deletedTaskId == id)
            {
                vTaskDelete(taskHandle);
            }
        }
    }
}

/****************************************************************************************
 * Sends a message to a queue requesting the Active Task List from the DDS. Once a
 * response is received from the DDS, the function returns the list.
 ****************************************************************************************/
dd_task_list *get_active_dd_task_list()
{
    dd_task_list *list;
    add_message_to_queue(GET_ACTIVE_LIST, &active_task_list->task, 0, messageRequestQueue);

    if (xQueueReceive(messageResponseQueue, &list, portMAX_DELAY) == pdPASS)
    {
        return list;
    }

    return NULL;
}

/****************************************************************************************
 * Sends a message to a queue requesting the Completed Task List from the DDS. Once
 * a response is received from the DDS, the function returns the list.
 ****************************************************************************************/
dd_task_list *get_completed_dd_task_list()
{
    dd_task_list *list;
    add_message_to_queue(GET_COMPLETED_LIST, &completed_task_list->task, 0, messageRequestQueue);

    if (xQueueReceive(messageResponseQueue, &list, portMAX_DELAY) == pdPASS)
    {
        return list;
    }

    return NULL;
}

/****************************************************************************************
 * This function sends a message to a queue requesting the Overdue Task List from the DDS.
 * Once a response is received from the DDS, the function returns the list.
 ****************************************************************************************/
dd_task_list *get_overdue_dd_task_list()
{
    dd_task_list *list;
    add_message_to_queue(GET_OVERDUE_LIST, &overdue_task_list->task, 0, messageRequestQueue);

    if (xQueueReceive(messageResponseQueue, &list, portMAX_DELAY) == pdPASS)
    {
        return list;
    }

    return NULL;
}

void add_message_to_queue(message_type type, dd_task *task, uint32_t id, QueueHandle_t *queue)
{
    dd_message *message = (dd_message *)pvPortMalloc(sizeof(dd_message));
    if (message == NULL)
        return;
    message->type = type;
    message->task = *task;
    message->id = id;
    xQueueSend(queue, message, 0);
}

void read_message_from_queue(QueueHandle_t *queue)
{
    dd_message receivedMessage;
    dd_task_list list_to_send;
    BaseType_t queueStatus;

    while (1)
    {
        check_overdue_tasks();

        queueStatus = xQueueReceive(queue, &receivedMessage, portMAX_DELAY);

        if (queueStatus)
        {
            switch (receivedMessage.type)
            {
            case CREATE:
            {
                receivedMessage.task.release_time = xTaskGetTickCount();
                add_dd_task_to_list_sorted(&active_task_list, &receivedMessage.task);
                vTaskResume(receivedMessage.task.handle);
                break;
            }
            case COMPLETE:
            {
                remove_dd_task_from_list(&active_task_list, receivedMessage.id);
                add_dd_task_to_list(&completed_task_list, &receivedMessage.task);
                break;
            }
            case GET_ACTIVE_LIST:
            {
                list_to_send = *active_task_list;
            }
            case GET_COMPLETED_LIST:
            {
                list_to_send = *completed_task_list;
            }
            case GET_OVERDUE_LIST:
            {
                list_to_send = *overdue_task_list;
            }
            }
            xQueueSend(messageResponseQueue, &list_to_send, portMAX_DELAY);
            break;
        }
    }
}

void add_dd_task_to_list(dd_task_list **list, dd_task *task)
{
    dd_task_list *newNode = (dd_task_list *)pvPortMalloc(sizeof(dd_task_list));
    if (newNode == NULL)
    {
        return;
    }

    newNode->task = *task;
    newNode->next = *list;
    *list = newNode;
}

void add_dd_task_to_list_sorted(dd_task_list **list, dd_task *task)
{
    dd_task_list *newNode = (dd_task_list *)pvPortMalloc(sizeof(dd_task_list));
    if (newNode == NULL)
    {
        return;
    }
    newNode->task = *task;

    dd_task_list *current = *list, *previous = NULL;
    while (current != NULL && current->task.absolute_deadline <= task->absolute_deadline)
    {
        previous = current;
        current = current->next;
    }

    if (previous == NULL)
    {
        newNode->next = *list;
        *list = newNode;
    }
    else
    {
        newNode->next = previous->next;
        previous->next = newNode;
    }
}

void remove_dd_task_from_list(dd_task_list **list, uint32_t taskId)
{
    dd_task_list *curr = *list;
    dd_task_list *prev = NULL;

    while (curr != NULL)
    {
        if (curr->task.id == taskId)
        {
            if (prev == NULL)
            {
                *list = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }
            vPortFree(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

void check_overdue_tasks()
{
    uint32_t curr_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    dd_task_list *prev = NULL;
    dd_task_list *curr_task = active_task_list;

    while (curr_task != NULL)
    {
        if (curr_time < curr_task->task.absolute_deadline)
            break; // Remaining tasks are not overdue

        dd_task_list *overdue_task = curr_task;
        curr_task = curr_task->next;

        // Remove overdue task from active
        if (prev)
            prev->next = curr_task;
        else
            active_task_list = curr_task;

        // Remove FreeRTOS task
        vTaskSuspend(overdue_task->task.handle);
        vTaskDelete(overdue_task->task.handle);

        // Add overdue task to list
        overdue_task->next = NULL;
        add_dd_task_to_list(&overdue_task_list, &overdue_task->task);
    }

    remove_extra_overdue_tasks();
}

void remove_extra_overdue_tasks()
{
    const int max_overdue_size = 5;
    int count = 0;
    dd_task_list *current = overdue_task_list;
    dd_task_list *prev = NULL;

    while (current != NULL && count < max_overdue_size)
    {
        prev = current;
        current = current->next;
        count++;
    }

    // Release extra tasks
    while (current != NULL)
    {
        dd_task_list *to_free = current;
        current = current->next;
        if (prev)
        {
            prev->next = current;
        }
        vPortFree(to_free);
    }
}
