#include "freertos_heap_test.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LED_Task_Handle = NULL;
static TaskHandle_t Test_Task_Handle = NULL;

uint8_t *Test_Ptr = NULL;
static void LED_Task(void *parameter)
{
        while(1){
                PINS_DRV_TogglePins(PTD, 1 << 0);
                vTaskDelay(1000);
        }
}

static void Test_Task(void *parameter)
{
        uint32_t g_memsize;
        uint32_t pinstate;
        while(1){
                pinstate = KEY_Proc (0);
                if(pinstate == BTN1_PRES){
                        if(NULL == Test_Ptr){
                                g_memsize = xPortGetFreeHeapSize();
                                u1_printf("系统当前内存大小为 %d 字节，开始申请内存\n", g_memsize);
                                Test_Ptr = pvPortMalloc(1024);
                                if(NULL != Test_Ptr){
                                        u1_printf("内存申请成功！\n");
                                        u1_printf("申请到的内存地址为 %#x\n", (int)Test_Ptr);

                                        g_memsize = xPortGetFreeHeapSize();
                                        u1_printf("系统当前剩余内存大小为 %d 字节，开始申请内存\n", g_memsize);
                                }
                        }
                        else{
                                u1_printf("请先按下 KEY2 释放内存再申请\n");
                        }
                }
                if(pinstate == BTN2_PRES){
                        if(NULL != Test_Ptr){
                                u1_printf("释放内存！\n");
                                vPortFree(Test_Ptr);
                                Test_Ptr = NULL;
                                g_memsize = xPortGetFreeHeapSize();
                                u1_printf("系统当前内存大小为 %d 字节，内存释放完成\n", g_memsize);
                        }
                        else{
                                u1_printf("请先按下 KEY1 申请内存再释放\n");
                        }
                }
        }
}

static AppTaskCreate(void)
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

        xReturn = xTaskCreate((TaskFunction_t)Test_Task,
                                (const char*)"Test_Task",
                                (const configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE,
                                (void*)NULL,
                                (UBaseType_t)3,
                                (TaskHandle_t*)&Test_Task_Handle);
        if(pdPASS == xReturn)
                u1_printf("创建 Test_Task 任务成功！\r\n");

        vTaskDelete(AppTaskCreate_Handle);

        taskEXIT_CRITICAL();

}

int rtos_start_heap_test(void)
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
