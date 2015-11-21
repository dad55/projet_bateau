#ifndef FONCTIONS_BATEAU_H
#define FONCTIONS_BATEAU_H

#include "stm32f10x.h"

/*
  CETTE LIBRARIE INCLUT DES FONCTIONS QUI ONT ETE CREE
	POUR CORRESPONDRE AU CAHIER DES CHARGES DU PROJET
	VOILIER ET DONC, SONT SPECIFIQUE A CE PROJET. ILS N'ONT
	PAS ETE CODES DANS L'OPTIQUE DE FAIRE UNE LIBRAIRIE 
	GENERIQUE
*/



/*
  Cette fonction renvoie en sortie la duree d'une 
  impulsion si le timer a ete configure en mode
  PWM entree voie 1 avec la fonction 
  Timer_Init_PWM_Input (dans timer1234.h)
*/
float lire_duree_imp (TIM_TypeDef *Timer);

/*
  Configuration du codeur incremental
*/
void Init_Codeur (TIM_TypeDef * Timer, int priority);

/*
  Configuration de l'USART pour la liaison HF
*/
void Config_UART_Projet_Bateau(USART_TypeDef * USART);

/*
  Fonction qui retourne la valeur l'angle alpha mesure
	par la girouette
*/
u16 lire_alpha (void);


#endif
