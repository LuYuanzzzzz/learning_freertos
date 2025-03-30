/*
 * Ʒ�ǿƼ�S32K144������
 * 02-Freertos��������
 * �����ĸ�task,LED���ڲ�ͬtask���Բ�ͬƵ����˸,����ʾtask����״̬
 * KEY1���º���һ����task,printf�����������,Ȼ��ɾ������.
 * IO����
 * KEY1 PTC12
 * KEY2 PTC13
 * KEY3 PTB2
 * LED1 PTD16
 * LED2 PTD15
 * LED3 PTD1
 * LED4 PTD0
*/

#include "Cpu.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "freertos_heap_test.h"

volatile int exit_code = 0;
#define PEX_RTOS_START rtos_start

/*��������*/
void rtos_start(void)
{
        rtos_start_heap_test();
}






int main(void)
{
  /* Write your local variable definition here */
	int MCU_Freq;
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* ͨ���궨��freeRTOS��ʼ���ļ� */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/

	CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);
	PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr); //��ʼ��IO
	LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0); //��ʼ������
	MCU_Freq = delay_init();//��ʼ��delay����
	u1_printf("��ʼ�����,MCU����Ƶ��Ϊ %d Mhz \r\n",MCU_Freq);


  /*** Don't write any code pass th5is line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the NXP S32K series of microcontrollers.
**
** ###################################################################
*/
