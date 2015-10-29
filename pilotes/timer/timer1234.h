#ifndef TIMER_H
#define TIMER_H

#include "stm32f10x.h"

extern void (*adr_fonction_it1) (void);
extern void (*adr_fonction_it2) (void);
extern void (*adr_fonction_it3) (void);
extern void (*adr_fonction_it4) (void);



/* Configure un timer avec la dur�e voulue en us.
renvoie la dur�e r�ellement compt�e
ex : duree_reelle = Timer_1234_Init (TIM3, 500);
*/
float Timer_1234_Init(TIM_TypeDef *Timer, float Duree_us );


/* D�clenche une interruption sur timer dont on d�finit la priorit� (0 (++) --> 15(--)) et la fonction ex�cut�e
ex : Timer_Acive_IT (TIM2, 0x10, fonction_a_execute)
*/
void Timer_Active_IT( TIM_TypeDef *Timer, u8 Priority, void (*IT_function) (void) );

/*
Configure le timer x channel y en pwm avec th en %, et la frequence
*/
void config_pwm (TIM_TypeDef *Timer, u8 channel, float duty_cycle, float duree_us);

/*
Permet de changer la valeur du DC du PWM
Cela requiert d'avoir d�j� fait un config_pwm
*/
void charger_DC_pwm(TIM_TypeDef *Timer, u8 channel, float duty_cycle);

#endif
