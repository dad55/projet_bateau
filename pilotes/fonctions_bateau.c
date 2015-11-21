#include "stm32f10x.h"
#include "clock.h"
#include "adc.h"
#include "gpio.h"
#include "timer1234.h"
#include "math.h"
#include "string.h"
#include "fonctions_bateau.h"

float lire_duree_imp (TIM_TypeDef *Timer){
	float tmp_impulsion;
	tmp_impulsion = CLOCK_GetTIMCLK(Timer);
	tmp_impulsion = (1/tmp_impulsion)*((Timer->PSC)+1)*(float)((Timer->CCR2)+1);
	return tmp_impulsion;
}


// initialisation du codeur incremental
void Init_Codeur (TIM_TypeDef * Timer, int priority){

	// ARR
	Timer ->ARR = 1439;
	
	//configuration de chaine A sur T1 (CC1S)
	Timer->CCMR1 = Timer->CCMR1 &~ TIM_CCMR1_CC1S;
	Timer->CCMR1 = Timer->CCMR1 | TIM_CCMR1_CC1S_0;
	
	//configuration de chaine B sur T2 (CC2S)
	Timer->CCMR1 = Timer->CCMR1 &~ TIM_CCMR1_CC2S;
	Timer->CCMR1 = Timer->CCMR1 | TIM_CCMR1_CC2S_0;
	
	//configuration de polaritÃ© CC1P = '0' CC2P = '0'
	Timer->CCER = Timer->CCER & ~ TIM_CCER_CC1P;
	Timer->CCER = Timer->CCER & ~ TIM_CCER_CC2P;
	
	//selection timer en mode encodeur
	Timer->SMCR = Timer->SMCR &~ TIM_SMCR_SMS;
	Timer->SMCR = Timer->SMCR | TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
	
	//counter de Timer enable 
	Timer->CR1 = Timer->CR1 | TIM_CR1_CEN;

	// requete d'IT de la ligne 5 non masquÃ©e
	EXTI->IMR = EXTI->IMR | (0x1 << 5);
	
	// activation du front montant et descendant de la ligne 5
	EXTI->RTSR = EXTI->RTSR | (0x1 << 5);
	
	//configuration de l'interruption pour l'index
	NVIC->IP[23] = priority << 4;
	NVIC->ISER[0] = NVIC->ISER[0] | (0x1<<23);
}


	 void Config_UART_Projet_Bateau(USART_TypeDef * USART){
		 RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
		//configuration UART
		USART1->CR1 |= USART_CR1_UE;
		//BRR mantisse 468 et fraction 75 = 12/16
		USART1->BRR = (468<<4) + 12;
	 }
	 
	 
	 // fonction d'interruption de l'index I
void EXTI9_5_IRQHandler (void)
{
	// remise Ã  0 du bit d'interruption (mise Ã  1 du pending 5)
	EXTI->PR = EXTI->PR | (0x1 << 5);
	// reinitialisation du compteur
	TIM3->CNT = 0;
}

u16 lire_alpha (void){
	return (TIM3->CNT / 4);
}	






