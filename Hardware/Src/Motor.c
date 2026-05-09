#include "Motor.h"
#include "gd32f4xx.h"
#include "Timer.h"

//振动时间
volatile uint16_t motor_timer = 0;  // 单位 ms

// value的值限制在[min,max]
#define Tool_Range(min, value, max) \
    if (value < (min))              \
        value = (min);              \
    else if (value > (max))         \
        value = (max);

#define TIMER1_period (1000 - 1)

void PwmMotor_Init(void)
{
    // 打开 GPIOA 的时钟，使能对 GPIOA 寄存器访问
    rcu_periph_clock_enable(RCU_GPIOA);

    // 配置 GPIOA2 为复用功能(AF)，不使用上下拉
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);

    // 设置 GPIO 输出类型为推挽，速度为 50MHz
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    // 将 GPIOA2 的复用功能设置为 AF1，对应 TIMER1_CH2 输出
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);

    // -------------------- 定时器初始化 --------------------
    timer_parameter_struct timer_initpara; // 定义定时器初始化结构体
    rcu_periph_clock_enable(RCU_TIMER1);   // 打开 TIMER1 时钟
    timer_deinit(TIMER1);                  // 复位 TIMER1，清除以前的配置

    // 配置 TIMER1 基本参数
    timer_initpara.prescaler = 200 - 1;    // 定时器预分频，定时器时钟 = TIMER_CLK / 200
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;    // 边缘对齐计数模式（普通模式）
    timer_initpara.counterdirection = TIMER_COUNTER_UP; // 向上计数，从 0 到 ARR
    timer_initpara.period = TIMER1_period; // 自动重装载值（ARR），计数到这个值后回到0
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;    // 时钟分频 = 1（不额外再分频）
    timer_initpara.repetitioncounter = 0;  // PWM 重复计数，0 表示每次计数都输出
    timer_init(TIMER1, &timer_initpara);   // 初始化 TIMER1

    // -------------------- 通道配置 --------------------
    timer_oc_parameter_struct timer_ocintpara; // 定义输出比较通道配置结构体
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;      // 输出高电平有效
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;           // 使能通道输出
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;    // 互补输出高电平有效（此通道未用）
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;        // 禁止互补输出
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;    // 输出空闲状态低电平
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;  // 互补输出空闲状态低电平

    // 配置 TIMER1 的通道2 输出参数
    timer_channel_output_config(TIMER1, TIMER_CH_2, &timer_ocintpara);

    // 设置通道2 为 PWM0 模式（上升沿占空比有效）
    timer_channel_output_mode_config(TIMER1, TIMER_CH_2, TIMER_OC_MODE_PWM0);

    // 禁用通道影子寄存器，直接使用 CCR 寄存器更新占空比
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    // 启用自动重载影子寄存器，使 ARR 在更新后自动生效
    timer_auto_reload_shadow_enable(TIMER1);

    // 如果需要中断可以开启（这里注释掉了）
    // timer_interrupt_enable(TIMER1, TIMER_INT_UP);

    // 启动定时器
    timer_enable(TIMER1);
}

void PwmMotor_SetValue(float value, uint8_t timeMs)
{
    Tool_Range(0, value, 1);
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_2, TIMER1_period * value);

    motor_timer = timeMs;       // 启动计时
    timer_enable(BSP_TIMER);    // 启动定时器
}