#include "freertos_count_semaphore.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"
#include "semphr.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Take_Task_Handle = NULL;
static TaskHandle_t Give_Task_Handle = NULL;

SemaphoreHandle_t CountSem_Handle = NULL;

static void Take_Task(void)
{
        uint32_t pinstate;
        BaseType_t xReturn = pdPASS;
        while(1){
                pinstate = KEY_Proc (0);
                if(pinstate == BTN2_PRES){
                        xReturn = xSemaphoreTake(CountSem_Handle, 0);
                        if(pdTRUE == xReturn)
                                u1_printf(" KEY2被按下 成功申请到停车位！\r\n");
                        else
                                u1_printf(" KEY2被按下 停车位已满！\r\n");
                }
                vTaskDelay(20);
        }

}

static void Give_Task(void)
{
        uint32_t pinstate;
        BaseType_t xReturn = pdPASS;
        while(1){
                pinstate = KEY_Proc (0);
                if(pinstate == BTN1_PRES){
                        xReturn = xSemaphoreGive(CountSem_Handle);
                        if(pdTRUE == xReturn)
                                u1_printf(" KEY1被按下 释放 1 个停车位！\r\n");
                        else
                                u1_printf(" KEY1被按下 无车位可以释放！\r\n");
                }
                vTaskDelay(20);
        }
}

static void AppTaskCreate(void)
{
        BaseType_t xReturn = pdPASS;

        taskENTER_CRITICAL();

        CountSem_Handle = xSemaphoreCreateCounting(5, 5);
        if(NULL != CountSem_Handle)
                u1_printf(" CountSem_Handle 计数信号量创建成功！\r\n");

        xReturn = xTaskCreate((TaskFunction_t)Take_Task,
                                (const char*)"Take_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)2,
                                (TaskHandle_t*)&Take_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 Take_Task 任务成功！\r\n");

        xReturn = xTaskCreate((TaskFunction_t)Give_Task,
                                (const char*)"Give_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)3,
                                (TaskHandle_t*)&Give_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 Give_Task 任务成功！\r\n");

        vTaskDelete(AppTaskCreate_Handle);
        taskEXIT_CRITICAL();

}

int rtos_start_count_semaphore(void)
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
