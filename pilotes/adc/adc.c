#include "stm32f10x.h"
#include "adc.h"
void (*ptr_func_it_adc)(void);

void power_ADC (ADC_TypeDef * ADC)
{
		if (ADC == ADC1)
				RCC -> APB2ENR |= RCC_APB2ENR_ADC1EN;
		else if  (ADC == ADC2)
				RCC -> APB2ENR |= RCC_APB2ENR_ADC2EN;
		
		ADC -> CR2 = 0x1; // allumage ADC -- mise a 1 bit adon (bit 0)
}
		

void config_adc_single_channel (ADC_TypeDef * ADC, u8 channel)
{
		if (channel < 16)
			{
				ADC -> SQR3 = ADC -> SQR3 &~(0xF);
				ADC -> SQR3 = (ADC -> SQR3) | channel; // sqr3 pour la conversion 1 de la chaine channel.
			}
}

u16 read_result_conv(ADC_TypeDef * ADC)
{
		u16 result;

		
		ADC -> CR2 = 0x1; // lancement de la conversion
		while ((((ADC -> SR & 0x2)>>1) != 0x1));
		
		result = (ADC -> DR) &~ (0xF000); // ADC->DR aligné a droite sur 12 bits, SR retombe a 0 tt seul
		return result;
}

void ADC_IRQHandler(void){	
	(*ptr_func_it_adc)();	
}

void Active_IT_ADC(ADC_TypeDef * ADC, u8 Priority,  void (*IT_function) (void)){
	NVIC -> IP [18] = Priority << 4;	
	ptr_func_it_adc = IT_function;
	ADC->CR1 = ADC->CR1 | (0x1 <<5); //interrupt enable
	NVIC->ISER[0] = NVIC->ISER[0] | (0x1<<18); //interruption de l'ADC 1 et 2	
}


void start_conv(ADC_TypeDef * ADC){
	ADC -> CR2 |= 0x1; // lancement de la conversion
}




