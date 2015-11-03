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
	float duty_cycle_moteur;
	float tmp_impulsion;
	//detecter CC2E comme IT
	if((TIM4->SR & TIM_SR_CC2IF) != 0)
	{
		/* Calcul de la durÃ©e de l'impulsion */
		/* Une durÃ©e de 1.5ms correspond Ã  une commande de 0% */
		/* Une deviation maximum de 5ms autour de 1.5ms est permise */
		/* 2ms => 100% sens '0' */
		/* 1ms => 100% sens '1' */
		tmp_impulsion = CLOCK_GetTIMCLK(TIM4);
		tmp_impulsion = (1/tmp_impulsion)*((TIM4->PSC)+1)*((TIM4->CCR2)+1);
		tmp_impulsion -= 0.0015;
		duty_cycle_moteur = tmp_impulsion * 100 / 0.0005;
		
		//implÃ©mentation d'une zone morte de 5% autour de 1.5ms 
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



int main (void) {
	CLOCK_Configure();
	Timer_Init_PWM_Input(TIM4, 1, 25);
	Port_IO_Init_AF_Output( GPIOA, 1);
	Port_IO_Init_Output( GPIOA, 2);
	Port_IO_Init_Input( GPIOB, 6);
	
	Timer_Active_IT(TIM4,5, 2,notreTIM4_IRQHandler);
	
	Timer_1234_Init(TIM2, 50);
	config_pwm(TIM2, 0, 2, 40);

	
	while(1);
	return 0;
}
