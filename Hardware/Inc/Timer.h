#ifndef TIMER_H
#define TIMER_H

#include "gd32f4xx.h"
#include "systick.h"
#include "gd32f4xx_rcu.h"
#include <stdio.h>
#include "LED.h"
#include "UsartTest.h"


//TIMER5
#define  BSP_TIMER_RCU    RCU_TIMER5        // ¶¨ʱƷʱ֓
#define  BSP_TIMER        TIMER5            // ¶¨ʱƷ
#define  BSP_TIMER_IRQ    TIMER5_DAC_IRQn   // ¶¨ʱƷ֐¶ύ

#define BSP_TIMER_IRQHandler TIMER5_DAC_IRQHandler // ¶¨ʱƷ֐¶Ϸþαº¯ʽ;

void basic_timer_config(uint16_t pre,uint16_t per);

#endif