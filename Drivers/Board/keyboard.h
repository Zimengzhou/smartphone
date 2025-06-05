#ifndef KEYBOARD_H_SZM
#define KEYBOARD_H_SZM
#include <stdint.h>
#include "stm32h750xx.h"
#include "stm32h7xx_ll_gpio.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    KEY_DOWN = 0,
    KEY_SHORT_UP,
    KEY_LONG_DOWN,
    KEY_LONG_UP,
    KEY_DOUBLE_CLICK,
    KEY_REPEAT_CLICK,

} keyboard_event_type;

typedef struct
{
    uint8_t key_code;    //按键码
    keyboard_event_type key_event;   //按键事件
}keyboard_event;

typedef struct keyboard_gpio_type
{
    GPIO_TypeDef *GPIOx;
    uint16_t pin;
} keyboard_gpio_type;
// void keyboard_init(void);
// void keyboard_gpio_init(void);
void keyboard_scan(void);

uint8_t keyboard_event_over();
keyboard_event keyboardPollevent();

#ifdef __cplusplus
}
#endif

#endif