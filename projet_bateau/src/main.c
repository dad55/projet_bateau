//__________________________________________________________
// BINOME : 
// ETAT : 
//__________________________________________________________

// registres de pÃ©riphÃ©riques du stm32
#include "stm32f10x.h"
#include "clock.h"
#include "adc.h"
#include "gpio.h"
#include "timer1234.h"
#include "math.h"
#include "string.h"

#define Y_AXIS_CHANNEL 11
#define Y_AXIS_1g 1.93
#define Y_AXIS_sens 0
#define ACCELERO_Sensitivity 0.48


#define BATTERY_CHANNEL 12
#define VCC 12
#define VCC_ratio 13
#define ADC_VDD 3.3

#define PI 3.14159265

#define LONGUEUR_MAX 50

/* Si le sens est Ã©gal Ã  0 plus le bateau */
/* s'incline plus et moins l'acceleration  */
/* sera positive*/

		double angle;
		u16 batterie;
		u8 alerte_roulis;
		u8 alerte_batterie;


	void send_UART( char message, USART_TypeDef * USART){
		USART->CR1 |= USART_CR1_TE;
		USART->DR = message;	
		while(USART->SR & USART_SR_TC == 0);
		USART->CR1 &= ~USART_CR1_TE;
		
	}
	
	void function_UART(u8 alerte_roulis, u8 alerte_batterie){
		char message[LONGUEUR_MAX];
		/*
		char message1[] ="ANGLE ROULIS TROP GRAND! & BATTERIE FAIBLE!";
		char message2[] ="ANGLE ROULIS TROP GRAND";
		char message3[] ="BATTERIE FAIBLE!";
		char message4[] ="OKAY";
		*/
		
		
		//messages de test
		char message1[] ="AA";
		char message2[] ="BB";
		char message3[] ="CC";
		char message4[] ="DD";
		
		int i = 0;
		char  char_msg;
		if ((alerte_roulis==1)&(alerte_batterie==1)){
			strcpy(message,message1);
		}
		else if ((alerte_roulis==1)&(alerte_batterie==0)){
			strcpy(message,message2);
		}
		else if ((alerte_roulis==0)&(alerte_batterie==1)){
			strcpy(message, message3 );
		}
		else{
			strcpy(message, message4);
		}
		char_msg = message[0];
		
		while (char_msg != '\0'){
			send_UART(*message, USART1);
			i++;
			char_msg=message[i];
		}
	}


  void ADC_IT (void){			
		u16 y;

		if ((ADC1->SR & ADC_SR_EOC) ==ADC_SR_EOC){
			y = (ADC1 -> DR) &~ (0xF000);
			ADC1->SR = ADC1->SR &~ ADC_SR_EOC;
		}
		if ((ADC2->SR & ADC_SR_EOC) ==ADC_SR_EOC){
			batterie = (ADC2->DR)&~ (0xF000);
			ADC2->SR = ADC2->SR &~ ADC_SR_EOC;
		}
		angle = acos(((y*ADC_VDD/0xFFF)-(Y_AXIS_1g-ACCELERO_Sensitivity))/ACCELERO_Sensitivity)*180/PI;
		
		if ((angle>45)&&(alerte_roulis==0))
			alerte_roulis=1;
		else if((angle<44)&&(alerte_roulis==1))
			alerte_roulis=0;
		if ((batterie*ADC_VDD/0xFFF)<(VCC*0.8/VCC_ratio))
			alerte_batterie=1;
		else if ((batterie*ADC_VDD/0xFFF)>(VCC*0.81/VCC_ratio))
			alerte_batterie=0;
		function_UART(alerte_roulis, alerte_batterie);
 }

 
 void Timer_IT(void){
	 
	 start_conv(ADC1);
	 start_conv(ADC2);
	 TIM1->SR = TIM1->SR &~ TIM_SR_UIF;
 }
 
 /*void Timer1_CC(void){
		TIM1->SR = TIM1->SR &~ TIM_SR_CC2IF;
		start_conv(ADC1);
		start_conv(ADC2);		
 }*/


	 void Config_UART_Projet_Bateau(USART_TypeDef * USART){
		 RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
		//configuration UART
		USART1->CR1 |= USART_CR1_UE;
		//BRR mantisse 468 et fraction 75 = 12/16
		USART1->BRR = (468<<4) + 12;
	 }
 

void notreTIM4_IRQHandler(void)
{
	float duty_cycle_moteur;
	float tmp_impulsion;
	//detecter CC2E comme IT
	if((TIM4->SR & TIM_SR_CC2IF) != 0)
	{
		/* Calcul de la durÃƒÂ©e de l'impulsion */
		/* Une durÃƒÂ©e de 1.5ms correspond Ãƒ  une commande de 0% */
		/* Une deviation maximum de 5ms autour de 1.5ms est permise */
		/* 2ms => 100% sens '0' */
		/* 1ms => 100% sens '1' */
		tmp_impulsion = CLOCK_GetTIMCLK(TIM4);
		tmp_impulsion = (1/tmp_impulsion)*((TIM4->PSC)+1)*((TIM4->CCR2)+1);
		tmp_impulsion -= 0.0015;
		duty_cycle_moteur = tmp_impulsion * 100 / 0.0005;
		
		//implÃƒÂ©mentation d'une zone morte de 5% autour de 1.5ms 
		if(duty_cycle_moteur > 5)
			/*sens '0'*/
		{
			Port_IO_Reset(GPIOA, 2);
		}
		else if(duty_cycle_moteur < -5)
		{
			/*sens '1'*/
			duty_cycle_moteur = -duty_cycle_moteur;
			Port_IO_Set(GPIOA, 2);
		}
		else
		{
				duty_cycle_moteur = 0;
		}
		if(duty_cycle_moteur>100)
			/*prise en compte d'une saturation*/
			{duty_cycle_moteur = 100;} 
		charger_DC_pwm(TIM2,2,duty_cycle_moteur);
	}
	TIM4->SR = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// fonction d'interruption de l'index I
void EXTI9_5_IRQHandler (void)
{
	// remise Ã  0 du bit d'interruption (mise Ã  1 du pending 5)
	EXTI->PR = EXTI->PR | (0x1 << 5);
	// rÃ©initialisation du compteur
	TIM3->CNT = 0;
}

// initialisation du codeur incrÃ©mental
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


float alpha;
float duty_cycle;
float teta;

void Convert_alpha_DC (float angle, u8 alerte_roulis){
	if (alerte_roulis == 1){
		duty_cycle = 5; // voiles non bordÃ©es
	}
	else {
		if (alpha > 180) { // remise de alpha [0;360] Ã  [0;180]
			alpha = 360-alpha;
		}
		
		if (alpha <= 45) {
			teta = 0; // les voiles sont bordÃ©es (angle des voiles Ã  0Â°), angle du bras = 90Â°
			duty_cycle = 10; 
		}
		else if (alpha >= 45) {
			
			teta =  alpha *2/3 - 30; // angle du bras dans [0Â°;90Â°]
			duty_cycle = - teta/18 + 10; 
		}
	}
	charger_DC_pwm(TIM1, 1, duty_cycle);
}




int main (void) {
	alerte_roulis=0;
	alerte_batterie=0;
	CLOCK_Configure();
	Timer_Init_PWM_Input(TIM4, 1, 25);
	Port_IO_Init_AF_Output( GPIOA, 1);
	Port_IO_Init_Output( GPIOA, 2);
	Port_IO_Init_Input( GPIOB, 6);
	
	
	
	Port_IO_Init_AF_Output (GPIOA, 9); //USART : liaison HF


	Timer_1234_Init(TIM1,20000);
	
	Config_UART_Projet_Bateau(USART1);
	power_ADC(ADC1);
	power_ADC(ADC2);
	config_adc_single_channel(ADC1, Y_AXIS_CHANNEL);
	config_adc_single_channel(ADC2, BATTERY_CHANNEL);

	
	
	
	
	
	
	
	//Timer_1234_Init(TIM2, 50);
	//gÃ©rer RF433Mhz
	Timer_1234_Init(TIM2, 52);
	
	config_pwm(TIM2, 2,0);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//activation des clock
	Enable_CLK_Timer1234(TIM3); // enable clock timer
	
	//configuration GPIOA floating input port 5, 6 et 7 (voie I & A & B)
	Port_IO_Init_Input(GPIOA, 5);
	Port_IO_Init_Input(GPIOA, 6);
	Port_IO_Init_Input(GPIOA, 7);
	
	//init du codeur incrÃ©mental
	Init_Codeur(TIM3, 5);
	
	// Servomoteur PWM
	// duty cycle varie entre 1/20 Ã  2/20 => entre 5 et 10%
	
	//alpha = 30;
	Port_IO_Init_AF_Output ( GPIOA, 8);
	
	config_pwm (TIM1, 1, 5); // config TIM1 CH1
	Timer_Active_IT(TIM4,5, 2,notreTIM4_IRQHandler);
	
	Active_IT_ADC(ADC1, 1,  &ADC_IT);
	Active_IT_ADC(ADC2, 1,  &ADC_IT);

	
	Timer_Active_IT( TIM1, 99, 0, &Timer_IT);
	
	Port_IO_Set( GPIOA, 11);
	while(1)
	{
		alpha = TIM3->CNT / 4; //angle de la girouette
		
		Convert_alpha_DC(alpha,alerte_roulis);
		
		// le bras varie entre 0 et 90Â°
		// valeur correspondant Ã  un bras Ã  0Â° (5%)
		// valeur correspondant Ã  un bras Ã  90Â° (10%)
		// quand le bras est Ã  90Â° les voiles sont Ã  0Â°
		// quand le bras est Ã  0Â° les voiles sont Ã  90Â° d'amplitude et le vent dÃ©cide du signe de l'angle de la voile
	};
	return 0;
}
