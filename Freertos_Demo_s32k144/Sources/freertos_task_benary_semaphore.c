#include "freertos_task_benary_semaphore.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"
#include "semphr.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t Receive_Task_Handle = NULL;
static TaskHandle_t Send_Task_Handle = NULL;

SemaphoreHandle_t BinarySem_Handle = NULL;

static void Receive_Task(void)
{
        BaseType_t xReturn = pdPASS;
        while(1){
                xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY);
                if(pdTRUE == xReturn)
                        u1_printf(" BinarySem_Handle 二值信号量获取成功！\r\n");
                PINS_DRV_TogglePins(PTD, 1 << 0);
        }

}

static void Send_Task(void)
{
        uint32_t pinstate;
        BaseType_t xReturn = pdPASS;
        while(1){
                pinstate = KEY_Proc (0);
                if(pinstate == BTN1_PRES){
                        xReturn = xSemaphoreGive(BinarySem_Handle);
                        if(pdPASS == xReturn)
                                u1_printf(" BinarySem_Handle 二值信号量释放成功！\r\n");
                        else
                                u1_printf(" BinarySem_Handle 二值信号量释放失败！\r\n");
                }
        }
        vTaskDelay(20);
}

static void AppTaskCreate(void)
{
        BaseType_t xReturn = pdPASS;

        taskENTER_CRITICAL();

        BinarySem_Handle = xSemaphoreCreateBinary();
        if(NULL != BinarySem_Handle)
                u1_printf(" BinarySem_Handle 二值信号量创建成功！\r\n");

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

int rtos_start_task_benary_semaphore(void)
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
