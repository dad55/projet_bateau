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


// fonction d'interruption de l'index I
void EXTI9_5_IRQHandler (void)
{
	// remise à 0 du bit d'interruption (mise à 1 du pending 5)
	EXTI->PR = EXTI->PR | (0x1 << 5);
	// réinitialisation du compteur
	TIM3->CNT = 0;
}

// initialisation du codeur incrémental
void Init_Codeur (TIM_TypeDef * Timer){

	// ARR
	Timer ->ARR = 1439;
	
	//configuration de chaine A sur T1 (CC1S)
	Timer->CCMR1 = Timer->CCMR1 &~ TIM_CCMR1_CC1S;
	Timer->CCMR1 = Timer->CCMR1 | TIM_CCMR1_CC1S_0;
	
	//configuration de chaine B sur T2 (CC2S)
	Timer->CCMR1 = Timer->CCMR1 &~ TIM_CCMR1_CC2S;
	Timer->CCMR1 = Timer->CCMR1 | TIM_CCMR1_CC2S_0;
	
	//configuration de polarité CC1P = '0' CC2P = '0'
	Timer->CCER = Timer->CCER & ~ TIM_CCER_CC1P;
	Timer->CCER = Timer->CCER & ~ TIM_CCER_CC2P;
	
	//selection timer en mode encodeur
	Timer->SMCR = Timer->SMCR &~ TIM_SMCR_SMS;
	Timer->SMCR = Timer->SMCR | TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
	
	//counter de Timer enable 
	Timer->CR1 = Timer->CR1 | TIM_CR1_CEN;
}

void Init_IT_Codeur (int priority){
	// requete d'IT de la ligne 5 non masquée
	EXTI->IMR = EXTI->IMR | (0x1 << 5);
	// activation du front montant et descendant de la ligne 5
	EXTI->RTSR = EXTI->RTSR | (0x1 << 5);
	
	//configuration de l'interruption pour l'index
	NVIC->IP[23] = priority << 4;
	NVIC->ISER[0] = NVIC->ISER[0] | (0x1<<23);
}

int alpha;
int main (void) {
	
	//activation des clock
	CLOCK_Configure();
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	//configuration GPIOA floating input port 5, 6 et 7 (voie I & A & B)
	Port_IO_Init_Input(GPIOA, 5);
	Port_IO_Init_Input(GPIOA, 6); 
	Port_IO_Init_Input(GPIOA, 7);
	
	//init du codeur incrémental
	Init_Codeur(TIM3);
	
	//init de l'IT du codeur
	Init_IT_Codeur(5);
	
	alpha = TIM3->CNT / 4; //angle de la girouette
	
	while (1);
	
	return 0;
}
