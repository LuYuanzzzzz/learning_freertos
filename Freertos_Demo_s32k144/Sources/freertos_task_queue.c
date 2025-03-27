#include "freertos_task_queue.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"
#include "queue.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Receive_Task_Handle = NULL;
static TaskHandle_t Send_Task_Handle = NULL;

QueueHandle_t Test_Queue = NULL;

#define QUEUE_LEN 4
#define QUEUE_SIZE 4

static void Receive_Task(void)
{
        BaseType_t xReturn = pdTRUE;
        uint32_t r_queue;
        while(1){
                xReturn = xQueueReceive(Test_Queue, &r_queue, portMAX_DELAY);
                if(pdTRUE == xReturn)
                        u1_printf("本次接收到的数据是 %d\r\n", r_queue);
                else
                        u1_printf("数据接收出错，错误代码：0x%1x\r\n", xReturn);
        }
}

static void Send_Task(void)
{
        BaseType_t xReturn = pdPASS;
        uint32_t send_data = 1;
        uint32_t pinstate;
        while(1){
                pinstate = KEY_Proc (0);
                if(pinstate == BTN1_PRES){
                        u1_printf("发送消息 send_data!\r\n");
                        xReturn = xQueueSend(Test_Queue, &send_data, 0);
                        if(pdPASS == xReturn)
                                u1_printf("消息 send_data 发送成功!\r\n");
                }
                vTaskDelay(20);
        }

}

static void AppTaskCreate(void)
{
        BaseType_t xReturn = pdPASS;

        taskENTER_CRITICAL();

        Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,
                                (UBaseType_t)QUEUE_SIZE);
        if(NULL != Test_Queue)
                u1_printf("创建 Test_Queue 消息队列成功！\r\n");

        xReturn = xTaskCreate((TaskFunction_t)Receive_Task,
                                (const char*)"Receive_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)2,
                                (TaskHandle_t*)&Receive_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 Receive_Task 任务成功！\r\n");

        xReturn = xTaskCreate((TaskFunction_t)Send_Task,
                                (const char*)"Send_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)3,
                                (TaskHandle_t*)&Send_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 Send_Task 任务成功！\r\n");
        vTaskDelete(AppTaskCreate_Handle);
        taskEXIT_CRITICAL();
}

int rtos_start_task_queue(void)
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
