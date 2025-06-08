#include "szmui.h"

/*
 *Activity Init
 * */
void ui_activity_init(struct Activity* activity)
{
    activity->layout.at_x = 0;
    activity->layout.at_y = 0;
    activity->layout.offset_x = 0;
    activity->layout.offset_y = 0;
    activity->layout.row_height = 0;
    activity->layout.item_width = 0;
    activity->layout.row_columns = 0;
    activity->layout.row_index = 0;
    activity->layout.layout_type = UI_LAYOUT_NONE;

    activity->is_active = false;
    activity->selected_button_index = 0;
    activity->button_index = 0;

    activity->clip.x = 0;
    activity->clip.y = 0;
    activity->clip.w = ACTIVITY_WIDTH;
    activity->clip.h = ACTIVITY_HEIGHT;

}

/**
 * 显示指定Activity
 *
 * parameter
 * - ctx: ui context 
 * - activity: activity to be shown
 * */
void ui_activity_show(ui_context *ctx, struct Activity *activity)
{
    if (ctx->activity != activity)
    {
        if(ctx->activity!=(void*)0)
            ctx->activity->is_active = false;
        ctx->activity = activity;
        ctx->activity->is_active = true;
    }
    ctx->activity->button_index = 0;
    ctx->activity->layout.at_y = 0;
    ctx->activity->layout.row_height = 0;
    ctx->widget_state_flag = UI_WIDGET_STATE_INACTIVE;
}
