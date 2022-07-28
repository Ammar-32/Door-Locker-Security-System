/******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Source file for the timer module driver for AVR microcontrollers
 *
 * Author: Ammar Moataz
 *
 *******************************************************************************/


/*******************************************************************************
 *                       Includes                                    		   *
 *******************************************************************************/

#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                      Global variable		                                   *
 *******************************************************************************/

void (*g_TIMER0_callBackPtr)(void);

/*******************************************************************************
 *                      ISR Declaration		                                   *
 *******************************************************************************/
/* Timer0 overflow ISR */
ISR(TIMER0_OVF_vect) {
	if (g_TIMER0_callBackPtr!=NULL_PTR)
	(*g_TIMER0_callBackPtr)();
}

/* Timer0 compare match ISR */
ISR(TIMER0_COMP_vect) {
	if (g_TIMER0_callBackPtr!=NULL_PTR)
	(*g_TIMER0_callBackPtr)();
}
/*******************************************************************************
 *                      Function Declarations                                  *
 *******************************************************************************/

void TIMER0_Init(TIMER0_Config* config){
	TCNT0 = config->initialValue;
	TIMSK = config->interruptMask;
	OCR0 = config->compareValue;
	TCCR0 |= config->mode | config->compareMatchOutputMode | config->clockPrescalar;
}

void TIMER0_setValue(uint8 newValue) {
	TCNT0 = newValue;
}

void TIMER0_clearValue(void) {
	TCNT0 = 0;
}

void TIMER0_setCallback(void (*functionPtr)(void)) {
	g_TIMER0_callBackPtr = functionPtr;
}

void TIMER0_deinit(void) {
	TCNT0 = 0;
	TIMSK &= 0xFC;
	OCR0 = 0;
	TCCR0 = 0;
}
