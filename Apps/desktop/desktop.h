#ifndef APPS_DESKTOP_H_SZM
#define APPS_DESKTOP_H_SZM
#include <stdint.h>
#include "stm32h750xx.h"
#include "stm32h7xx_ll_gpio.h"

#include "szmui.h"
#ifdef __cplusplus
extern "C" {
#endif

void desktop_display(ui_context *ctx);

#ifdef __cplusplus
}
#endif
#endif