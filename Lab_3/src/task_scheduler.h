/*
 * scheduler.h
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 */

#include "STM_32_RTOS_Config.h"
#ifndef SCHEDULER_H_
#define SCHEDULER_H_

void SchedulerTask(void *pvParameters);

void create_dd_task(TaskHandle_t handle, task_type type, uint32_t id, uint32_t absolute_deadline);
void delete_dd_task(uint32_t id);
dd_task_list *get_active_dd_task_list(void);
dd_task_list *get_completed_dd_task_list(void);
dd_task_list *get_overdue_dd_task_list(void);
void add_message_to_queue(message_type type, dd_task *task, uint32_t id, QueueHandle_t *queue);
void read_message_from_queue(QueueHandle_t *queue);
void add_dd_task_to_list(dd_task_list **list, dd_task *task);
void add_dd_task_to_list_sorted(dd_task_list **list, dd_task *task);
void remove_dd_task_from_list(dd_task_list **list, uint32_t taskId);
void check_overdue_tasks(void);
void remove_extra_overdue_tasks(void);

#endif /* SCHEDULER_H_ */
