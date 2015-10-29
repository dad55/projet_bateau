#ifndef GPIO_H
#define GPIO_H

#include "stm32f10x.h"
/* Configure la broche x du GPIO y en alternate function en sortie push pull*/
char Port_IO_Init_AF_Output ( GPIO_TypeDef *Port, u8 Broche);

/* Met en sortie 1 broche du port GPIO
renvoie 0 si ok , -1 si erreur
ex : erreur = Port_IO_Init_Output (GPIOB,5);
*/
char Port_IO_Init_Output( GPIO_TypeDef * Port, u8 Broche);

/* Met en entree analogique 1 broche du port GPIO
renvoie 0 si ok , -1 si erreur
ex : erreur = Port_IO_Init_Analog_Input (GPIOB,5);
*/
char Port_IO_Init_Analog_Input ( GPIO_TypeDef * Port, u8 Broche);

/* Met en entree 1 broche du port GPIO
renvoie 0 si ok , -1 si erreur
ex : erreur = Port_IO_Init_Input (GPIOB,5);
*/
char Port_IO_Init_Input( GPIO_TypeDef * Port, u8 Broche);

/* Met à '1' une broche du port GPIO
ex : Port_IO_Set (GPIOB,5);
Nécessite d'avoir mis le port en sortie
*/
void Port_IO_Set(GPIO_TypeDef * Port, u8 Broche);

/* Met à '0' une broche du port GPIO
ex : Port_IO_Reset (GPIOB,5);
Nécessite d'avoir mis le port en sortie
*/
void Port_IO_Reset(GPIO_TypeDef * Port, u8 Broche);

/* Change l'état d'une broche du port GPIO (XOR 1)
ex : Port_IO_Blink (GPIOB,5);
Nécessite d'avoir mis le port en sortie
*/
void Port_IO_Blink(GPIO_TypeDef * Port, u8 Broche);

/* Permet de lire une broche du port GPIO
renvoie l'etat du bit n sur le bit 0
ex : retour = Port_IO_Read (GPIOB,5);
Nécessite d'avoir mis le port en entree
*/
unsigned int Port_IO_Read(GPIO_TypeDef * Port, u8 Broche);




#endif
