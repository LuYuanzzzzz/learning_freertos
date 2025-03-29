#include "freertos_task_notify.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Receive1_Task_Handle = NULL;
static TaskHandle_t Receive2_Task_Handle = NULL;
static TaskHandle_t Send_Task_Handle = NULL;

static void Receive1_Task(void *parament)
{
        BaseType_t xReturn = pdPASS;
        uint32_t r_num;
        while(1){
                xReturn = xTaskNotifyWait(0x0,
                                        0xFFFFFFFF,
                                        &r_num,
                                        portMAX_DELAY);
                if(pdTRUE == xReturn)
                        u1_printf(" Receive1_Task 任务通知为 %d \n", r_num);
                PINS_DRV_TogglePins(PTD, 1 << 0);
        }
}

static void Receive2_Task(void *parament)
{
        BaseType_t xReturn = pdPASS;
        uint32_t r_num;
        while(1){
                xReturn = xTaskNotifyWait(0x0,
                                        0xFFFFFFFF,
                                        &r_num,
                                        portMAX_DELAY);
                if(pdTRUE == xReturn)
                        u1_printf(" Receive2_Task 任务通知为 %d \n", r_num);
                PINS_DRV_TogglePins(PTD, 1 << 1);
        }

}

static void Send_Task(void *parament)
{
        BaseType_t xReturn = pdPASS;
        uint32_t send1 = 1;
        uint32_t send2 = 2;

        uint32_t pinstate;
        while(1){
                pinstate = KEY_Proc (0);
                if(pinstate == BTN1_PRES){
                        u1_printf(" KEY1被按下\n");
                        xReturn = xTaskNotify(Receive1_Task_Handle,
                                                send1,
                                                eSetValueWithOverwrite);
                        if(xReturn = pdPASS)
                                u1_printf(" Receive1_Task_Handle 任务通知释放成功！\r\n");
                }
                if(pinstate == BTN2_PRES){
                        u1_printf(" KEY2被按下\n");
                        xReturn = xTaskNotify(Receive2_Task_Handle,
                                send2,
                                eSetValueWithOverwrite);
                        if(xReturn = pdPASS)
                                u1_printf(" Receive2_Task_Handle 任务通知释放成功！\r\n");
                }
                vTaskDelay(20);
        }
}

static AppTaskCreate(void)
{
        BaseType_t xReturn = pdPASS;
        taskENTER_CRITICAL();

        xReturn = xTaskCreate((TaskFunction_t)Receive1_Task,
                                (const char*)"Receive1_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)2,
                                (TaskHandle_t*)&Receive1_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 Receive1_Task 任务成功！\r\n");

        xReturn = xTaskCreate((TaskFunction_t)Receive2_Task,
                                (const char*)"Receive2_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)3,
                                (TaskHandle_t*)&Receive2_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 Receive2_Task 任务成功！\r\n");

        xReturn = xTaskCreate((TaskFunction_t)Send_Task,
                                (const char*)"Send_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)4,
                                (TaskHandle_t*)&Send_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 Send_Task 任务成功！\r\n");

        vTaskDelete(AppTaskCreate_Handle);

        taskEXIT_CRITICAL();

}
int rtos_start_task_notify(void)
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
