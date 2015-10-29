//__________________________________________________________
// BINOME : 
// ETAT : 
//__________________________________________________________

// registres de périphériques du stm32
#include "stm32f10x.h"
#include "clock.h"
#include "adc.h"
#include "gpio.h"
#include "timer1234.h"

void notreTIM4_IRQHandler(void)
{
	//char sens;
	float dc;
	float tpsH;
	//detecter CC2E comme IT
	if((TIM4->SR & TIM_SR_CC2IF) != 0)
	{
		tpsH = CLOCK_GetTIMCLK(TIM4);
		tpsH = (1/tpsH)*((TIM4->PSC)+1)*((TIM4->CCR2)+1);
		tpsH -= 0.0015;
		dc = tpsH * 100 / 0.0005;
		
		if(dc > 5)
		{
			Port_IO_Reset(GPIOA, 2);
		}
		else if( dc < -5)
		{
			dc = -dc;
			Port_IO_Set(GPIOA, 2);
		}
		else
		{
				dc = 0;
		}
		if(dc>100)
			dc = 100;
		charger_DC_pwm(TIM2, 2, dc);
	}
	TIM4->SR = 0;
}

void Timer_Init_PWM_Input_IC1_IC2(TIM_TypeDef * Timer){
	
	Timer->CCMR1 = Timer->CCMR1 &~TIM_CCMR1_CC1S;
	Timer->CCMR1 |= TIM_CCMR1_CC1S_0;
	Timer->CCER = Timer->CCER & ~TIM_CCER_CC1P;
	Timer->CCER |=TIM_CCER_CC1E ;
	Timer->CCMR1 = Timer->CCMR1 = Timer->CCMR1 &~TIM_CCMR1_CC2S;
	Timer->CCMR1 |= TIM_CCMR1_CC2S_1;	
	Timer->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P;	
	Timer->SMCR = (Timer->SMCR & ~TIM_SMCR_TS) | TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
	Timer->SMCR = (Timer->SMCR & ~TIM_SMCR_SMS) | TIM_SMCR_SMS_2;
	
	Timer->ARR = 0xFFFF;
	Timer->PSC = 0x4;
	Timer->DIER |= TIM_DIER_UIE;
	Timer->CR1= Timer->CR1| TIM_CR1_CEN;
}


int main (void) {
	CLOCK_Configure();
	(RCC->APB1ENR)|= RCC_APB1ENR_TIM4EN;
	Timer_Init_PWM_Input_IC1_IC2(TIM4);
	Port_IO_Init_AF_Output ( GPIOA, 1);
	Port_IO_Init_Output( GPIOA, 2);
	Port_IO_Init_Input( GPIOB, 6);
	
	Timer_Active_IT(TIM4,5, 2,notreTIM4_IRQHandler);
	
	Timer_1234_Init(TIM2, 50);
	charger_DC_pwm(TIM2, 2, 0);
	
	while(1);
	return 0;
}
