#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "sdram.h"

static void SDRAM_InitSequence(void)
{
    uint32_t tmpr = 0;

    /* Step 1 ----------------------------------------------------------------*/
    /* 配置命令：开启提供给SDRAM的时钟 */
    Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command.CommandTarget = FMC_COMMAND_TARGET_BANK;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    /* 发送配置命令 */
    HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

    /* Step 2: 延时100us */
    SDRAM_delay(1);

    /* Step 3 ----------------------------------------------------------------*/
    /* 配置命令：对所有的bank预充电 */
    Command.CommandMode = FMC_SDRAM_CMD_PALL;
    Command.CommandTarget = FMC_COMMAND_TARGET_BANK;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    /* 发送配置命令 */
    HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

    /* Step 4 ----------------------------------------------------------------*/
    /* 配置命令：自动刷新 */
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command.CommandTarget = FMC_COMMAND_TARGET_BANK;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;
    /* 发送配置命令 */
    HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

    /* Step 5 ----------------------------------------------------------------*/
    /* 设置sdram寄存器配置 */
    tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
        SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
        SDRAM_MODEREG_CAS_LATENCY_3           |
        SDRAM_MODEREG_OPERATING_MODE_STANDARD |
        SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    /* 配置命令：设置SDRAM寄存器 */
    Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command.CommandTarget = FMC_COMMAND_TARGET_BANK;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = tmpr;
    /* 发送配置命令 */
    HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

    /* Step 6 ----------------------------------------------------------------*/

    /* 设置刷新计数器 */
    /* 刷新周期=64ms/8192行=7.8125us */
    /* COUNT=(7.8125us x Freq) - 20 */
    /* 设置自刷新速率 */
    HAL_SDRAM_ProgramRefreshRate(&sdramHandle, 824);
}