#include "freertos_task_management.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"
#include "queue.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LED_Task_Handle = NULL;
static TaskHandle_t KEY_Task_Handle = NULL;

static void LED_Task(void)
{
        while(1)
	{
		PINS_DRV_TogglePins(PTD, 1 << 0);
		vTaskDelay(800);
	}
}

static void KEY_Task(void)
{
        uint32_t pinstate;
        while(1)
	{
	        pinstate = KEY_Proc (0);
	        if(pinstate == BTN1_PRES)
	        {
                        u1_printf("挂起 LED 任务！\n");
                        vTaskSuspend(LED_Task_Handle);
	        }
                vTaskDelay(20);
	}
}

static void AppTaskCreate(void)
{
        BaseType_t xReturn = pdPASS;

        taskENTER_CRITICAL();

        xReturn = xTaskCreate((TaskFunction_t)LED_Task,
                                (const char*)"LED_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)2,
                                (TaskHandle_t*)&LED_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 LED_Task 任务成功！\r\n");

        xReturn = xTaskCreate((TaskFunction_t)KEY_Task,
                                (const char*)"KEY_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)3,
                                (TaskHandle_t*)&KEY_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 KEY_Task 任务成功！\r\n");

        vTaskDelete(AppTaskCreate_Handle);

        taskEXIT_CRITICAL();
}

int rtos_start_task_management(void)
{
        BaseType_t xReturn = pdPASS;
        xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,
                                (const char*)"AppTaskCreate",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)1,
                                (TaskHandle_t*)&AppTaskCreate_Handle);
        if(pdPASS == xReturn)
                vTaskStartScheduler();
        else
                return -1;
}
