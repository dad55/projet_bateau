#include "stm32f10x.h"
#include "gpio.h"

char Port_IO_Init_AF_Output ( GPIO_TypeDef *Port, u8 Broche)
{
	char erreur = 0x0;
	
	if (Port == GPIOA)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPAEN;
	else if (Port == GPIOB)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPBEN;
	else if (Port == GPIOC)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPCEN;
	
	if (Broche <= 0x7)
				{
					Port -> CRL = Port -> CRL &~(0xF << (4*Broche));
					Port -> CRL = Port -> CRL | (0xB << (4*Broche));
				}
	else if (Broche <= 0xF)
				{	
					Port -> CRH = Port -> CRH &~(0xF << (4*(Broche-8)));
					Port -> CRH = Port -> CRH | (0xB << (4*(Broche-8)));
			  }
	else
		erreur=0x1;
	return erreur;
}
	
char Port_IO_Init_Output( GPIO_TypeDef * Port, u8 Broche)
{
		
	char erreur = 0x0;
	
	if (Port == GPIOA)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPAEN;
	else if (Port == GPIOB)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPBEN;
	else if (Port == GPIOC)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPCEN;

	
	if (Broche <= 0x7)
				{
					Port -> CRL = Port -> CRL &~(0xF << (4*Broche));
					Port -> CRL = Port -> CRL | (0x1 << (4*Broche));
				}
	else if (Broche <= 0xF)
				{	
					Port -> CRH = Port -> CRH &~(0xF << (4*(Broche-8)));
					Port -> CRH = Port -> CRH | (0x1 << (4*(Broche-8)));
			  }
	else
		erreur=0x1;
	return erreur;
}


char Port_IO_Init_Analog_Input ( GPIO_TypeDef * Port, u8 Broche)
{
	char erreur = 0x0;

	if (Port == GPIOA)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPAEN;
	else if (Port == GPIOB)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPBEN;
	else if (Port == GPIOC)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPCEN;
	
	if (Broche <=0x7)
					Port -> CRL = Port -> CRL &~ (0xF<< (4*Broche));
	else if (Broche <= 0xF)
					Port -> CRH = Port -> CRH &~(0xF << (4*(Broche-8)));
	else
		erreur=0x1;
	return erreur;
}
	
char Port_IO_Init_Input( GPIO_TypeDef * Port, u8 Broche)
{
	char erreur = 0x0;

	if (Port == GPIOA)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPAEN;
	else if (Port == GPIOB)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPBEN;
	else if (Port == GPIOC)
		(RCC->APB2ENR)|= RCC_APB2ENR_IOPCEN;
	
	if (Broche <=0x7)
				{
					Port -> CRL = Port -> CRL &~ (0xF<< (4*Broche));
					Port -> CRL = Port -> CRL | (0x4 << (4*Broche));
				}
	else if (Broche <= 0xF)
				{	
					Port -> CRH = Port -> CRH &~(0xF << (4*(Broche-8)));
					Port -> CRH = Port -> CRH | (0x4 << (4*(Broche-8)));
				}
	else
		erreur=0x1;
	return erreur;
}

void Port_IO_Set(GPIO_TypeDef * Port, u8 Broche)
{

	
	if (Broche <=0xF)
			Port -> ODR = Port -> ODR |(0x1 << Broche);
}

void Port_IO_Reset(GPIO_TypeDef * Port, u8 Broche)
{
	
	if (Broche <=0xF)
			Port -> ODR = Port -> ODR &~(0x1 << Broche);
}

void Port_IO_Blink(GPIO_TypeDef * Port, u8 Broche)
{
	
	if (Broche <=0xF)
			Port -> ODR = Port -> ODR ^(0x1 << Broche);
}

unsigned int Port_IO_Read(GPIO_TypeDef * Port, u8 Broche)
{
	unsigned int etat;
	etat = 0;
	
	if (Broche <=0xF)
	{
		etat = Port -> IDR & (0x1 << Broche);
		etat = etat >> Broche;
	}
	return etat;
}
