/*
 * task_generator.h
 *
 *  Created on: Mar 12, 2024
 *      Author: lbare
 */

#include "task.h"

#ifndef TASK_GENERATOR_H_
#define TASK_GENERATOR_H_

// Task Priorities
void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);

// Task Callbacks
void Task1Callback(void *pvParameters);
void Task2Callback(void *pvParameters);
void Task3Callback(void *pvParameters);

#endif /* TASK_GENERATOR_H_ */
