/*!
    \file    main.c
    \brief   led spark with systick

    \version 2025-07-31, V3.3.2, firmware for GD32F4xx
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "main.h"
#include "Image.h"

/*!
    \brief    main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
  systick_config();
	key_init();
	LEDenable();
	Lcd_Init();
	
	Adc_Init();
	
	// 初始化 PWM 和定时器
    PwmMotor_Init();
    basic_timer_config(200, 1000);  // TIMER5 1ms中断
	//振动测试
	//PwmMotor_SetValue(0.5, (uint8_t)500);
	
	//SetLED1(true);
	
	//pwm_config(200,10000);  // 不知道是什么总之暂时用不上,应该是呼吸灯用的？
	//printf("test1114514\n");
	

    while(1) {
		JoystickDir joystick_dir = Joystick_ReadDir();
		//当前界面
		switch(current_screen){
			//：主菜单
			case mainscreen:{
				Lcd_NewFrame();
				//当前光标位置
				switch(current_cursor){
				    case START:{
					    Lcd_DrawImage(0,0,240,280,gImage_start);
						break;
					}
					case MODE_SET:{
					    Lcd_DrawImage(0,0,240,280,gImage_mode);
						break;
					}
					case CONFIG:{
						//暂时没有设置功能
					    Lcd_DrawImage(0,0,240,280,gImage_main0);
						break;
					}
				}
				Lcd_ShowFrame();
				if(A_pressed || (joystick_dir==JOY_UP)){//上
					if(current_cursor==0) current_cursor=2;
					else current_cursor--;
					A_pressed = false;
				}
				if(B_pressed || (joystick_dir==JOY_DOWN)){//下
					if(current_cursor==2) current_cursor=0;
					else current_cursor++;
					B_pressed = false;
				}
				if(R_pressed || (joystick_dir==JOY_RIGHT)){//确认
					if(current_cursor==START) current_screen = game_interface;
					//其他两个界面没做
					R_pressed = false;
				}
				if(L_pressed || (joystick_dir==JOY_LEFT)){//返回（这里没用
					L_pressed = false;
				}
				break;
			}
			case game_interface:{
				//游戏初始化
				if(!game_has_init){
					Lcd_NewFrame();
		      game_init();
		      Lcd_ShowFrame();
					while(!game_over_flag){
						JoystickDir joystick_dir = Joystick_ReadDir();
						if(A_pressed || (joystick_dir==JOY_UP)){//上
					    snake.dir = UP;
					    A_pressed = false;
						}
				    if(B_pressed || (joystick_dir==JOY_DOWN)){//上
					    snake.dir = DOWN;
					    B_pressed = false;
						}
				    if(R_pressed || (joystick_dir==JOY_RIGHT)){
					    snake.dir = RIGHT;
					    R_pressed = false;
						}
				    if(L_pressed || (joystick_dir==JOY_LEFT)){
					    snake.dir = LEFT;
					    L_pressed = false;
						}
						snake_erase();
						snake_move();
						snake_draw();
						Lcd_ShowFrame();
						delay_1ms(250);
					}
					if(game_over_flag){
						//delay_1ms(2000);
						current_screen = game_fail;
						game_over_flag = 0;
						game_has_init = 0;
					}
		    }
			}
			case game_fail:{
				char str[16];
				sprintf(str, "%d", score);
				Lcd_DrawLine(0,19,240,19,COLOR_WHITE);//擦除分割线
				Lcd_DrawFilledRectangle(100, 0, 58, 20, COLOR_WHITE);//擦除上方分数栏
				
				switch(current_fail_cursor){
					case(back):{
						Lcd_DrawImage(0,25,240,210,gImage_fail_back);
				    Lcd_ShowString(118,100,str,COLOR_BLACK,16);
						break;
					}
					case(restart):{
						Lcd_DrawImage(0,25,240,210,gImage_fail_restart);
				    Lcd_ShowString(118,100,str,COLOR_BLACK,16);
						break;
					}
				}
				Lcd_ShowFrame();
				
				JoystickDir joystick_dir = Joystick_ReadDir();
				if(A_pressed || (joystick_dir==JOY_UP)){//上
					if(current_fail_cursor==0) current_fail_cursor=1;
					else current_fail_cursor--;
					A_pressed = false;
				}
				if(B_pressed || (joystick_dir==JOY_DOWN)){//下
					if(current_fail_cursor==1) current_fail_cursor=0;
					else current_fail_cursor++;
					B_pressed = false;
				}
				if(R_pressed || (joystick_dir==JOY_RIGHT)){//确认
					if(current_fail_cursor==0) current_screen = mainscreen;
					else current_screen = game_interface;
					R_pressed = false;
				}
				break;
			}
		}
  }
}

#ifdef GD_ECLIPSE_GCC
/* retarget the C library printf function to the USART, in Eclipse GCC environment */
int __io_putchar(int ch)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
#else
/* retarget the C library printf function to the USART */

#endif /* GD_ECLIPSE_GCC */
