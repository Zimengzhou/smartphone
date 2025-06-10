#include "desktop.h"
#include "szmui.h"

struct Activity desktop_activity = {
    .layout = {
        .at_x = 0,
        .at_y = 0,
        .offset_x = 0,
        .offset_y = 0,
        .row_height = 0,
        .item_width = 0,
        .row_columns = 0,
        .row_index = 0,
        .layout_type = UI_LAYOUT_NONE
    },
    .is_active = false,
    .selected_button_index = 11,
    .button_index = 0,
    .clip = {0, 0, ACTIVITY_WIDTH, ACTIVITY_HEIGHT}
};

static int16_t appGrid_scroll_offset_y = 0;
static int16_t appGrid_scroll_offset_y_ref = 0;

void desktop_init(void)
{

}

void desktop_display(ui_context *ctx)
{
    ui_color normal = {0xee, 0x11, 0x69, 255};
    ui_color hover = {0xbe, 0x0e, 0x54, 255};  
    ui_color active = {0x7F, 0x09, 0x38, 255};

    ui_activity_show(ctx, &desktop_activity);
    desktop_activity.layout.at_x = 15;
    ui_row(ctx, 15, 0, 0); /*空白行, 高15*/

    uint16_t row1_width_array[] = {48, 48, 48, 48, 48};       /* 设置每行内各组件宽度 */
    ui_row(ctx, 30, row1_width_array, 5);
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { }
    if (ui_button_colored(ctx, normal, hover, active)) { }
    if (ui_button_colored(ctx, normal, hover, active)) { }
    if (ui_button_colored(ctx, normal, hover, active)) { }

    uint16_t row2_width_array[] = {150};
    ui_row(ctx, 129, row2_width_array, 1);
    ui_scroll_begin(ctx, 20);
    ctx->activity->layout.offset_y = appGrid_scroll_offset_y_ref;
    uint16_t appListGridSize[] = {50, 50, 50};
    ui_row(ctx, 50, appListGridSize, 3);
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    appGrid_scroll_offset_y_ref += ctx->activity->layout.button_scroll_ref;
    ui_scroll_end(ctx);

    ui_row(ctx, 5, row2_width_array, 2);
    desktop_activity.layout.at_x = 30;
    uint16_t row3_width_array[] = {60, 70};
    ui_row(ctx, 34, row3_width_array, 2);
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
    if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
}

void desktop_process(void)
{
    // This function is intended to process events or updates for the desktop interface.
    // It can handle user inputs, refresh the display, or manage desktop elements.
    // Currently, it does not perform any operations.
}

    // static int32_t time = 0;
    // static uint16_t counter_mov = 0;
    // counter_mov++;
    // if (counter_mov >= 10)
    // {
    //     time-=1;
    //     counter_mov = 0;
    // }
    // ctx->activity->layout.offset_y = time;