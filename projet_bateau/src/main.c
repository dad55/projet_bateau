//__________________________________________________________
// BINOME : 	PERSAND Kaveena
//		RAKOTOPARE Marcy
//		SCLAFER David
//		ZHANG Guang Xue
// ETAT : 	TERMINE
//__________________________________________________________

// registres de peripheriques du stm32
#include "stm32f10x.h"
#include "clock.h"
#include "adc.h"
#include "gpio.h"
#include "timer1234.h"
#include "math.h"
#include "string.h"
#include "fonctions_bateau.h"

#define Y_AXIS_CHANNEL 11
#define Y_AXIS_1g 1.93
#define Y_AXIS_sens 0
#define ACCELERO_Sensitivity 0.48

#define DEAD_ZONE 10
#define RF_MID 0.0015
#define TIM4_CC2_FLAG (TIM4->SR & TIM_SR_CC2IF)


#define BATTERY_CHANNEL 12
#define VCC 12
#define VCC_ratio 13
#define ADC_VDD 3.3

#define PI 3.14159265

#define LONGUEUR_MAX 50



double angle;
u16 batterie;
u8 alerte_roulis;
u8 alerte_batterie;
float alpha;
float duty_cycle;
float teta;

void send_UART( char message, USART_TypeDef * USART){
//		USART->CR1 |= USART_CR1_TE;
//		USART->DR = message;	
//		while((USART->SR & USART_SR_TC) == 0);
//		USART->CR1 &= ~USART_CR1_TE;
		
//		while(( USART -> SR & USART_SR_TXE) == 0);
//		USART -> DR = message;
		
		while(USART1->SR && USART_SR_TC == 0);
		USART1->DR = message;	
		USART1->CR1 |= USART_CR1_TE;
	}
	
void function_UART(u8 alerte_roulis, u8 alerte_batterie){
		char message[LONGUEUR_MAX];
		
		char message1[] ="ANGLE ROULIS TROP GRAND! & BATTERIE FAIBLE!";
		char message2[] ="ANGLE ROULIS TROP GRAND";
		char message3[] ="BATTERIE FAIBLE!";
		char message4[] ="OKAY";
		
		
		
		//messages de test
		/*
		char message1[] ="AA";
		char message2[] ="BB";
		char message3[] ="CC";
		char message4[] ="DD";*/
		
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

		if (ADC_ready(ADC1)){
			y=lire_ADC(ADC1);
		}
		if (ADC_ready(ADC2)){
			batterie = lire_ADC(ADC2);
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
	 Timer_IT_Update_OFF(TIM1);
 }
 

void notreTIM4_IRQHandler(void){
	float duty_cycle_moteur;
	float tmp_impulsion;
	//detecter CC2F comme IT
	if((TIM4_CC2_FLAG)!= 0)
	{
		/* Calcul de la duree de l'impulsion */
		/* Une duree de 1.5ms correspond a  une commande de 0% */
		/* Une deviation maximum de 5ms autour de 1.5ms est permise */
		/* 2ms => 100% sens '0' */
		/* 1ms => 100% sens '1' */
		tmp_impulsion = lire_duree_imp(TIM4);
		tmp_impulsion -= RF_MID;
		duty_cycle_moteur = tmp_impulsion * 100 / 0.0005;
		
		//implementation d'une zone morte de 10% autour de 1.5ms 
		if(duty_cycle_moteur > DEAD_ZONE)
			/*sens '0'*/
		{
			Port_IO_Reset(GPIOA, 2);
		}
		else if(duty_cycle_moteur < -DEAD_ZONE)
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
		Timer_IT_CCx_OFF(TIM4,2);	
	}	
}


void Convert_alpha_DC (float angle, u8 alerte_roulis){
	if (alerte_roulis == 1){
		duty_cycle = 5; // voiles non bordÃ©es
	}
	else {
		if (alpha > 180) { // remise de alpha [0;360] Ã  [0;180]
			alpha = 360-alpha;
		}
		
		if (alpha <= 45) {
			teta = 0; // les voiles sont bordÃ©es (angle des voiles Ã  0Â°), angle du bras = 90Â°
			duty_cycle = 10; 
		}
		else if (alpha >= 45) {
			
			teta =  alpha *2/3 - 30; // angle du bras dans [0Â°;90Â°]
			duty_cycle = - teta/18 + 10; 
		}
	}
	charger_DC_pwm(TIM1, 1, duty_cycle);
}

	/*######################################################
	                         MAIN
	########################################################*/


int main (void) {
	alerte_roulis=0;
	alerte_batterie=0;
	CLOCK_Configure();
	
	
	
	/*######################################################
	          CONFIGURATION DES ENTREES/SORTIES
	########################################################*/
	//configuration GPIOA floating input port 5, 6 et 7 
	//(voie I & A & B) pour la girouette
	Port_IO_Init_Input(GPIOA, 5);
	Port_IO_Init_Input(GPIOA, 6);
	Port_IO_Init_Input(GPIOA, 7);
	//Config Servo-moteur
	Port_IO_Init_AF_Output ( GPIOA, 8);
	//Config Moteur-CC
	Port_IO_Init_AF_Output( GPIOA, 1); //PWM moteur cc
	Port_IO_Init_Output( GPIOA, 2); //sens du moteur cc
	//Config Récepteur RF
	Port_IO_Init_Input( GPIOB, 6); //Récepteur RF CH1
	//Config liaison HF	
	Port_IO_Init_AF_Output (GPIOA, 9); //USART : liaison HF
	Port_IO_Init_Output(GPIOA, 11); //TX_ENABLE
	//Config ADC
	Port_IO_Init_Analog_Input(GPIOC, Y_AXIS_CHANNEL);
	Port_IO_Init_Analog_Input(GPIOC, BATTERY_CHANNEL);
		
	/*######################################################
	              CONFIGURATION DES TIMERS
	########################################################*/
	//Enable clock Timer 3
	Enable_CLK_Timer1234(TIM3);
	//init du codeur incrÃ©mental
	Init_Codeur(TIM3, 5);	
	//configuration pour lire les impulsions du récepteur RF
	Timer_Init_PWM_Input(TIM4, 1, 20000); 
	//Servo Moteur
	Timer_1234_Init(TIM1,20000); 	
	config_pwm (TIM1, 1, 5); // config TIM1 CH1		
	//Moteur CC (PWM)
	Timer_1234_Init(TIM2, 52);
	config_pwm(TIM2, 2,0);
	
	/*######################################################
	              CONFIGURATION DE L'USART
	########################################################*/
	//Config UART
	Config_UART_Projet_Bateau(USART1);
	Port_IO_Set( GPIOA, 11);
	
	/*######################################################
	              CONFIGURATION DES ADCs
	########################################################*/
	//Config Accéléromètre et niveau batterie
	power_ADC(ADC1);
	power_ADC(ADC2);
	config_adc_single_channel(ADC1, Y_AXIS_CHANNEL);
	config_adc_single_channel(ADC2, BATTERY_CHANNEL);
	
	/*######################################################
	              ACTIVATION DES INTERRUPTIONS
	########################################################*/
	//Recepteur RF & Moteur_CC
	Timer_Active_IT(TIM4,5, 2,notreTIM4_IRQHandler); 
	//Accelerometre & Batterie
	Active_IT_ADC(ADC1, 1,  &ADC_IT);//Accelerometre
	Active_IT_ADC(ADC2, 1,  &ADC_IT);//Batterie
	Timer_Active_IT(TIM1, 99, 0, &Timer_IT);//f_s ADCs
	
	
	while(1)
	{
		alpha = lire_alpha(); //angle de la girouette
		
		Convert_alpha_DC(alpha,alerte_roulis);
		
		// le bras varie entre 0 et 90 deg
		// valeur correspondant a  un bras a  0 deg (5%)
		// valeur correspondant a  un bras a  90 deg (10%)
		// quand le bras est a  90 deg les voiles sont a  0 deg
		// quand le bras est a  0 deg les voiles sont a 90 deg d'amplitude 
		// et le vent decide du signe de l'angle de la voile
	};
}
