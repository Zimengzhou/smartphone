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
#include <stdio.h>
void ui_draw_button(struct ui_button *button, uint8_t widget_state_flag, ui_context* ctx)
{
    ui_color color;
    struct ui_rect *bounds = &ctx->activity->layout.bounds_clip;
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
    // char str[5];
    // sprintf(str, "%d", ctx->activity->button_index);
    // LCD_DrawString(ctx->activity->layout.bounds.x, ctx->activity->layout.bounds.y, str, BLACK, 32);
}

bool ui_button_colored(ui_context* ctx, ui_color normal_color, ui_color hover_color, ui_color active_color)
{
    bool result = false;
//    struct ui_layout *layout = &ctx->activity->layout;
    ctx->activity->button_index++;
    ctx->widget_state_flag = UI_WIDGET_STATE_INACTIVE;

    uint8_t show_flag = ui_layout_do( ctx);
    result = ui_do_button(ctx);

    if (ctx->activity->button_index == ctx->activity->selected_button_index) 
    {
        // 判断按键是否有处于clip范围之外的部分
        struct ui_rect *clip = &ctx->activity->clip;
        struct ui_rect bounds_bk = ctx->activity->layout.bounds; 

        if(bounds_bk.y < clip->y)
        {
            ctx->activity->layout.button_scroll_ref = -bounds_bk.y + clip->y;
        }
        else if(bounds_bk.y + bounds_bk.h > clip->y + clip->h)
        {
            ctx->activity->layout.button_scroll_ref = -bounds_bk.y - bounds_bk.h + clip->y + clip->h;
        }
        // if(bounds_bk.x < clip->x || bounds_bk.y < clip->y || bounds_bk.x + bounds_bk.w > clip->x + clip->w || bounds_bk.y + bounds_bk.h > clip->y + clip->h) { }
    }

    if(show_flag)
    {
        struct ui_button button;
        button.hover = hover_color;
        button.active = active_color;
        button.normal = normal_color;
        ui_draw_button(&button, ctx->widget_state_flag, ctx);
    }

    return result;
}


void ui_button_set_type(ui_context* ctx, enum button_pressed_type type) { ctx->button_behavior = type; }

