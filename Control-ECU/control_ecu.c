// Includes
#include "timer.h"
#include "uart.h"
#include "dcmotor.h"
#include "micro_config.h"
#include "buzzer.h"
#include "external_eeprom.h"

// Defines
#define DELAY_Keypad 600
#define DELAY_UART 130

// Prototypes
void recievePassword(uint8 password[]);
uint8 verifyPassword(uint8 password[], uint8 confirmPassword[]);
void openDoor(void);
void timerHandler(void);
void checkFalsePasswords(void);
void storePasswordEEPROM(uint8 password[]);
void readPasswordEEPROM(uint8 password[]);

// Global Variables
int g_counter = 0;
uint8 falsePasswordsCounter;

// Structs
UART_Config UConfig = {ASYNCH,ENABLED_EVEN,BIT_1};// Asynchronous with even parity and only one stop bit
TIMER0_Config TConfig = {OVERFLOW, 0, F_CPU_1024, OVERFLOW_INT_ENABLE, 3, 0};

// Main
int main(void)
{
	SREG |= (1<<7);
	uint8 password[4], confirmPassword[4];
	uint8 isVerified, command;
	DCMOTOR_init();
	buzzer_init();
	TIMER0_setCallback(timerHandler);
	UART_init(&UConfig);
	do{
		recievePassword(password);

		recievePassword(confirmPassword);

		isVerified = verifyPassword(password, confirmPassword);
		UART_sendByte(isVerified);

		_delay_ms(DELAY_UART);
	}while(isVerified == 0);

	// Store pw in eeprom
	storePasswordEEPROM(password);


	while(1)
	{
		uint8 passwordEEPROM[4];
		readPasswordEEPROM(passwordEEPROM); // read pw and store it in variable
		command = UART_recieveByte();
		_delay_ms(DELAY_UART);
		if(command == '+'){
			recievePassword(password);

			if(verifyPassword(password, passwordEEPROM)){
				falsePasswordsCounter = 0;
				UART_sendByte(1);
				_delay_ms(DELAY_UART);
				openDoor();
			}else{
				UART_sendByte(0);
				_delay_ms(DELAY_UART);
				falsePasswordsCounter++;
				UART_sendByte(falsePasswordsCounter);
				_delay_ms(DELAY_UART);
				checkFalsePasswords();

			}
		}else if(command == '-'){
			recievePassword(password);


			isVerified = verifyPassword(password, passwordEEPROM);
			_delay_ms(DELAY_UART);
			UART_sendByte(isVerified);
			if(isVerified){
				falsePasswordsCounter = 0;
				recievePassword(password);


				recievePassword(confirmPassword);

				isVerified = verifyPassword(password, confirmPassword);
				_delay_ms(DELAY_UART);
				UART_sendByte(isVerified);

				if(isVerified){
					// store new password in eeprom
					storePasswordEEPROM(password);
				}else{
					// do nothing
				}

			}else{
				falsePasswordsCounter++;
				UART_sendByte(falsePasswordsCounter);
				_delay_ms(DELAY_UART);
				checkFalsePasswords();
			}
		}
	}
}

// Functions
void recievePassword(uint8 password[])
{
	for (uint8 i = 0; i < 4; i++) {
		password[i]=UART_recieveByte();
	}
	_delay_ms(DELAY_UART);

}

uint8 verifyPassword(uint8 password[], uint8 confirmPassword[]){
	uint8 i;
	for(i = 0; i <4 ; i++){
		if(password[i]!= confirmPassword[i]){
			return 0;
		}
	}
	return 1;
}

void openDoor(){
	TIMER0_Init(&TConfig);
	DCMOTOR_rotate(CLOCKWISE);
	while(g_counter < 465);
	g_counter = 0;
	DCMOTOR_rotate(STOP);

	while(g_counter < 93);
	g_counter = 0;
	DCMOTOR_rotate(ANTICLOCKWISE);

	while(g_counter < 465);
	DCMOTOR_rotate(STOP);
	TIMER0_deinit();
	g_counter = 0;
}

void timerHandler(void){
	TIMER0_setValue(3);
	g_counter++;
}

void checkFalsePasswords(void){
	if(falsePasswordsCounter >= 3){
		falsePasswordsCounter = 0;
		TIMER0_Init(&TConfig);
		buzzer_start();
		while(g_counter < 465);
		buzzer_stop();
		TIMER0_deinit();
		g_counter = 0;
	}
}

void storePasswordEEPROM(uint8 password[]){
	uint8 i;
	for(i=0;i<4;i++)
	{
		EEPROM_writeByte((0X0090+i), password[i]);
		_delay_ms(DELAY_UART);
	}
}

void readPasswordEEPROM(uint8 password[]){
	for (uint8 i = 0; i < 4; i++)
	{
		EEPROM_readByte( (0x0090 + i), (password+i));
		_delay_ms(100);
	}
}
