 /******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the buzzer driver
 *
 * Author: Ammar Moataz
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_


#include "std_types.h"


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void buzzer_init(void);
void buzzer_start(void);
void buzzer_stop(void);


#endif /* BUZZER_H_ */
