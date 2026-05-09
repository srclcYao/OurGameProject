#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>
#include <stdbool.h>

#define ADCX_PIN GPIO_PIN_1
#define ADCX_PORT GPIOC
#define ADCY_PIN GPIO_PIN_1
#define ADCY_PORT GPIOA

#define ADC_BAT_PIN GPIO_PIN_1
#define ADC_BAT_PORT GPIOB

#define RES_UP   10000.0 //10k 分压电阻阻值
#define RES_DOWN 10000.0 //10k
#define RES_DAC  50000.0 //50k 输入阻抗
#define RES_VALUE ((RES_DOWN)/(RES_UP+RES_DOWN))

// 摇杆ADC阈值
#define JOY_THRESHOLD_HIGH  0.3f   // 超过偏移量认为是正方向
#define JOY_THRESHOLD_LOW  -0.3f   // 低于偏移量认为是负方向

// 摇杆方向定义
typedef enum {
    JOY_NONE = 0,
    JOY_UP,
    JOY_DOWN,
    JOY_LEFT,
    JOY_RIGHT
} JoystickDir;

// 摇杆按键状态机
typedef struct {
    JoystickDir lastDir;  // 上一次方向
    //bool active;       // 当前动作是否被触发
} JoystickState;

extern volatile JoystickState joyState;

void Adc_Init(void);
uint16_t Adc0_GetValue(uint8_t channel);
void AdcKey_Init(void);

// 摇杆方向读取
float Get_Joystick_X(void);
float Get_Joystick_Y(void);
JoystickDir Joystick_ReadDir(void);


#endif