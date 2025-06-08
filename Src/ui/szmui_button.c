#include "szmui.h"

#include "SEGGER_RTT.h"
bool ui_do_button(ui_context* ctx)
{
    bool result = false;
    if (ctx->activity->button_index == ctx->activity->selected_button_index) 
    {
        ctx->widget_state_flag = UI_WIDGET_STATE_HOVER;
        if (ui_input_is_key_pressed(&ctx->input, UI_KEY_OK))
        {
            ctx->widget_state_flag |= UI_WIDGET_STATE_ACTIVE;
        }

        if (ctx->widget_state_flag & UI_WIDGET_STATE_ACTIVE)
        {
            result = (ctx->button_behavior == UI_BUTTON_JUST_PRESSED) ? 
            ui_input_is_key_short_up(&ctx->input, UI_KEY_OK) :
            ui_input_is_key_pressed(&ctx->input, UI_KEY_OK);
        }
    }
    return result;
}

#include "lcd.h"
void ui_draw_button(struct ui_button *button, struct ui_rect* bounds, uint8_t widget_state_flag)
{
    ui_color color;
    if (widget_state_flag&UI_WIDGET_STATE_ACTIVE)
    {
        color = button->active;
    }
    else if (widget_state_flag&UI_WIDGET_STATE_HOVER)
    {
        color = button->hover;
    }
    else color = button->normal;

    LCD_FillRect(bounds->x, bounds->y, bounds->w, bounds->h, ((color.r>>3)<<11) | ((color.g>>2)<<5) | (color.b>>3));
}

bool ui_button_colored(ui_context* ctx, ui_color normal_color, ui_color hover_color, ui_color active_color)
{
    bool result = false;
//    struct ui_layout *layout = &ctx->activity->layout;
    ctx->activity->button_index++;
    ctx->widget_state_flag = UI_WIDGET_STATE_INACTIVE;

    struct ui_rect bounds;
    uint8_t show_flag = ui_layout_do(&bounds, ctx);
    result = ui_do_button(ctx);

    if(show_flag)
    {
        struct ui_button button;
        button.hover = hover_color;
        button.active = active_color;
        button.normal = normal_color;
        ui_draw_button(&button, &bounds, ctx->widget_state_flag);
    }

    return result;
}


void ui_button_set_type(ui_context* ctx, enum button_pressed_type type) { ctx->button_behavior = type; }

