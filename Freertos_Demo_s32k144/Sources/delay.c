/*
 * S32K144开发板
 * 感谢原子哥的延时函数
*/

#include"cpu.h"
#include"delay.h"
#include"uart.h"

static uint32_t fac_us;


void SysTick_Init(void)  //systick 走 core_clk 48mhz  重装载寄存器 FFFFFF 为16 777 215hz 16.77mhz/48mhz 约等于333毫秒
{
	S32_SysTick->RVR = 0xFFFFFFul; //重装载寄存器
	S32_SysTick->CVR = 0ul; //当前计数
	S32_SysTick->CSR = 0u; //控制寄存器
}

/* Enable SysTick counter and interrupt */
void SysTick_Enable(void)
{
	S32_SysTick->CSR = S32_SysTick_CSR_TICKINT(1u) | S32_SysTick_CSR_ENABLE(1);
}

/* Disable SysTick */
void SysTick_Disable(void)
{
	S32_SysTick->CSR = 0ul;
}


static uint32_t fac_us;

void delay_us(uint32_t nus)
{
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=S32_SysTick->RVR;				//LOAD的值
	ticks=nus*fac_us; 						//需要的节拍数
	told= S32_SysTick->CVR ;      				//刚进入时的计数器值
	while(1)
	{
		tnow= S32_SysTick->CVR ;
		if(tnow!=told)
		{
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}
	};
}

void delay_ms(uint32_t nms)
{
	uint32_t i;
	for(i=0;i<nms;i++) delay_us(1000);
}

int delay_init(void)
{
	uint32_t frequency;
	CLOCK_SYS_GetFreq(CORE_CLOCK, &frequency);
	fac_us = frequency / 1000000;// 得到的频率是mhz 如果想知道1us多少个节拍,需要除 1 000 000
	SysTick_Init(); //systick 走 core_clk 48mhz  重装载寄存器 FFFFFF 为16 777 215hz 16.77mhz/48mhz 0.333秒  约等于333毫秒
	SysTick_Enable();
	return fac_us;
}


