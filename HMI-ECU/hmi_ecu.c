
// Includes
#include "timer.h"
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "micro_config.h"

// Definitions
#define DELAY_Keypad 500
#define DELAY_UART 130

// Prototypes
void enterPassword(uint8 password[]);
void sendPassword( uint8 password[]);
void timerHandler(void);
void openDoor(void);
void securityAlarm(void);


// Global Variables
int g_counter = 0;

// Configs
UART_Config UConfig = {ASYNCH,ENABLED_EVEN,BIT_1}; // Asynchronous with even parity and only one stop bit
TIMER0_Config TConfig = {OVERFLOW, 0, F_CPU_1024, OVERFLOW_INT_ENABLE, 3, 0};

// Main Function
int main(void){
	SREG |= (1<<7);

	LCD_init();
	UART_init(&UConfig);



	uint8 password[4], confirmPassword[4];
	uint8 isVerified, command;

	TIMER0_setCallback(timerHandler);

	do{
		LCD_clearScreen();
		LCD_displayString("Enter Password");

		// get password from keypad and send it with UART to Control ECU
		enterPassword(password);
		_delay_ms(DELAY_UART);
		sendPassword(password);

		// confirm password and send it with UART to Control ECU
		LCD_clearScreen();
		LCD_displayString("Confirm Password");
		enterPassword( confirmPassword );
		_delay_ms(DELAY_UART);
		sendPassword( confirmPassword );
		_delay_ms(DELAY_UART);

		// recieve verification byte from Control ECU
		// if password match -> 1
		// doesnt match -> 0
		isVerified = UART_recieveByte();
		_delay_ms(DELAY_UART);

		if(isVerified){
			LCD_clearScreen();
			LCD_displayString("Password Set");
		}else{
			LCD_clearScreen();
			LCD_displayString("Wrong Password");
		}

		_delay_ms(2500);

	}
	while(isVerified == 0);

	while(1){
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "- :Change PW");
		LCD_displayStringRowColumn(1, 0, "+ :Open Door");
		command = KEYPAD_getPressedKey();
		_delay_ms(DELAY_Keypad);
		UART_sendByte(command);
		_delay_ms(DELAY_UART);
		if(command == '+'){
			LCD_clearScreen();
			LCD_displayString("Enter Password");
			enterPassword(password);
			_delay_ms(DELAY_UART);
			sendPassword(password);
			isVerified = 0;
			isVerified = UART_recieveByte();
			if(isVerified){
				openDoor();
			}else{
				uint8 falsePasswordCounter = UART_recieveByte();
				if(falsePasswordCounter >=3){
					securityAlarm();
				}else{
					LCD_clearScreen();
					LCD_displayString("Wrong Password");
					_delay_ms(2500);
				}

			}

		}
		else if(command == '-'){
			LCD_clearScreen();
			LCD_displayString("Enter Current PW");
			enterPassword(password);
			_delay_ms(DELAY_UART);
			sendPassword(password);
			isVerified = 0;
			isVerified = UART_recieveByte();
			if(isVerified){
				LCD_clearScreen();
				LCD_displayString("Enter New PW");
				enterPassword(password);
				_delay_ms(DELAY_UART);
				sendPassword(password);
				LCD_clearScreen();
				LCD_displayString("Confirm New Password");
				enterPassword( confirmPassword );
				_delay_ms(DELAY_UART);
				sendPassword( confirmPassword );
				_delay_ms(DELAY_UART);
				isVerified = 0;
				isVerified = UART_recieveByte();
				if(isVerified){
					LCD_clearScreen();
					LCD_displayString("Password Changed!");
					_delay_ms(2500);
				}else{
					LCD_clearScreen();
					LCD_displayString("Unmatching PW");
					_delay_ms(2500);
				}
			}else{
				uint8 falsePasswordCounter = UART_recieveByte();
				if(falsePasswordCounter >=3){
					securityAlarm();
				}else{
					LCD_clearScreen();
					LCD_displayString("Wrong Password");
					_delay_ms(2500);
				}
			}
		}
	}
}

// Functions
void enterPassword(uint8 password[] )
{
	LCD_moveCursor(1, 5);
	for (uint8 i = 0; i < 5; i++) {
		while ((KEYPAD_getPressedKey() != 13) && (i == 4));  // POLLING UNTIL ON BUTTON PRESSED

		password[i] = KEYPAD_getPressedKey();
		_delay_ms(DELAY_Keypad);				//DELAY SO KEYPAD DOESN'T REPEAT
		LCD_displayCharacter('*');
	}
	_delay_ms(DELAY_Keypad);
}


void sendPassword( uint8 password[] )
{
	for (uint8 i=0 ; i<4 ; i++)
	{
		UART_sendByte( password[i] );
		_delay_ms(DELAY_UART);
	}
}

void openDoor(void){
	TIMER0_Init(&TConfig);
	LCD_clearScreen();
	LCD_displayString("Opening Door...");
	while(g_counter < 465);
	g_counter = 0;
	LCD_clearScreen();
	LCD_displayString("Door Open !");
	while(g_counter < 93);
	g_counter = 0;

	LCD_clearScreen();
	LCD_displayString("Closing Door...");
	while(g_counter < 465);

	TIMER0_deinit();
	g_counter = 0;
}

void securityAlarm(void){
	TIMER0_Init(&TConfig);
	LCD_clearScreen();
	LCD_displayString("SECURITY ALARM!");
	while(g_counter < 465);
	TIMER0_deinit();
	g_counter = 0;
}

void timerHandler(void){
	TIMER0_setValue(3);
	g_counter++;
}

