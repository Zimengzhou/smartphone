#include "keyboard.h"

#define KEYBOARD_COLUMN 5
#define KEYBOARD_ROW    4
#define KEYBOARD_KEY_NUMS KEYBOARD_COLUMN * KEYBOARD_ROW
#define EVENT_BUFFER_SIZE 50
static __IO keyboard_event keyboard_event_buffer[EVENT_BUFFER_SIZE];
static __IO uint8_t buf_write_pos = 0;
static __IO uint8_t buf_read_pos = 0;

static uint16_t scan_count[KEYBOARD_KEY_NUMS];
uint32_t keystate = 0;

keyboard_gpio_type keyboard_col_gpio[KEYBOARD_COLUMN] = {
    {GPIOC, LL_GPIO_PIN_13},  // 0
    {GPIOC, LL_GPIO_PIN_14},  // 0
    {GPIOC, LL_GPIO_PIN_15},  // 0
    {GPIOI, LL_GPIO_PIN_8},  // 0
    {GPIOI, LL_GPIO_PIN_11},  // 0
};

keyboard_gpio_type keyboard_row_gpio[KEYBOARD_ROW] = {
    {GPIOE, LL_GPIO_PIN_3},  // 0
    {GPIOE, LL_GPIO_PIN_4},  // 0
    {GPIOB, LL_GPIO_PIN_2},  // 0
    {GPIOE, LL_GPIO_PIN_6},  // 0
};

static void push_event(uint8_t keycode, keyboard_event_type event_type)
{
    keyboard_event_buffer[buf_write_pos].key_code = keycode;
    keyboard_event_buffer[buf_write_pos].key_event = event_type;
    buf_write_pos = (buf_write_pos + 1) % EVENT_BUFFER_SIZE;
}

keyboard_event keyboardPollevent(void)
{
    uint8_t temp = buf_read_pos;
    buf_read_pos = (buf_read_pos + 1) % EVENT_BUFFER_SIZE;
    return keyboard_event_buffer[temp];
}

uint8_t keyboard_event_over(void)
{
    return buf_read_pos == buf_write_pos;
}

/**
 * @brief 10ms called per 
 * 
 */
void keyboard_scan(void)
{
    for (uint8_t i = 0; i < KEYBOARD_ROW; i++)
    {
        for (uint16_t h = 0; h < 6;h++)
        {
            for (uint8_t k = 0; k < 4;k++)
            {
                LL_GPIO_SetOutputPin(keyboard_row_gpio[k].GPIOx, keyboard_row_gpio[k].pin); // 拉高行
            }
        }
        for (uint16_t h = 0; h < 6;h++)
        {
            LL_GPIO_ResetOutputPin(keyboard_row_gpio[i].GPIOx, keyboard_row_gpio[i].pin); // 拉低列
        }
        // LL_GPIO_SetOutputPin(keyboard_col_gpio[i].GPIOx, keyboard_col_gpio[i].pin); // 拉低列

        // LL_GPIO_ResetOutputPin(keyboard_row_gpio[i].GPIOx, keyboard_row_gpio[i].pin); //拉低列
        // LL_GPIO_SetOutputPin(keyboard_col_gpio[i].GPIOx, keyboard_col_gpio[i].pin); //拉高列

        for (uint8_t j = 0; j < KEYBOARD_COLUMN; j++)
        {
            uint8_t index = j+i*5;
            if (LL_GPIO_IsInputPinSet(keyboard_col_gpio[j].GPIOx, keyboard_col_gpio[j].pin) == 0)//读取输入
            {
                if (scan_count[index] == 50)// LONG_DOWN
                {
                    push_event(index, KEY_LONG_DOWN);
                }
                if (scan_count[index] == 2)
                {
                    push_event(index, KEY_DOWN);
                    keystate |= 0x0001 << index;
                }
                scan_count[index]++;
            }
            else //松开
            {
                if (scan_count[index]>=50)
                {
                    push_event(index, KEY_LONG_UP);
                }
                else if (scan_count[index]>= 4)
                {
                    push_event(index, KEY_SHORT_UP);
                }
                if (scan_count[index] > 0)
                {
                    scan_count[index] = 0;
                    keystate &= ~(0x0001 << index);
                }
            }
            // index++;
        }
    }
}

/*
void keyboard_init(void)
{
    keyboard_gpio_init();
}

void keyboard_gpio_init(void)
{
	
	crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
	gpio_init_type initst;
	gpio_default_para_init(&initst);
	initst.gpio_pull = GPIO_PULL_DOWN;
	initst.gpio_pins = GPIO_PINS_7|GPIO_PINS_8|GPIO_PINS_9|GPIO_PINS_10;
	gpio_init(GPIOE, &initst);
	
	initst.gpio_mode = GPIO_MODE_OUTPUT;
	initst.gpio_pins = GPIO_PINS_11|GPIO_PINS_12|GPIO_PINS_13|GPIO_PINS_14|GPIO_PINS_15;
	gpio_init(GPIOE, &initst);

}
*/