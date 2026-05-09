#include "Joystick.h"
#include "gd32f4xx.h"
#include "systick.h"

float adcX_ValueOffset;//偏移量,以上电时刻摇杆位置为中值
float adcY_ValueOffset;

float adcBat_Value = 0;

// 摇杆初始状态
volatile JoystickState joyState = {JOY_NONE};

void AdcBat_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
    gpio_mode_set(ADC_BAT_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_BAT_PIN);
}

void Adc_Init(void)
{ 
    /* enable ADC0 clock */
    rcu_periph_clock_enable(RCU_ADC0);

    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8); 
    /* reset ADC */
    rcu_periph_reset_enable(RCU_ADCRST);
    rcu_periph_reset_disable(RCU_ADCRST);

    /* configure the ADC  mode */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT); // 所有ADC都工作在独立模式

    /* ADC contineous function disable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE); // 关闭连续模式
    /* ADC scan mode disable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE); // 关闭扫描模式

    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT); // LSB对齐，低位对齐

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1U); //  ADC规则通道 长度为1

    /* enable ADC interface */
    adc_enable(ADC0);
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0); // ADC校准
    /* wait for ADC stability */
    delay_1ms(1);
    
    AdcKey_Init();
    AdcBat_Init();
}

/**
 * @brief   Adc0_GetValue: 采集ADC0的channel通道的电压数据
 * @param    [in] channel: 需要采集的通道
 * @return       uint16_t: 采集到的电压数据
 */

uint16_t Adc0_GetValue(uint8_t channel)
{
    /* ADC regular channel config */
    adc_routine_channel_config(ADC0, 0U, channel, ADC_SAMPLETIME_15); // 15个采样周期
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL); // ADC软件触发使能

    /* wait the end of conversion flag */
    while (!adc_flag_get(ADC0, ADC_FLAG_EOC));
	
    /* clear the end of conversion flag */
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    /* return regular channel sample value */
    return (adc_routine_data_read(ADC0));
}


void AdcKey_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_mode_set(ADCX_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADCX_PIN);
	gpio_mode_set(ADCY_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADCY_PIN);

    
    adcX_ValueOffset=((4096.0-(float)Adc0_GetValue(ADC_CHANNEL_1)) / 2048.0 - 1);
    adcY_ValueOffset=((4096.0-(float)Adc0_GetValue(ADC_CHANNEL_11)) / 2048.0 - 1);
}

// 摇杆方向读取
float Get_Joystick_X(void) {
    float val = ((4096.0 - (float)Adc0_GetValue(ADC_CHANNEL_1)) / 2048.0 - 1) - adcX_ValueOffset;
    return val;
}

float Get_Joystick_Y(void) {
    float val = ((4096.0 - (float)Adc0_GetValue(ADC_CHANNEL_11)) / 2048.0 - 1) - adcY_ValueOffset;
    return val;
}

// 摇杆方向检测（边沿触发版）
JoystickDir Joystick_ReadDir(void) {
    float x = Get_Joystick_X();
    float y = Get_Joystick_Y();
    JoystickDir dir = JOY_NONE;

    // 判断当前方向
    if (x > JOY_THRESHOLD_HIGH) dir = JOY_RIGHT;
    else if (x < JOY_THRESHOLD_LOW) dir = JOY_LEFT;
    else if (y > JOY_THRESHOLD_HIGH) dir = JOY_UP;
    else if (y < JOY_THRESHOLD_LOW) dir = JOY_DOWN;

    JoystickDir ret = JOY_NONE;

    // 只有方向刚改变时才触发一次
    if (dir != joyState.lastDir && dir != JOY_NONE) {
        ret = dir;  // 触发动作
    }

    // 更新上一次方向
    joyState.lastDir = dir;

    return ret;
}