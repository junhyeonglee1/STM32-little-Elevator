#include "stepmotor.h"
#include "button.h"


extern void delay_us(unsigned int us);
void stepmotor_drive(int step);
void set_rpm(int rpm);
void stepmotor_forward(void);
void stepmotor_reverse(void);
void stepmotor_button_control(void);
void elevator_state_debug(void);
void elevator_stepmotor_movement(void);

extern volatile int TIM2_elv_stop_counter;
extern volatile int TIM2_elv_debug_counter;
extern char button0_state;
extern char stepmotor_state;
extern int current_floor;
extern int f1_state;
extern int f2_state;
extern int f3_state;
extern int f4_state;
int stop_flag = 0;
int step_motor_direction = FORWARD;
/*
 *  RPM(Revolutions Per Minute) : 분당 회전수
 *  1분 : 60sec : 1,000,000us(1sec) x 60 = 60,000,000us
 *  1초 : 1000ms -> 1ms(1000us) x 1000ms => 1,000,000us
 *  1바퀴 회전 : 4096 스탭 필요
 *  4096 / 8 ---> 512sequence : 360도 회전
 *  1sequence(8step) : 0.70312도
 *  0.70312 x 512sequences = 360도
 *  --- RPM 조정 함수 ---
 *  60,000,000us / 4096 / RPM(1~13)
 *  13회전 : 60,000,000us / 4096 / 13 ==> 스탭과 step 간격 time 1126us
 *  1126us x 4096(1회전시 필요 스탭) = 4,615,384us
 *  							= 4615ms
 *  							= 4.6sec
 *  60sec / 4.6(1회전시의 소요 초) ==> 13번 회전
 */

void elevator_state_debug(void)
{
	if(TIM2_elv_debug_counter>=1000)
	{
		TIM2_elv_debug_counter = 0;
		printf("curr_floor: %d\n",current_floor);
		printf("f1_state : %d\n", f1_state);
		printf("f2_state : %d\n", f2_state);
		printf("f3_state : %d\n", f3_state);
		printf("f4_state : %d\n", f4_state);
		printf("stopflag : %d\n", stop_flag);
	}
}

void elevator_stepmotor_movement(void)
{
	static int b = 0;

	if((f1_state || f2_state || f3_state || f4_state) && !stop_flag)
	{
		stepmotor_state = RUN;
	}
	else stepmotor_state = STOP;

	if(stop_flag)
	{
		if(TIM2_elv_stop_counter >= 3000)
		{
			stop_flag = 0;
		}
	}
	else;

	if(stepmotor_state == RUN)
	{
		stepmotor_drive(b);
		set_rpm(13);
		switch(current_floor)
		{
		case 1:
			step_motor_direction = FORWARD;
			b++;
			b%=8;
			if(f1_state) // 지금있는 층 누르면 취소됨
			{
				f1_state = 0;
			}
			break;
		case 2:
			if(f1_state)
			{
				f3_state = 0; //내려가는 도중에는 올라가는층 누르면 취소
				f4_state = 0;
				f2_state = 0;
				step_motor_direction = REVERSE;
				b--;
				if(b<0) b=7;
			}
			else if(f3_state || f4_state)
			{
				f1_state = 0; //올라가는 도중에 내려가는 층 누르면 취소
				step_motor_direction = FORWARD;
				b++;
				b%=8;
			}
			if(f2_state) // 지금있는 층 누르면 취소됨
			{
				f2_state = 0;
			}
			break;
		case 3:
			if(f1_state || f2_state)
			{
				f3_state = 0; //내려가는 도중에는 올라가는층 누르면 취소
				f4_state = 0;
				step_motor_direction = REVERSE;
				b--;
				if(b<0) b=7;
			}
			else if(f4_state)
			{
				f1_state = 0;
				f2_state = 0;
				f3_state = 0; //올라가는 도중에 내려가는 층 누르면 취소
				step_motor_direction = FORWARD;
				b++;
				b%=8;
			}
			break;
		case 4:
			step_motor_direction = REVERSE;
			b--;
			if(b<0) b=7;
			if(f4_state) // 지금있는 층 누르면 취소됨
			{
				f4_state = 0;
			}
			break;
		}
	}
	else if(stepmotor_state == STOP)
		;
}

void stepmotor_button_control(void)
{
	static int k = 0;

	if(button0_state)
	{
		stepmotor_drive(k);
		set_rpm(13);
		if(stepmotor_state == FORWARD)
		{
			k++;
			k%=8;
		}
		else if(stepmotor_state == REVERSE)
		{
			k--;
			if(k<0)
			{
				k=7;
			}
		}
	}
	else ;
}

void set_rpm(int rpm) // rpm : 1~13
{
	delay_us(60000000/4096/rpm);
	// 최대 speed 기준(13) : delay_us(1126);
}

void stepmotor_forward(void)
{
	for(int i=0;i < 512; i++) // 시계방향 1바퀴 회전
	{
		for(int j=0; j<8; j++)
		{
			stepmotor_drive(j);
			set_rpm(13); //1126us만큼 wait
		}
	}
}


void stepmotor_reverse(void)
{
	for(int i=0;i < 512; i++) // 반시계방향 1바퀴 회전
	{
		for(int j=7; j>=0; j--)
		{
			stepmotor_drive(j);
			set_rpm(13);
		}
	}
}

void stepmotor_drive(int step)
{
	switch(step)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		break;
	}
}
