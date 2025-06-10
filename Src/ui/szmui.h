#ifndef COPY_NUKLEAR_H_SZM
#define COPY_NUKLEAR_H_SZM
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
/**
 * Constants 
 */
#define UI_COLOR_FORMAT_RGBA8888 //color format
// #define UI_COLOR_FORMAT_RGB565 //color format
#define UI_INPUT_MAX 20
#define UI_UTF_SIZE 4
#define ACTIVITY_WIDTH (280-1)
#define ACTIVITY_HEIGHT (240-1)

struct ui_buffer;
typedef struct ui_context ui_context;
struct ui_style;
struct ui_input;
struct Activity;
struct ui_layout;
typedef struct ui_color ui_color;
struct ui_button;


struct ui_rect{int16_t x; int16_t y; int16_t w; int16_t h;};

enum button_pressed_type        {UI_BUTTON_JUST_PRESSED, UI_BUTTON_PRESSED};


#ifndef UI_COLOR_FORMAT_RGB565
#define UI_COLOR_FORMAT_RGBA8888
#endif
struct ui_color
{
#ifdef UI_COLOR_FORMAT_RGBA8888
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
#endif

#ifdef UI_COLOR_FORMAT_RGB565
    uint8_t r: 5;
    uint8_t g: 6;
    uint8_t b: 5;
    uint8_t a;
#endif
};
struct ui_buffer
{
    // struct nk_buffer_marker marker[NK_BUFFER_MAX];
    // /* buffer marker to free a buffer to a certain offset */
    // struct nk_allocator pool;
    // /* allocator callback for dynamic buffers */
    // enum nk_allocation_type type;
    // /* memory management type */
    // struct nk_memory memory;
    // /* memory and size of the current memory block */
    // float grow_factor;
    // /* growing factor for dynamic memory management */
    // nk_size allocated;
    // /* total amount of memory allocated */
    // nk_size needed;
    // /* totally consumed memory given that enough memory is present */
    // nk_size calls;
    // /* number of allocation calls */
    // nk_size size;
};
struct ui_style
{
};
struct ui_input
{
    int32_t key_state;
    int32_t key_down;
    int32_t key_short_up;
    int32_t key_long_down;
    int32_t key_long_up;

    uint8_t fn_flag;

};
enum ui_input_key
{
    // 物理按键
    UI_KEY_0 = 0, UI_KEY_1, UI_KEY_2, UI_KEY_3,
    UI_KEY_4, UI_KEY_5, UI_KEY_6, UI_KEY_7,
    UI_KEY_8, UI_KEY_9, UI_KEY_10, UI_KEY_11,
    UI_KEY_12, UI_KEY_13, UI_KEY_14, UI_KEY_15,
    UI_KEY_16, UI_KEY_17, UI_KEY_18, UI_KEY_19,
    //功能按键
    UI_KEY_OK,
//    UI_KEY_OK   = UI_KEY_19,      UI_KEY_Delete = UI_KEY_18,
//    UI_KEY_Menu = UI_KEY_15,      UI_KEY_Shift  = UI_KEY_7,
//    UI_KEY_Fn   = UI_KEY_3,       UI_KEY_Space  = UI_KEY_11, UI_KEY_Back=UI_KEY_18,
};
bool ui_input_is_key_down(struct ui_input *input, enum ui_input_key key);
bool ui_input_is_key_pressed(struct ui_input *input, enum ui_input_key key);
bool ui_input_is_key_long_down(struct ui_input *input, enum ui_input_key key);
bool ui_input_is_key_long_up(struct ui_input *input, enum ui_input_key key);
bool ui_input_is_key_short_up(struct ui_input *input, enum ui_input_key key);
/**************************************************************************************************
            -Layout
 * ************************************************************************************************/
#define   ROW_SPACE_SIZE 1
enum ui_layout_type
{
    UI_LAYOUT_NONE = 0,
    UI_LAYOUT_ROW,
    UI_LAYOUT_COLMN,
};
struct ui_layout
{
    enum ui_layout_type layout_type;
    int16_t at_x;
    int16_t at_y;
    int16_t offset_x;
    int16_t offset_y;
    uint16_t row_height;
    uint8_t row_columns;
    uint8_t row_index;
    uint16_t *item_width;
    uint16_t item_height;
    uint16_t item_offset;
    
    // Group layout state
    int16_t saved_at_x;
    int16_t saved_at_y;
    int16_t saved_offset_x;
    int16_t saved_offset_y;
};
void ui_row(struct ui_context *ctx, uint16_t item_height, uint16_t *item_width_array, uint8_t col);
uint8_t ui_layout_do(struct ui_rect* bounds, ui_context *ctx);
void ui_scroll_begin(struct ui_context* ctx, uint16_t pos_x);
void ui_scroll_end(struct ui_context* ctx);

/**************************************************************************************************
                 -Activity
 * ************************************************************************************************/
struct Activity
{
    bool is_active;
    uint16_t selected_button_index;
    uint16_t button_index;
    struct ui_layout layout;
    struct ui_rect clip;
};

void ui_activity_init(struct Activity* activity);
void ui_activity_show(ui_context *ctx, struct Activity *activity);

struct ui_context
{
    struct ui_input input;
    struct ui_style style;
    struct ui_buffer memory;

    enum button_pressed_type button_behavior;
    uint8_t widget_state_flag;

    struct Activity *activity;
};

bool ui_init(struct ui_context *ctx);


/*****************************************************************************************************
 *          UI Widget
 *
 ****************************************************************************************************/
enum ui_widget_states
{
    UI_WIDGET_STATE_INACTIVE = 0x01u,
    UI_WIDGET_STATE_HOVER    = 0x02u,
    UI_WIDGET_STATE_ACTIVE   = 0x04u,
};
struct ui_button
{    /* background */
    struct ui_color normal;
    struct ui_color hover;
    struct ui_color active;
    struct ui_color border_color;

    /* text */
    struct ui_color text_background;
    struct ui_color text_normal;
    struct ui_color text_hover;
    struct ui_color text_active;
};
bool ui_button_colored(ui_context* ctx, ui_color normal_color, ui_color hover_color, ui_color active_color);
void ui_button_set_type(ui_context* ctx, enum button_pressed_type type);


#ifdef __cplusplus
}
#endif
#endif
