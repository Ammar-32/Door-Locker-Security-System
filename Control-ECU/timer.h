/******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.h
 *
 * Description: Header file for the AVR Timer driver
 *
 * Author: Ammar Moataz
 *
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define TIMER0_MAXIMUM_VALUE 255

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum{
	OVERFLOW=0x80, CTC=0x88, PWM_PHASE_CORRECT=0x40, FAST_PWM=0x48
}TIMER0_Mode;

typedef enum{
	OC0_DISCONNECTED, OC0_TOGGLE=0x10, OC0_CLEAR=0x20, OC0_SET=0X30
}TIMER0_Compare_Match_Mode;

typedef enum
{
	NO_CLK, F_CPU_1, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024, EXTERNAL_CLK_FALLING_EDGE, EXTERNAL_CLK_RISING_EDGE
}TIMER0_Clock;

typedef enum{
	OVERFLOW_INT_ENABLE=0x1, CTC_INT_ENABLE=0x2
}TIMER0_INT_Mask;

typedef struct{
	TIMER0_Mode mode;
	TIMER0_Compare_Match_Mode compareMatchOutputMode;
	TIMER0_Clock clockPrescalar;
	TIMER0_INT_Mask interruptMask;
	uint8 initialValue;
	uint8 compareValue;
}TIMER0_Config;
/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
void TIMER0_Init(TIMER0_Config* config);
void TIMER0_setValue(uint8 newValue);
void TIMER0_clearValue(void);
void TIMER0_setCallback(void(*functionPtr)(void));
void TIMER0_deinit(void);

#endif /* GPIO_H_ */
