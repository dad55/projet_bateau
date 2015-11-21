#ifndef TIMER_H
#define TIMER_H

#include "stm32f10x.h"

extern void (*adr_fonction_it1) (void);
extern void (*adr_fonction_it2) (void);
extern void (*adr_fonction_it3) (void);
extern void (*adr_fonction_it4) (void);


void Enable_CLK_Timer1234(TIM_TypeDef *Timer);

/* Configure un timer avec la durée voulue en us.
renvoie la durée réellement comptée
ex : duree_reelle = Timer_1234_Init (TIM3, 500);
*/
float Timer_1234_Init(TIM_TypeDef *Timer, float Duree_us );


/* Déclenche une interruption sur timer dont on définit la priorité (0 (++) --> 15(--)) et la fonction exécutée
ex : Timer_Acive_IT (TIM2, 0x10, fonction_a_execute)
*/
void Timer_Active_IT( TIM_TypeDef *Timer, u8 Priority, u8 channel, void (*IT_function) (void) );

/*
Configure le timer x channel y en pwm avec th en %, et la frequence
*/
void config_pwm (TIM_TypeDef *Timer, u8 channel, float duty_cycle);

/*
Permet de changer la valeur du DC du PWM
Cela requiert d'avoir déjà fait un config_pwm
*/
void charger_DC_pwm(TIM_TypeDef *Timer, u8 channel, float duty_cycle);

/*
	Mode PWM en entrée
	Remarque : A chaque front montant de counteur passe à 0
*/

void Timer_Init_PWM_Input(TIM_TypeDef * Timer, u8 voie, int duree_impulsion_max);

/*
	Fonction à mettre obligatoirement en fin de gestion d'une interruption
	si l'interruption update est active
*/
void Timer_IT_Update_OFF(TIM_TypeDef * Timer);

/*
	Fonction à mettre obligatoirement en fin de gestion d'une interrution
	si l'interruption Capture/Compare channel est active
*/
void Timer_IT_CCx_OFF(TIM_TypeDef * Timer, u8 channel);



#endif
