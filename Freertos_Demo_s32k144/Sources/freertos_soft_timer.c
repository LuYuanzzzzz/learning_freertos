#include "freertos_soft_timer.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"
#include "timers.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;

static TimerHandle_t Swtmr1_Handle = NULL;
static TimerHandle_t Swtmr2_Handle = NULL;

static uint32_t TmrCb_Count1 = 0;
static uint32_t TmrCb_Count2 = 0;

static void Swtmr1_Callback(void *parameter)
{
        TickType_t tick_num1;
        TmrCb_Count1++;
        tick_num1 = xTaskGetTickCount();
        PINS_DRV_TogglePins(PTD, 1 << 0);
        u1_printf(" swtmr1_callback 函数执行 %d 次\n", TmrCb_Count1);
        u1_printf("滴答定时器数值= %d\n", tick_num1);
}

static void Swtmr2_Callback(void *parameter)
{
        TickType_t tick_num2;
        TmrCb_Count2++;
        tick_num2 = xTaskGetTickCount();
        u1_printf(" swtmr2_callback 函数执行 %d 次\n", TmrCb_Count2);
        u1_printf("滴答定时器数值= %d\n", tick_num2);
}
static void AppTaskCreate(void)
{
        taskENTER_CRITICAL();

        Swtmr1_Handle = xTimerCreate((const char*)"AutoReloadTimer",
                                        (TickType_t)1000,
                                        (UBaseType_t)pdTRUE,
                                        (void*)1,
                                        (TimerCallbackFunction_t)Swtmr1_Callback);
        if(Swtmr1_Handle != NULL){
                u1_printf(" 定时器 1 创建成功！\r\n");
                xTimerStart(Swtmr1_Handle, 0);
        }

        Swtmr2_Handle = xTimerCreate((const char*)"OneShotTimer",
                                        (TickType_t)5000,
                                        (UBaseType_t)pdFALSE,
                                        (void*)2,
                                        (TimerCallbackFunction_t)Swtmr2_Callback);
        if(Swtmr2_Handle != NULL){
                u1_printf(" 定时器 2 创建成功！\r\n");
                xTimerStart(Swtmr2_Handle, 0);
        }

        vTaskDelete(AppTaskCreate_Handle);
        taskEXIT_CRITICAL();

}

int rtos_start_soft_timer(void)
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
