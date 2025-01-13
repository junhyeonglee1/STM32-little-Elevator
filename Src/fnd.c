#include "fnd.h"

extern void spi_data_write(uint8_t data);

extern RTC_TimeTypeDef sTime; //시각 정보
extern volatile int TIM2_elv_fnd_counter;
volatile int sec = 0;
volatile int min = 0;
void fnd_clock_display(void);
void fnd_timer(void);

const uint8_t FND_DIGIT_PATTERN[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};
void fnd_clock_display()
{
    static uint8_t FND_data[4] = {0}; // FND에 출력할 데이터 배열
    FND_data[0] = FND_DIGIT_PATTERN[sTime.Minutes / 10]; // 분의 십의 자리
    if(TIM2_elv_fnd_counter>=500) FND_data[1] = FND_DIGIT_PATTERN[sTime.Minutes % 10]; // 분의 일의 자리
    else FND_data[1] = FND_DIGIT_PATTERN[sTime.Minutes % 10] | 0x80;//가운데 점깜빡이기
    FND_data[2] = FND_DIGIT_PATTERN[sTime.Seconds / 10]; // 초의 십의 자리
    FND_data[3] = FND_DIGIT_PATTERN[sTime.Seconds % 10]; // 초의 일의 자리
    // FND에 데이터 출력
    for (int i = 0; i < 4; ++i) {
            spi_data_write(FND_data[i]); // SPI 통신으로 FND에 데이터 전송
            switch(i) {
                case 0:
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // DIGIT 1 선택
                    break;
                case 1:
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // DIGIT 2 선택
                    break;
                case 2:
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // DIGIT 3 선택
                    break;
                case 3:
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // DIGIT 4 선택
                    break;
                default:
                    break;
            }
            HAL_Delay(1); // 1ms 딜레
            // DIGIT 비활성화
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
        }
}

//void fnd_clock_display(void)
//{
//
//	static int digit_position = 0;
//
//	HAL_GPIO_WritePin(GPIOA, 0b11110000, 1);
//
//
//	switch(digit_position)
//	{
//	case 0:
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0); // common 캐소드
//		HAL_GPIO_WritePin(GPIOA, 0b01110000, 1);
//		spi_data_write(fnd_font[TIM2_elv_fnd_counter /10 % 10]);
//		break;
//	case 1:
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0); // common 캐소드
//		HAL_GPIO_WritePin(GPIOA, 0b10110000, 1);
//		spi_data_write(fnd_font[TIM2_elv_fnd_counter/100 % 10]);
//		break;
//	case 2:
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0); // common 캐소드
//		HAL_GPIO_WritePin(GPIOA, 0b11010000, 1);
//		if(sec % 2 == 0)
//		{
//			spi_data_write(fnd_font[sec%10]); // 가운데 점 깜빡이기
//		}
//		else if(sec % 2 == 1)
//		{
//			spi_data_write(fnd_font[sec%10] | fnd_font[10]);
//		}
//		break;
//	case 3:
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0); // common 캐소드
//		HAL_GPIO_WritePin(GPIOA, 0b11100000, 1);
//		spi_data_write(fnd_font[sec/10 % 6]);
//		break;
//	}
//	digit_position++;
//	digit_position%= 4;
//}
