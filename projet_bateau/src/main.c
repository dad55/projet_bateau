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
void Init_Codeur (TIM_TypeDef * Timer, int priority){

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

	// requete d'IT de la ligne 5 non masquée
	EXTI->IMR = EXTI->IMR | (0x1 << 5);
	
	// activation du front montant et descendant de la ligne 5
	EXTI->RTSR = EXTI->RTSR | (0x1 << 5);
	
	//configuration de l'interruption pour l'index
	NVIC->IP[23] = priority << 4;
	NVIC->ISER[0] = NVIC->ISER[0] | (0x1<<23);
}


float alpha;
float duty_cycle;
float teta;
u8 alerte_roulis;

void Convert_alpha_DC (float angle, u8 alerte_roulis){
	if (alerte_roulis == 1){
		duty_cycle = 10; // voiles non bordées
	}
	else {
		if (alpha > 180) { // remise de alpha [0;360] à [0;180]
			alpha = 360-alpha;
		}
		
		if (alpha <= 45) {
			teta = 0; // les voiles sont bordées (angle des voiles à 0°), angle du bras = 90°
			duty_cycle = 10; 
		}
		else if (alpha >= 45) {
			
			teta =  alpha *2/3 - 30; // angle du bras dans [0°;90°]
			duty_cycle = - teta/18 + 10; 
		}
	}
	charger_DC_pwm(TIM1, 1, duty_cycle);
}

int main (void) {
	
	//activation des clock
	CLOCK_Configure();
	Enable_CLK_Timer1234(TIM3); // enable clock timer
	
	//configuration GPIOA floating input port 5, 6 et 7 (voie I & A & B)
	Port_IO_Init_Input(GPIOA, 5);
	Port_IO_Init_Input(GPIOA, 6);
	Port_IO_Init_Input(GPIOA, 7);
	
	//init du codeur incrémental
	Init_Codeur(TIM3, 5);
	
	// Servomoteur PWM
	// duty cycle varie entre 1/20 à 2/20 => entre 5 et 10%
	
	//alpha = 30;
	Port_IO_Init_AF_Output ( GPIOA, 8);
	Timer_1234_Init(TIM1,20000);
	config_pwm (TIM1, 1, 5); // config TIM1 CH1
	
	while (1){
		alpha = TIM3->CNT / 4; //angle de la girouette
		
		Convert_alpha_DC(alpha, alerte_roulis);
		
		// le bras varie entre 0 et 90°
		// valeur correspondant à un bras à 0° (5%)
		// valeur correspondant à un bras à 90° (10%)
		// quand le bras est à 90° les voiles sont à 0°
		// quand le bras est à 0° les voiles sont à 90° d'amplitude et le vent décide du signe de l'angle de la voile

	//duty_cycle = 5;
	//duty_cycle = 10;
	
	}
	//while(1);
}
