#include "stm32f10x.h"
#include "timer1234.h"
#include "clock.h"
#include "gpio.h"
#include "math.h"



void (*adr_fonction_it1) (void);
void (*adr_fonction_it2) (void);
void (*adr_fonction_it3) (void);
void (*adr_fonction_it4) (void);
void (*adr_fonction_it_TIM1_CC) (void);

void TIM1_UP_IRQHandler (void)
{
	(*adr_fonction_it1)();
}

void TIM2_IRQHandler (void)
{
	(*adr_fonction_it2)();
}

void TIM3_IRQHandler (void)
{
	(*adr_fonction_it3)();
}

void TIM4_IRQHandler (void)
{
	(*adr_fonction_it4)();
}


void Enable_CLK_Timer1234(TIM_TypeDef *Timer)
{
	if (Timer == TIM1)
		(RCC->APB2ENR) |= RCC_APB2ENR_TIM1EN;
	else if (Timer == TIM2)
		(RCC->APB1ENR) |= RCC_APB1ENR_TIM2EN;
	else if (Timer == TIM3)
		(RCC->APB1ENR) |= RCC_APB1ENR_TIM3EN;
	else if (Timer == TIM4)
		(RCC->APB1ENR) |= RCC_APB1ENR_TIM4EN;
}

float Timer_1234_Init(TIM_TypeDef *Timer, float Duree_us )
{
	u32 freq_timer, prescale,counter;
	float duree_timer_reelle;
	
	Enable_CLK_Timer1234(Timer);
	
	freq_timer = CLOCK_GetTIMCLK (Timer);
	
	prescale = (int)(((float)(Duree_us * 0.000001) * freq_timer) / 65535)+1;
	Timer -> PSC = prescale;
	counter = (int)(Duree_us * 0.000001 * freq_timer / ((double)(prescale+1)))-1; 
	Timer -> ARR = counter;
	Timer -> CR1 = 0x1; // Autoreload
	
	duree_timer_reelle = (float) (counter * prescale / freq_timer);
	
	return duree_timer_reelle;
}

void Timer_Active_IT( TIM_TypeDef *Timer, u8 Priority, u8 channel,void (*IT_function) (void) )
{
		u32 numIT=0;
	//configuration NVIC
	//reglage priorite timer
	if( Timer == TIM1)
			numIT = 25;
	else if( Timer == TIM2)
			numIT = 28;
	else if (Timer == TIM3)
			numIT = 29;
	else if (Timer == TIM4)
			numIT = 30;
	
  NVIC -> IP [numIT] = Priority << 4;
	
	
	//autoriser le NVIC a prendre a compte l'interruption du Timer
	NVIC ->ISER[0] = NVIC -> ISER[0] | (0x1 << numIT); 
	
	if (Timer == TIM1)
		adr_fonction_it1 = IT_function;
	else if (Timer == TIM2)
		adr_fonction_it2 = IT_function;
	else if (Timer == TIM3)
		adr_fonction_it3 = IT_function;
	else if (Timer == TIM4)
		adr_fonction_it4 = IT_function;	
	
	// mise à 1 exclusive du bit UIE registre TIMx_DIER pour declencher
	// une interruption du timer sur debordement et activation IT
	Timer -> DIER = (Timer -> DIER | 0x1 | (1 << channel));
}


void config_pwm (TIM_TypeDef *Timer, u8 channel, float duty_cycle)
{
	 u32 counter;
	
	Enable_CLK_Timer1234(Timer);
	
	switch (channel)
	{
		case 1 : 
				counter = Timer -> ARR;
				
				Timer -> CCR1 = (u32)(counter * (duty_cycle / 100));
				
				Timer -> CCMR1 = Timer -> CCMR1 &~ ((TIM_CCMR1_OC1M) | TIM_CCMR1_OC1PE);
				Timer -> CCMR1 = Timer -> CCMR1 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
				
				Timer -> CCER = Timer-> CCER &~ TIM_CCER_CC1P;
				Timer -> CCER = Timer -> CCER | TIM_CCER_CC1E;
				Timer -> CR1 = Timer -> CR1 | TIM_CR1_CEN;
		
				break;
		case 2 : 
				counter = Timer -> ARR;
				
				Timer -> CCR2 = (u32)(counter * (duty_cycle / 100));
				
				Timer -> CCMR1 = Timer -> CCMR1 &~ ((TIM_CCMR1_OC2M) | TIM_CCMR1_OC2PE);
				Timer -> CCMR1 = Timer -> CCMR1 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
				
				Timer -> CCER = Timer-> CCER &~ TIM_CCER_CC2P;
				Timer -> CCER = Timer -> CCER | TIM_CCER_CC2E;
				Timer -> CR1 = Timer -> CR1 | TIM_CR1_CEN;
		
				break;
		case 3 : 
				counter = Timer -> ARR;
				
				Timer -> CCR3 = (u32)(counter * (duty_cycle / 100));
				
				Timer -> CCMR2 = Timer -> CCMR2 &~ ((TIM_CCMR2_OC3M) | TIM_CCMR2_OC3PE);
				Timer -> CCMR2 = Timer -> CCMR2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
				
				Timer -> CCER = Timer-> CCER &~ TIM_CCER_CC3P;
				Timer -> CCER = Timer -> CCER | TIM_CCER_CC3E;
				Timer -> CR1 = Timer -> CR1 | TIM_CR1_CEN;
		
				break;
		case 4 : 
				counter = Timer -> ARR;
				
				Timer -> CCR4 = (u32)(counter * (duty_cycle / 100));
				
				Timer -> CCMR2 = Timer -> CCMR2 &~ ((TIM_CCMR2_OC4M) | TIM_CCMR2_OC4PE);
				Timer -> CCMR2 = Timer -> CCMR2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
				
				Timer -> CCER = Timer-> CCER &~ TIM_CCER_CC4P;
				Timer -> CCER = Timer -> CCER | TIM_CCER_CC4E;
				Timer -> CR1 = Timer -> CR1 | TIM_CR1_CEN;
		
				break;
	}
	
	if (Timer == TIM1)
		TIM1 -> BDTR = TIM1 -> BDTR | TIM_BDTR_MOE;
	
	
}

void charger_DC_pwm(TIM_TypeDef *Timer, u8 channel, float duty_cycle)
{
	switch(channel)
	{
		case 1: Timer->CCR1 = (u32)((Timer->ARR + 1) * (duty_cycle / 100)); break;
		case 2: Timer->CCR2 = (u32)((Timer->ARR + 1) * (duty_cycle / 100)); break;
		case 3: Timer->CCR3 = (u32)((Timer->ARR + 1) * (duty_cycle / 100)); break;
		case 4: Timer->CCR4 = (u32)((Timer->ARR + 1) * (duty_cycle / 100));
	}
}
void Timer_Init_PWM_Input(TIM_TypeDef * Timer, u8 voie, int duree_impulsion_max){
	int prescale;
	u32 freq_timer;
	Enable_CLK_Timer1234(Timer);
	freq_timer = CLOCK_GetTIMCLK (Timer);
	prescale = (int)(((float)(duree_impulsion_max* 0.000001) * freq_timer) / 65535)+1;
	Timer->ARR = 0xFFFF;
	Timer->PSC = prescale;
	Timer->CR1= Timer->CR1| TIM_CR1_CEN;
	switch (voie){
		case 1 :
			Timer->CCMR1 = Timer->CCMR1 &~TIM_CCMR1_CC1S;
			Timer->CCMR1 |= TIM_CCMR1_CC1S_0;
			Timer->CCER = Timer->CCER & ~TIM_CCER_CC1P;
			Timer->CCER |=TIM_CCER_CC1E ;
			Timer->CCMR1 = Timer->CCMR1 = Timer->CCMR1 &~TIM_CCMR1_CC2S;
			Timer->CCMR1 |= TIM_CCMR1_CC2S_1;	
			Timer->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P;	
			Timer->SMCR = (Timer->SMCR & ~TIM_SMCR_TS) | TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
			Timer->SMCR = (Timer->SMCR & ~TIM_SMCR_SMS) | TIM_SMCR_SMS_2;
			break;
		case 2 :
			Timer->CCMR2 = Timer->CCMR2 &~TIM_CCMR2_CC3S;
			Timer->CCMR2 |= TIM_CCMR2_CC3S_0;
			Timer->CCER = Timer->CCER & ~TIM_CCER_CC3P;
			Timer->CCER |=TIM_CCER_CC3E ;
			Timer->CCMR2 = Timer->CCMR2 &~TIM_CCMR2_CC4S;
			Timer->CCMR2 |= TIM_CCMR2_CC4S_1;	
			Timer->CCER |= TIM_CCER_CC4E | TIM_CCER_CC4P;	
			Timer->SMCR = (Timer->SMCR & ~TIM_SMCR_TS) | TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
			Timer->SMCR = (Timer->SMCR & ~TIM_SMCR_SMS) | TIM_SMCR_SMS_2;
			break;
	}

}


