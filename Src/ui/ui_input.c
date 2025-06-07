#include "szmui.h"
/*
 * 按键是否有按下事件
 * */
bool ui_input_is_key_down(struct ui_input *input, enum ui_input_key key)
{
    return (input->key_down)&(1<<key);
}

/*
 * 按键是否有长按下事件
 * */
bool ui_input_is_key_long_down(struct ui_input *input, enum ui_input_key key)
{
    return (input->key_long_down)&(1<<key);
}
/*
 * 按键是否有长按后抬起事件
 * */
bool ui_input_is_key_long_up(struct ui_input *input, enum ui_input_key key)
{
    return (input->key_long_up)&(1<<key);
}
/*
 * 按键是否有短按下抬起事件
 * */
bool ui_input_is_key_short_up(struct ui_input *input, enum ui_input_key key)
{
    return (input->key_short_up)&(1<<key);
}
/*
 * 按键是否处于按下状态
 * */
bool ui_input_is_key_pressed(struct ui_input *input, enum ui_input_key key)
{
    return (input->key_state)&(1<<key);
}
