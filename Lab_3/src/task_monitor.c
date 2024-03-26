#include "task_monitor.h"
#include "STM_32_RTOS_Config.h"
#include "task_scheduler.h"
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
        int activeCount = 0, completedCount = 0, overdueCount = 0;

        dd_task_list *activeList = get_active_dd_task_list();
        dd_task_list *completedList = get_completed_dd_task_list();
        dd_task_list *overdueList = get_overdue_dd_task_list();

        for (dd_task_list *list = activeList; list != NULL; list = list->next)
            activeCount++;
        for (dd_task_list *list = completedList; list != NULL; list = list->next)
            completedCount++;

        for (dd_task_list *list = overdueList; list != NULL; list = list->next)
            overdueCount++;

        printf("Active Tasks: %d, Completed Tasks: %d, Overdue Tasks: %d\n", activeCount, completedCount, overdueCount);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
