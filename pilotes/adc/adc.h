#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"


/*Allumer le adc*/
void power_ADC (ADC_TypeDef * ADC);

/*Configuer l'adc en conversion unique en choisissant la cha�ne voulue*/
void config_adc_single_channel (ADC_TypeDef * ADC, u8 channel);

/*Retourne le r�sultat de la conversion sur les 12 lsb
  Necessite d'avoir allumer l'adc et de l'avoir configur�*/
u16 read_result_conv(ADC_TypeDef * ADC);

#endif

