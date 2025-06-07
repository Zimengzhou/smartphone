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
    .selected_button_index = 0,
    .button_index = 0,
    .clip = {0, 0, ACTIVITY_WIDTH, ACTIVITY_HEIGHT}
};

void desktop_init(void)
{

}

void desktop_display(ui_context *ctx)
{
    ui_color normal = {107, 112, 187, 255};
    ui_color hover = {60, 68, 182, 255};
    ui_color active = {188, 65, 102, 255};

    ui_activity_show(ctx, &desktop_activity);

    ui_row(ctx, 5, 0, 0); /*空白行, 高15*/

    uint16_t row1_width_array[] = {49, 60, 49};
    ui_row(ctx, 30, row1_width_array, 3);
    if (ui_button_colored(ctx, normal, hover, active))
    { /* 按钮按下执行*/
    }
    if (ui_button_colored(ctx, normal, hover, active))
    { /* 按钮按下执行*/
    }
    if (ui_button_colored(ctx, normal, hover, active))
    { /* 按钮按下执行*/
    }
}

void desktop_process(void)
{
    // This function is intended to process events or updates for the desktop interface.
    // It can handle user inputs, refresh the display, or manage desktop elements.
    // Currently, it does not perform any operations.
}
