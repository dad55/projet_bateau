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




int main (void) {
	CLOCK_Configure();
	
	//counter de TIM3 enable 
	TIM3->CR1 = TIM3->CR1 | 0x1;
	// ARR
	TIM3 ->ARR = 1439;
	
	//TIM3_CH1 input connecte sur TI1 
	TIM3->CR2 = TIM3->CR2&~(0x1<<7);
	
	//selection timer en mode encodeur
	TIM3->SMCR = TIM3->SMCR &~0x7;
	TIM3->SMCR = TIM3->SMCR | 0x3;
	
	//configuration de chaine A sur T1
	TIM3->CCMR1 = TIM3->CCMR1 &~0x3;
	TIM3->CCMR1 = TIM3->CCMR1 | TIM_CCMR1_CC1S_1;
	
	//configuration de chaine B sur T2
	TIM3->CCMR1 = TIM3->CCMR1 &~(0x3<<8);
	TIM3->CCMR1 = TIM3->CCMR1 | TIM_CCMR1_CC2S_1;
	
	//configuration de polarité CC1P = '0' CC2P = '0'
	TIM3->CCER = TIM3->CCER & ~0x22;
	
	//configuration GPIOA output push-pull alternative function port 6 et 7 (voie A & B)
	Port_IO_Init_Input( GPIOA, 6); 
	Port_IO_Init_Input( GPIOA, 7); 
	
	// configuration GPIOA pour I Index output push-pull alternative function port 5
	Port_IO_Init_Input( GPIOA, 5);
	
	while(1);
	return 0;
}
