/*
 * scheduler.h
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 */

#include <task.h>
#ifndef SCHEDULER_H_
#define SCHEDULER_H_

void SchedulerTask(void *pvParameters);

void create_dd_task(TaskHandle_t handle, task_type type, uint32_t id, uint32_t absolute_deadline);
void delete_dd_task(uint32_t id);
**dd_task_list get_active_dd_task_list(void);
**dd_task_list get_completed_dd_task_list(void);
**dd_task_list get_overdue_dd_task_list(void);

#endif /* SCHEDULER_H_ */
