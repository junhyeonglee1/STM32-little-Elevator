#include "uart2.h"
#include <string.h>
extern UART_HandleTypeDef huart2;
extern uint8_t rx_data;

extern void led_all_on(void);
extern void led_all_off(void);
extern void led_on_up(void);
extern void led_on_down(void);
extern void led_keepon_up(void);
extern void led_keepon_down(void);
extern void led_flower_on(void);
extern void led_flower_off(void);
extern void set_rtc(char *date_time);
void pc_command_processing(void);
extern void dotmatrix_main_test(void);
extern int dotmatrix_main(void);

uint8_t rx_buff[20][40];
volatile static int front = 0;
volatile static int rear = 0;
// move from Drivers\STM32F4xx_HAL_Driver\Src\HAL_UART_RxCpltCallback of stm32f4xx_hal_uart to here
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static int i = 0;

	if(huart == &huart2)
	{
		unsigned char data;
		data = rx_data;
		if(data == '\r' || data =='\n')
		{
			rx_buff[rear][i] = '\0';
			i=0;
			rear++;
			rear %= 20;
		}
		else
		{
			rx_buff[rear][i++] = data;
		}
		HAL_UART_Receive_IT(&huart2, &rx_data, 1);
	}
}

void pc_command_processing(void)
{
	if(front != rear)
	{
		printf("%s\n",rx_buff[front]); // rx_buff[front]0번째방의 주소
		if(strncmp(rx_buff[front], "led_all_on" ,strlen("led_all_on"))==0)
		{
			led_all_on();
		}
		else if(strncmp(rx_buff[front],"settime",strlen("settime"))==0)
		{
			//set_rtc(&rx_buff[front][7]);
		}
		else if(strncmp(rx_buff[front],"DOTDIS",strlen("DOTDIS"))==0)
		{
			make_dot(&rx_buff[front][7]);
		}
		front++;
		front %= 20;
	}
}
