#include "Key.h"
#include <stdbool.h>

/* 按键状态标志 */
volatile bool L_pressed = false;
volatile bool R_pressed = false;
volatile bool A_pressed = false;
volatile bool B_pressed = false;

/* 按键锁，防止连续触发 */
static uint8_t L_lock = 0;
static uint8_t R_lock = 0;
static uint8_t A_lock = 0;
static uint8_t B_lock = 0;

/* 防抖延时(ms) */
#define DEBOUNCE_MS 5

/* 按键定义 */
#define KEY_L_PORT GPIOA
#define KEY_L_PIN  GPIO_PIN_0
#define KEY_L_EXTI_LINE EXTI_0

#define KEY_R_PORT GPIOB
#define KEY_R_PIN  GPIO_PIN_2
#define KEY_R_EXTI_LINE EXTI_2

#define KEY_A_PORT GPIOG
#define KEY_A_PIN  GPIO_PIN_9
#define KEY_A_EXTI_LINE EXTI_9

#define KEY_B_PORT GPIOB
#define KEY_B_PIN  GPIO_PIN_15
#define KEY_B_EXTI_LINE EXTI_15

/* 初始化按键和中断 */
void key_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* 下拉输入 */
    gpio_mode_set(KEY_L_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, KEY_L_PIN);
    gpio_mode_set(KEY_R_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, KEY_R_PIN);
    gpio_mode_set(KEY_A_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, KEY_A_PIN);
    gpio_mode_set(KEY_B_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, KEY_B_PIN);

    /* 中断优先级组 */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    /* NVIC中断使能 */
    nvic_irq_enable(EXTI0_IRQn, 3, 3);
    nvic_irq_enable(EXTI2_IRQn, 3, 3);
    nvic_irq_enable(EXTI5_9_IRQn, 3, 3);
    nvic_irq_enable(EXTI10_15_IRQn, 3, 3);

    /* 配置EXTI线路到GPIO */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN0);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN2);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOG, EXTI_SOURCE_PIN9);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN15);

    /* 初始化EXTI，BOTH沿触发 */
    exti_init(KEY_L_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(KEY_R_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(KEY_A_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(KEY_B_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);

    /* 使能中断 */
    exti_interrupt_enable(KEY_L_EXTI_LINE);
    exti_interrupt_enable(KEY_R_EXTI_LINE);
    exti_interrupt_enable(KEY_A_EXTI_LINE);
    exti_interrupt_enable(KEY_B_EXTI_LINE);

    /* 清除中断标志位 */
    exti_interrupt_flag_clear(KEY_L_EXTI_LINE);
    exti_interrupt_flag_clear(KEY_R_EXTI_LINE);
    exti_interrupt_flag_clear(KEY_A_EXTI_LINE);
    exti_interrupt_flag_clear(KEY_B_EXTI_LINE);
}

/* KEY_L中断回调 */
void EXTI0_IRQHandler(void)
{
    if(exti_interrupt_flag_get(KEY_L_EXTI_LINE))
    {
        if(gpio_input_bit_get(KEY_L_PORT, KEY_L_PIN)) /* 上升沿：按下 */
        {
            if(!L_lock)
            {
                delay_1ms(DEBOUNCE_MS);
                if(gpio_input_bit_get(KEY_L_PORT, KEY_L_PIN))
                {
                    L_pressed = true;
                    gpio_bit_toggle(GPIOD, GPIO_PIN_7);
                    L_lock = 1;
                }
            }
        }
        else /* 下降沿：松开 */
        {
            L_lock = 0;
        }
        exti_interrupt_flag_clear(KEY_L_EXTI_LINE);
    }
}

/* KEY_R中断回调 */
void EXTI2_IRQHandler(void)
{
    if(exti_interrupt_flag_get(KEY_R_EXTI_LINE))
    {
        if(gpio_input_bit_get(KEY_R_PORT, KEY_R_PIN)) /* 上升沿：按下 */
        {
            if(!R_lock)
            {
                delay_1ms(DEBOUNCE_MS);
                if(gpio_input_bit_get(KEY_R_PORT, KEY_R_PIN))
                {
                    R_pressed = true;
                    gpio_bit_toggle(GPIOD, GPIO_PIN_7);
                    R_lock = 1;
                }
            }
        }
        else /* 下降沿：松开 */
        {
            R_lock = 0;
        }
        exti_interrupt_flag_clear(KEY_R_EXTI_LINE);
    }
}

/* KEY_A中断回调 */
void EXTI5_9_IRQHandler(void)
{
    if(exti_interrupt_flag_get(KEY_A_EXTI_LINE))
    {
        if(gpio_input_bit_get(KEY_A_PORT, KEY_A_PIN)) /* 上升沿：按下 */
        {
            if(!A_lock)
            {
                delay_1ms(DEBOUNCE_MS);
                if(gpio_input_bit_get(KEY_A_PORT, KEY_A_PIN))
                {
                    A_pressed = true;
                    gpio_bit_toggle(GPIOD, GPIO_PIN_7);
                    A_lock = 1;
                }
            }
        }
        else /* 下降沿：松开 */
        {
            A_lock = 0;
        }
        exti_interrupt_flag_clear(KEY_A_EXTI_LINE);
    }
}

/* KEY_B中断回调 */
void EXTI10_15_IRQHandler(void)
{
    if(exti_interrupt_flag_get(KEY_B_EXTI_LINE))
    {
        if(gpio_input_bit_get(KEY_B_PORT, KEY_B_PIN)) /* 上升沿：按下 */
        {
            if(!B_lock)
            {
                delay_1ms(DEBOUNCE_MS);
                if(gpio_input_bit_get(KEY_B_PORT, KEY_B_PIN))
                {
                    B_pressed = true;
                    gpio_bit_toggle(GPIOD, GPIO_PIN_7);
                    B_lock = 1;
                }
            }
        }
        else /* 下降沿：松开 */
        {
            B_lock = 0;
        }
        exti_interrupt_flag_clear(KEY_B_EXTI_LINE);
    }
}