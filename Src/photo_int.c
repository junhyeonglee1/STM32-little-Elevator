#include "photo_int.h"

extern char button0_state;
extern char stepmotor_state;
extern int current_floor;
extern int f1_state;
extern int f2_state;
extern int f3_state;
extern int f4_state;
extern int stop_flag;
extern volatile int TIM2_elv_stop_counter;

//move from Drivers\STM32F4xx_HAL_Driver\Src\stm32f4xx_hal_gpio.c to here
//external interrupt call back function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case GPIO_PIN_0: //PA.0
		current_floor = 1;
		if(f1_state)
		{
			stop_flag = 1;
			TIM2_elv_stop_counter = 0;
			f1_state = 0;
		}
		break;
	case GPIO_PIN_1: //PA.1
		current_floor = 2;
		if(f2_state)
		{
			stop_flag = 1;
			TIM2_elv_stop_counter = 0;
			f2_state = 0;
		}
		break;
	case GPIO_PIN_4: //PA.4
		current_floor = 3;
		if(f3_state)
		{
			stop_flag = 1;
			TIM2_elv_stop_counter = 0;
			f3_state = 0;
		}
		break;
	case GPIO_PIN_5: //PA.5
		current_floor = 4;
		if(f4_state)
		{
			stop_flag = 1;
			TIM2_elv_stop_counter = 0;
			f4_state = 0;
		}
		break;
	}
}
