#include "szmui.h"

bool ui_init(ui_context* ctx)
{
    ctx->activity = (void*)0;
    ctx->button_behavior = UI_BUTTON_JUST_PRESSED; 
    ctx->widget_state_flag = UI_WIDGET_STATE_INACTIVE;

    ctx->input.key_down = 0;
    ctx->input.fn_flag = 0;
    ctx->input.key_long_up = 0;
    ctx->input.key_short_up = 0;
    ctx->input.key_long_down = 0;

	return true;
}

