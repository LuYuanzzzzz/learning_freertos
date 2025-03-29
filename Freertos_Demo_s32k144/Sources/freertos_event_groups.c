#include "freertos_event_groups.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "task.h"
#include "event_groups.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t LED_Task_Handle = NULL;
static TaskHandle_t KEY_Task_Handle = NULL;

static EventGroupHandle_t Event_Handle = NULL;

#define KEY1_EVENT (0X01 << 0)
#define KEY2_EVENT (0x01 << 1)

static void LED_Task(void *parameter)
{
        EventBits_t r_event;
        while(1){
                r_event = xEventGroupWaitBits(Event_Handle,
                                                KEY1_EVENT | KEY2_EVENT,
                                                pdTRUE,
                                                pdTRUE,
                                        portMAX_DELAY);
                if((r_event & (KEY1_EVENT | KEY2_EVENT)) == (KEY1_EVENT | KEY2_EVENT)){
                        u1_printf(" KEY1 与 KEY2 都按下\n");
                        PINS_DRV_TogglePins(PTD, 1 << 0);
                }
                else
                        u1_printf("事件错误！\n");
        }

}

static void KEY_Task(void *parament)
{
        uint32_t pinstate;
        while(1){
                pinstate = KEY_Proc (0);
                if(pinstate == BTN1_PRES){
                        u1_printf(" KEY1被按下\n");
                        xEventGroupSetBits(Event_Handle, KEY1_EVENT);
                }
                if(pinstate == BTN2_PRES){
                        u1_printf(" KEY2被按下\n");
                        xEventGroupSetBits(Event_Handle, KEY2_EVENT);
                }
                vTaskDelay(20);
        }
}

static AppTaskCreate(void)
{
        BaseType_t xReturn = pdPASS;
        taskENTER_CRITICAL();

        Event_Handle = xEventGroupCreate();
        if(NULL != Event_Handle)
                u1_printf(" Event_Handle 事件创建成功！\r\n");

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

int rtos_start_event_groups(void)
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
