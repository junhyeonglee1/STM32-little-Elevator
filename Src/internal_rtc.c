#include "main.h"  //GPIO , HAL 정보 //
#include <string.h>
#include <stdlib.h> // atoi

void get_rtc_date_time(void);
void set_rtc(char *date_time);
unsigned char bcd_2_dec(unsigned char byte);
unsigned char dec_2_bcd(unsigned char byte);
extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);

extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTime = {0}; //시각 정보
RTC_DateTypeDef sDate = {0}; //날짜 정보
//uint8_t Year;
//예) 24년의 Year에 저장된 data format;
// 7654 3210
// ---- ----
// 0010 0100
//   2   4
// ==> 24

unsigned char bcd_2_dec(unsigned char byte)
{
	unsigned char high, low;

	low = byte & 0x0f; // 상위4비트는 무시하고 하위 4비트만 취함
	high = (byte >> 4) * 10; // 상위4비트 취함
	return (high+low);
}
//10진수를 BCD포멧으로 변환
//10진수 예) 24 ==> 0010 0100
unsigned char dec_2_bcd(unsigned char byte)
{
	unsigned char high, low;

	low = (byte/10) << 4; // 상위 4비트
	high = byte % 10;  // 하위 4비트
	return (high+low);
}

// RTC에서 날짜와 시각정보를 읽어오는 함수
void get_rtc_date_time(void)
{
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}
//시각 보정 기능
// setrtc240524141800  (2024-05-24 14:18:00)
// 20240524141800를 넘겨받는다. call by reference
void set_rtc(char *date_time)
{
	char yy[4], mm[4], dd[4]; // date
	char hh[4], min[4], ss[4]; //time

	strncpy(yy,date_time,2);
	strncpy(mm,date_time+2,2); //date_time+2 == &date_time[2]
	strncpy(dd,date_time+4,2);

	strncpy(hh,date_time+6,2);
	strncpy(min,date_time+8,2);
	strncpy(ss,date_time+10,2);
	//1. ascii to int 2. int to BCD 3. RTC적용
	sDate.Year = dec_2_bcd(atoi(yy));
	sDate.Month = dec_2_bcd(atoi(mm));
	sDate.Date = dec_2_bcd(atoi(dd));

	sTime.Hours = dec_2_bcd(atoi(hh));
	sTime.Minutes = dec_2_bcd(atoi(min));
	sTime.Seconds = dec_2_bcd(atoi(ss));

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}
