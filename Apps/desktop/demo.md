```C++
uint16_t item_width_array[] = {48, 48, 48, 48, 48};       /* 设置每行内各组件宽度 */
ui_row(ctx, 30, item_width_array, 5);
if (ui_button_colored(ctx, 按钮颜色, 被选中时颜色, 按下时颜色)) { /* 按钮按下执行*/ }
if (ui_button_colored(ctx, 按钮颜色, 被选中时颜色, 按下时颜色)) { /* 按钮按下执行*/ }
if (ui_button_colored(ctx, 按钮颜色, 被选中时颜色, 按下时颜色)) { /* 按钮按下执行*/ }
if (ui_button_colored(ctx, 按钮颜色, 被选中时颜色, 按下时颜色)) { /* 按钮按下执行*/ }
if (ui_button_colored(ctx, 按钮颜色, 被选中时颜色, 按下时颜色)) { /* 按钮按下执行*/ }
uint16_t row2_width_array[] = {150};
ui_row(ctx, 150, row2_width_array, 1);
ui_scroll_begin(ctx, 20);

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
ui_scroll_end(ctx);

ui_row(ctx, 5, row2_width_array, 2);
desktop_activity.layout.at_x = 30;
uint16_t row3_width_array[] = {60, 70};
ui_row(ctx, 34, row3_width_array, 2);
if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
if (ui_button_colored(ctx, normal, hover, active)) { /* 按钮按下执行*/ }
```