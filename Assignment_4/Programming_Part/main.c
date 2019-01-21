/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved
    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.
    This file is part of the FreeRTOS distribution.
    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.
    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html
    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************
    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?
    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.
    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.
    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.
    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.
    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.
    1 tab == 4 spaces!
*/

/*
 * main() creates all the demo application tasks, then starts the scheduler.
 * The web documentation provides more details of the standard demo application
 * tasks, which provide no particular functionality but do provide a good
 * example of how to use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Check" task - This only executes every five seconds but has a high priority
 * to ensure it gets processor time.  Its main function is to check that all the
 * standard demo tasks are still operational.  While no errors have been
 * discovered the check task will print out "OK" and the current simulated tick
 * time.  If an error is discovered in the execution of a task then the check
 * task will print out an appropriate error message.
 *
 */


/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define SIZE                            10
#define ROW                             SIZE
#define COL                             SIZE

/*
 * Prototypes for the standard FreeRTOS callback/hook functions implemented
 * within this file.
 */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );

static uint32_t MatrixTime = 0;
static uint32_t AperiodicResponse = 0;

static void MatrixTask(void *MatrixTaskParameters);
TaskHandle_t MatrixTaskHandle =NULL;

long lExpireCounters = 0;
TimerHandle_t Timer1Handle;
static void vTimerCallback(TimerHandle_t pxTimer);

static void aperiodic_task(void *AperiodicTaskParametres);
TaskHandle_t aperiodic_handle = NULL;


/*-----------------------------------------------------------*/

int main ( void )
{
    Timer1Handle = xTimerCreate("Timer1",   pdMS_TO_TICKS(5000) /*5000*/, pdTRUE , (void *) 0, vTimerCallback);
    if(Timer1Handle == NULL){
        printf("The timer handle was not created correctly \n");
        fflush(stdout);

    }
    else{
        if(xTimerStart(Timer1Handle,0) != pdPASS){
            printf("The timer handle was not started correctly \n");
            fflush(stdout);
        }
    }
    xTaskCreate( MatrixTask, "Matrix", 1000, NULL, 3, &MatrixTaskHandle);
	/* Start the scheduler itself. */
	vTaskStartScheduler();

	/* Should never get here unless there was not enough heap space to create
	the idle and other system tasks. */
	return 0;
}
/*-----------------------------------------------------------*/

static void MatrixTask(void *MatrixTaskParameters){
    ( void ) MatrixTaskParameters;
    int i;
    double **a = (double **)pvPortMalloc(ROW * sizeof(double*));
    for (i = 0; i < ROW; i++) a[i] = (double *)pvPortMalloc(COL * sizeof(double));
    double **b = (double **)pvPortMalloc(ROW * sizeof(double*));
    for (i = 0; i < ROW; i++) b[i] = (double *)pvPortMalloc(COL * sizeof(double));
    double **c = (double **)pvPortMalloc(ROW * sizeof(double*));
    for (i = 0; i < ROW; i++) c[i] = (double *)pvPortMalloc(COL * sizeof(double));

    double sum = 0.0;
    int j, k, l;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            a[i][j] = 1.5;
            b[i][j] = 2.6;
        }
    }

    while (1) {
        /*
        * In an embedded systems, matrix multiplication would block the CPU for a long time
        * but since this is a PC simulator we must add one additional dummy delay.
        */
        long simulationdelay;
        for (simulationdelay = 0; simulationdelay<1000000000; simulationdelay++)
            ;
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                c[i][j] = 0.0;
            }
        }

        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                sum = 0.0;
                for (k = 0; k < SIZE; k++) {
                    for (l = 0; l<10; l++) {
                        sum = sum + a[i][k] * b[k][j];
                    }
                }
                c[i][j] = sum;
            }
        }
        printf("Matrix Period is %d \n", MatrixTime);
        fflush(stdout);
        MatrixTime =0;
        vTaskDelay(100);
    }
}

static void vTimerCallback(TimerHandle_t pxTimer){
    printf("Timer callback!\n");
    fflush(stdout);
   AperiodicResponse =0;
    xTaskCreate((pdTASK_CODE)aperiodic_task,"Aperiodic", configMINIMAL_STACK_SIZE, NULL, 2, &aperiodic_handle);
//    long lArrayIndex;
    const long xMaxExpiryCountBeforeStopping = 10;
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT(pxTimer);
    /* Increment the number of times that pxTimer has expired. */
    lExpireCounters += 1;
    /* If the timer has expired 10 times then stop it from running. */
    if (lExpireCounters == xMaxExpiryCountBeforeStopping) {
        /* Do not use a block time if calling a timer API function from a
        timer callback function, as doing so could cause a deadlock! */
        xTimerStop(pxTimer, 0);
    }
}
static void aperiodic_task(void *AperiodicTaskParametres){
    ( void ) AperiodicTaskParametres;
    printf("Aperiodic task started!\n");
    fflush(stdout);
    long i;
    for (i = 0; i<1000000000; i++); //Dummy workload
    printf("Aperiodic task done!\n");
    printf("Aperiodic Response is %d \n", AperiodicResponse);
    fflush(stdout);
    vTaskDelete(aperiodic_handle);
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */

		/* Call the idle task processing used by the full demo.  The simple
		blinky demo does not use the idle task hook. */
		//vFullDemoIdleFunction();
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    AperiodicResponse++;
    MatrixTime++;
}
/*-----------------------------------------------------------*/

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
 	taskENTER_CRITICAL();
	{
        printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
        fflush(stdout);
	}
	taskEXIT_CRITICAL();
	exit(-1);
}
/*-----------------------------------------------------------*/
