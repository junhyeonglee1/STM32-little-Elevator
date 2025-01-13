/*
 * i2c_lcd.c
 *
 *  Created on: 2019. 9. 4.
 *      Author: k
 */
#include "main.h"

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "i2c_lcd.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart3;

extern int current_floor;
extern int f1_state;
extern int f2_state;
extern int f3_state;
extern int f4_state;
extern char stepmotor_state;
extern int step_motor_direction;
extern char prev_stepmotor_state;
extern int prev_stop_flag;
extern int prev_floor;
extern int stop_flag;

void elevator_lcd_main(void)
{
	if(prev_stepmotor_state != stepmotor_state || prev_stop_flag != stop_flag || prev_floor != current_floor)
	{
		move_cursor(0,0);
		lcd_string("current floor:");

		move_cursor(0,15);
		lcd_data(current_floor + '0');

		move_cursor(1,0);
		if(f1_state) 	   lcd_data('1');
		else if(!f1_state) lcd_data(' ');
		move_cursor(1,2);
		if(f2_state) 	   lcd_data('2');
		else if(!f2_state) lcd_data(' ');
		move_cursor(1,4);
		if(f3_state) 	   lcd_data('3');
		else if(!f3_state) lcd_data(' ');
		move_cursor(1,6);
		if(f4_state) 	   lcd_data('4');
		else if(!f4_state) lcd_data(' ');
	}
}


void i2c_lcd_main(void){

	uint8_t value=0;
	//i2c_lcd_init();


	while(1){
		move_cursor(0,0);
		lcd_string("Hello World!!!");
		move_cursor(1,0);
		lcd_data(value + '0');
		value++;
		if(value>9)value=0;
		HAL_Delay(500);
	}
}

void lcd_command(uint8_t command){

	uint8_t high_nibble, low_nibble;
	uint8_t i2c_buffer[4];
	high_nibble = command & 0xf0;
	low_nibble = (command<<4) & 0xf0;
	i2c_buffer[0] = high_nibble | 0x04 | 0x08; //en=1, rs=0, rw=0, backlight=1
	i2c_buffer[1] = high_nibble | 0x00 | 0x08; //en=0, rs=0, rw=0, backlight=1
	i2c_buffer[2] = low_nibble  | 0x04 | 0x08; //en=1, rs=0, rw=0, backlight=1
	i2c_buffer[3] = low_nibble  | 0x00 | 0x08; //en=0, rs=0, rw=0, backlight=1
	while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
			i2c_buffer, 4, 100)!=HAL_OK){
		//HAL_Delay(1);
	}
	return;
}
void lcd_data(uint8_t data){

	uint8_t high_nibble, low_nibble;
	uint8_t i2c_buffer[4];
	high_nibble = data & 0xf0;
	low_nibble = (data<<4) & 0xf0;
	i2c_buffer[0] = high_nibble | 0x05 | 0x08; //en=1, rs=1, rw=0, backlight=1
	i2c_buffer[1] = high_nibble | 0x01 | 0x08; //en=0, rs=1, rw=0, backlight=1
	i2c_buffer[2] = low_nibble  | 0x05 | 0x08; //en=1, rs=1, rw=0, backlight=1
	i2c_buffer[3] = low_nibble  | 0x01 | 0x08; //en=0, rs=1, rw=0, backlight=1
	while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
			i2c_buffer, 4, 100)!=HAL_OK){
		//HAL_Delay(1);
	}
	return;
}
void i2c_lcd_init(void){

	lcd_command(0x33);
	lcd_command(0x32);
	lcd_command(0x28);	//Function Set 4-bit mode
	lcd_command(DISPLAY_ON);
	lcd_command(0x06);	//Entry mode set
	lcd_command(CLEAR_DISPLAY);
	HAL_Delay(2);
}
void lcd_string(uint8_t *str){
	while(*str)lcd_data(*str++);
}
void move_cursor(uint8_t row, uint8_t column){
	lcd_command(0x80 | row<<6 | column);
	return;
}











