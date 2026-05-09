#include "Score.h"
#include <stdio.h>
#include "BasicDrawing.h"
#include "Image.h"

volatile int score = 0;
void score_init(void) {
    score = 0;
}

void score_update(int value) {
    score += value;
	score_draw();
}

void score_draw(void) {
    char str[16];
    sprintf(str, "%d", score);
    //清除旧分数，绘制新分数
	Lcd_DrawImage(100,0,58,20,gImage_score);
	Lcd_DrawLine(0,19,240,19,COLOR_BLACK);
	Lcd_ShowString(122,1,str,COLOR_BLACK,16);
}