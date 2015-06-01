/**
 * \file FreeRTOSConfig.h
 *
 * \brief FreeRTOS configuration
 * \copyright (C) 2014 Real Time Engineers Ltd.
 * \copyright (C) 2014 HES-SO Valais/Wallis
 *
 * \see http://www.freertos.org/a00110.html
 *
 * Full documentation is provided on the FreeRTOS website
 * (http://www.freertos.org/a00110.html).
 *
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

// Tracing
#define configUSE_TRACE_FACILITY				1

// Scheduler options
#define configUSE_PREEMPTION					1
#define configUSE_TIME_SLICING					1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configIDLE_SHOULD_YIELD					1
#define configMAX_PRIORITIES					16

// Stack
#define configMINIMAL_STACK_SIZE				256

// Clocking
#define configCPU_CLOCK_HZ						(168*1000*1000)
#define configTICK_RATE_HZ						1000
#define configUSE_16_BIT_TICKS					0
#define configUSE_TICKLESS_IDLE					0
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP	2
#define configPRE_SLEEP_PROCESSING				0
#define configPOST_SLEEP_PROCESSING				0
#define configUSE_IDLE_HOOK						0
#define configUSE_TICK_HOOK						0

// Locking
#define configUSE_MUTEXES						1
#define configUSE_QUEUE_SETS					1
#define configUSE_RECURSIVE_MUTEXES				1
#define configUSE_COUNTING_SEMAPHORES			1

// API options
#define configUSE_ALTERNATIVE_API				0
#define configENABLE_BACKWARD_COMPATIBILITY		0

// Development helpers
#define configCHECK_FOR_STACK_OVERFLOW			2
#define configUSE_MALLOC_FAILED_HOOK			1
#define configMAX_TASK_NAME_LEN					16
#define configQUEUE_REGISTRY_SIZE				8
#define configUSE_APPLICATION_TASK_TAG			0
#define configGENERATE_RUN_TIME_STATS			0
#define configUSE_STATS_FORMATTING_FUNCTIONS	1

// Co-routine
#define configUSE_CO_ROUTINES 					0
#define configMAX_CO_ROUTINE_PRIORITIES 		2

// Software timer
#define configUSE_TIMERS						1
#define configTIMER_TASK_PRIORITY				2
#define configTIMER_QUEUE_LENGTH				10
#define configTIMER_TASK_STACK_DEPTH			( configMINIMAL_STACK_SIZE * 2 )

// Miscellaneous
#define configUSE_NEWLIB_REENTRANT				0
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS	0

// Optional functions
#define INCLUDE_vTaskDelay						1
#define INCLUDE_vTaskPrioritySet				1
#define INCLUDE_uxTaskPriorityGet				1
#define INCLUDE_vTaskDelete						1
#define INCLUDE_xTaskResumeFromISR				1
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_vTaskDelayUntil					1
#define INCLUDE_vTaskCleanUpResources			1
#define INCLUDE_xTaskGetCurrentTaskHandle		1
#define INCLUDE_pcTaskGetTaskName				1
#define INCLUDE_eTaskGetState					1
#define INCLUDE_xTaskGetSchedulerState			1
#define INCLUDE_uxTaskGetStackHighWaterMark		1
#define INCLUDE_xTaskGetIdleTaskHandle			1
#define INCLUDE_xTimerPendFunctionCall			1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle	1
#define INCLUDE_xEventGroupSetBitFromISR		1
#define INCLUDE_xSemaphoreGetMutexHolder		1
#define INCLUDE_xQueueGetMutexHolder			1

// FreeRTOS assert
#include "heivs/bsp.h"
#include "heivs/error.h"
#define configASSERT( x ) if( ( x ) == 0 ) { bsp_fatal(ERROR_CONFIG); }

// Cortex M definitions
#define configPRIO_BITS       				__NVIC_PRIO_BITS

/**
 * The stm32 library use priorities from 0 (higher) to (2^configPRIO_BITS)-1
 * (lower).
 *
 * The interrupts are configured using NVIC_Init(), the priority is set in the
 * NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority field.
 *
 * Interrupts with priority higher (numerically smaller) than
 * configKERNEL_INTERRUPT_PRIORITY can't use kernel functions.
 *
 * In any case, only kernel function ending with FromISR can be called.
 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			((1 << configPRIO_BITS) - 1)
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5
#define configKERNEL_INTERRUPT_PRIORITY 				( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 			( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

// Map FreeRTOS handlers to standard CMSIS name
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
