#include "szmui.h"
#include <stdint.h>

uint8_t setinclip(struct ui_rect *bounds, struct ui_rect clip);
void ui_row(struct ui_context *ctx, uint16_t item_height, uint16_t *item_width_array, uint8_t cols)
{
    struct Activity *activity = ctx->activity;
    struct ui_layout *layout = &ctx->activity->layout;

    layout->at_y += layout->row_height;
    layout->item_height = item_height;
    layout->item_width = item_width_array;
    layout->item_offset = 0;
    layout->row_height = item_height + ROW_SPACE_SIZE;
    layout->row_columns = cols;
    layout->row_index = 0;
    layout->offset_x = 0;
}


uint8_t ui_layout_do(ui_context *ctx)
{
    struct ui_layout *layout = &ctx->activity->layout;
    struct ui_rect *bounds = &layout->bounds;
    if (layout->row_index >= layout->row_columns) 
    {
        ui_row(ctx, layout->item_height, layout->item_width, layout->row_columns);  
    }
    bounds->x = layout->at_x + layout->item_offset + layout->row_index * ROW_SPACE_SIZE + layout->offset_x;
    bounds->y = layout->at_y + layout->offset_y;
    bounds->w = layout->item_width[layout->row_index];
    bounds->h = layout->item_height;

    layout->bounds_clip = *bounds;

    uint8_t show_flag = setinclip(&layout->bounds_clip, ctx->activity->clip);

    layout->item_offset += layout->item_width[layout->row_index]; 
    layout->row_index++;
    return show_flag;
}

#include "lcd.h"
void ui_layout_draw_scroll(struct ui_rect bounds)
{
    // LCD_FillRect(bounds.x, bounds.y, bounds.w, bounds.h, 0x352d);
}

void ui_scroll_begin(struct ui_context* ctx, uint16_t pos_x)
{
    struct ui_layout* layout = &ctx->activity->layout;
    
    layout->saved_at_x = layout->at_x;
    layout->saved_at_y = layout->at_y + layout->row_height;

    layout->at_x += pos_x;
    ui_layout_do(ctx);
    ui_layout_draw_scroll(layout->bounds_clip);
    ctx->activity->clip.x = layout->bounds.x;
    ctx->activity->clip.y = layout->bounds.y;
    ctx->activity->clip.w = layout->bounds.w;
    ctx->activity->clip.h = layout->bounds.h;

    layout->row_height = 0;
}
void ui_scroll_end(struct ui_context* ctx)
{
    ctx->activity->layout.at_x = ctx->activity->layout.saved_at_x;
    ctx->activity->layout.at_y = ctx->activity->layout.saved_at_y;
    ctx->activity->layout.button_scroll_ref = 0;
    ctx->activity->layout.row_height = 0;
    ctx->activity->layout.offset_y = 0;
    ctx->activity->layout.offset_x = 0;
    ctx->activity->clip.x = 0;
    ctx->activity->clip.y = 0;
    ctx->activity->clip.w = ACTIVITY_WIDTH;
    ctx->activity->clip.h = ACTIVITY_HEIGHT;
    
}

uint8_t setinclip(struct ui_rect *bounds, struct ui_rect clip)
{
    struct ui_rect b = *bounds;

    int16_t x1 = clip.x, x2 = clip.x+clip.w, y1 = clip.y, y2 = clip.y+clip.h;

    if (bounds->x + bounds->w < clip.x) 
        return 0;
    if (bounds->x  > x2) 
        return 0;
    if (bounds->y + bounds->h < clip.y) 
        return 0;
    if (bounds->y  > y2) 
        return 0;

    bounds->x = (b.x < clip.x)? clip.x : b.x;
    bounds->y = (b.y < clip.y)? clip.y : b.y;
    bounds->w = (b.x + b.w > x2)? x2-bounds->x : b.x+b.w-bounds->x;
    bounds->h = (b.y + b.h > y2)? y2-bounds->y : b.y+b.h-bounds->y;
    return 1;
}
