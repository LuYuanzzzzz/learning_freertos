#include "freertos_demo1.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"
#include "queue.h"

void LED1_Task( void *pvParameters )
{
	(void) pvParameters;
	while(1)
	{
		PINS_DRV_TogglePins(PTD, 1 << 0);
		vTaskDelay(800);
	}
}


void LED2_Task( void *pvParameters )
{
	(void) pvParameters;
	while(1)
	{
		PINS_DRV_TogglePins(PTD, 1 << 1);
		vTaskDelay(400);
	}
}


void LED3_Task( void *pvParameters )
{
	(void) pvParameters;
	while(1)
	{
		PINS_DRV_TogglePins(PTD, 1 << 15);

		vTaskDelay(200);
	}
}


void LED4_Task( void *pvParameters )
{
	(void) pvParameters;
	while(1)
	{
		PINS_DRV_TogglePins(PTD, 1 << 16);
		vTaskDelay(100);
	}
}

TaskHandle_t TEST_task_handler;

/*创建一个任务并删除自身*/
void TEST_Task( void *pvParameters )
{
	(void) pvParameters;
	taskENTER_CRITICAL();
	u1_printf("test Task runnig & delete!\r\n");
	vTaskDelete(TEST_task_handler);
	taskEXIT_CRITICAL();
}


/*主循环*/
void main_Task( void *pvParameters )
{
	(void) pvParameters;
	uint32_t pinstate;
	while(1)
	{
	        pinstate = KEY_Proc (0);
	        if(pinstate ==BTN1_PRES )
	        {
                        xTaskCreate((TaskFunction_t)TEST_Task,
                                (char*)"TEST_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)1,
                                (TaskHandle_t*) TEST_task_handler);

	        }
		vTaskDelay(20);
	}
}

void rtos_start_demo1(void)
{
        xTaskCreate((TaskFunction_t)LED1_Task,
                (char*)"LED1_Task",
                (const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE ,
                (void *				 	) 			NULL,
                (UBaseType_t	  ) 				14,
                (TaskHandle_t *) 					NULL ) ;


	xTaskCreate(	(TaskFunction_t	) 					LED2_Task,
					(        char * ) 					"LED2_Task",
					(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE ,
					(void *				 	) 			NULL,
					(UBaseType_t	  ) 				14,
					(TaskHandle_t *) 					NULL ) ;


	xTaskCreate(	(TaskFunction_t	) 					LED3_Task,
					(        char * ) 					"LED3_Task",
					(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE ,
					(void *				 	) 			NULL,
					(UBaseType_t	  ) 				14,
					(TaskHandle_t *) 					NULL ) ;


	xTaskCreate(	(TaskFunction_t	) 					LED4_Task,
					(        char * ) 					"LED4_Task",
					(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE ,
					(void *				 	) 			NULL,
					(UBaseType_t	  ) 				14,
					(TaskHandle_t *) 					NULL ) ;


	xTaskCreate(	(TaskFunction_t	) 					main_Task,
					(        char * ) 					"main_Task",
					(const configSTACK_DEPTH_TYPE)	    configMINIMAL_STACK_SIZE ,
					(void *				 	) 			NULL,
					(UBaseType_t	  ) 				14,
					(TaskHandle_t *) 					NULL ) ;



	vTaskStartScheduler();
}
