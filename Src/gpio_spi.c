#include "gpio_spi.h"

extern volatile int TIM2_GPIO_SPI_counter;

void spi_data_write(uint8_t data)
{
	for(uint8_t i = 0;i < 8; i++)
	{
		if(data & 0b10000000)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1); //shift clock high
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
		data = data << 1;
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1); //latch clock
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);
}


void spi_led_main(void)
{
	static uint8_t index = 1;
	uint8_t pattern ;

	if(TIM2_GPIO_SPI_counter >= 200)
	{
		index = !index;
		TIM2_GPIO_SPI_counter = 0;
		if(index) pattern = 0xff;
		else if(!index) pattern = 0x00;
		spi_data_write(pattern);
	}
}
