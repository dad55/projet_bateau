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
#define Y_AXIS_1g 2.13
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
		while(USART->SR && USART_SR_TC == 0);
		USART->DR = message;	
		USART->CR1 |= USART_CR1_TE;
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
		
		if ((angle>45)&(alerte_roulis==0))
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
	 TIM1->SR = TIM1->SR &~ TIM_SR_UIF;
	 start_conv(ADC1);
	 start_conv(ADC2);
 }
 

	 void Config_UART_Projet_Bateau(USART_TypeDef * USART){
		 RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
		//configuration UART
		USART1->CR1 |= USART_CR1_UE;
		//BRR mantisse 468 et fraction 75 = 12/16
		USART1->BRR = (468<<4) + 12;
	 }
 
int main (void) {

	CLOCK_Configure();

	Port_IO_Init_AF_Output (GPIOA, 9); //USART : liaison HF
	alerte_roulis=0;
	alerte_batterie=0;

	
	Config_UART_Projet_Bateau(USART1);
	Timer_1234_Init(TIM1, 20000);
	Timer_Init_Compare (TIM1, 50.0, 2);
	power_ADC(ADC1);
	power_ADC(ADC2);
	config_adc_single_channel(ADC1, Y_AXIS_CHANNEL);
	config_adc_single_channel(ADC2, BATTERY_CHANNEL);
	Active_IT_ADC(ADC1, 15,  &ADC_IT);
	Active_IT_ADC(ADC2, 15,  &ADC_IT);
	Timer_Active_IT( TIM1, 17, 0, &Timer_IT);

	
	



	


	while(1);

}
