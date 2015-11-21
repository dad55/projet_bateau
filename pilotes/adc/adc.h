#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"


/*Allumer le adc*/
void power_ADC (ADC_TypeDef * ADC);

/*Configuer l'adc en conversion unique en choisissant la chaîne voulue*/
void config_adc_single_channel (ADC_TypeDef * ADC, u8 channel);

/*Retourne le résultat de la conversion sur les 12 lsb
  Necessite d'avoir allumer l'adc et de l'avoir configuré*/
u16 read_result_conv(ADC_TypeDef * ADC);

/*Lancement d'une nouvelle conversion*/
void start_conv(ADC_TypeDef * ADC);


/*Active l'intérruption ADC1 et ADC2     
  Remarque : ADC1 et ADC2 ont une seule    
  intérruption globale et si on utilise  
  cette fonction pour changer le handler 
  de l'un autre et automatiquement modifié*/
void Active_IT_ADC(ADC_TypeDef * ADC, u8 Priority,  void (*IT_function) (void));


u16 lire_ADC(ADC_TypeDef * ADC);

/* Fonction qui renvoit 0 si la conversion
   a fini, une valeur different de 0 sinon
*/
u8 ADC_ready(ADC_TypeDef * ADC);

#endif

