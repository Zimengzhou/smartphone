#ifndef _BOARD_TOOLS_H_
#define _BOARD_TOOLS_H_

/**
 TSG 64-bit 时间戳
*/
#define TSG_CNTCR   *(volatile unsigned int *)0x5C005000
#define TSG_CNTFID0 *(volatile unsigned int *)0x5C005020
#define TSG_CNTCVL  *(volatile unsigned int *)0x5C005008
#define TSG_CNTCVU  *(volatile unsigned int *)0x5C00500C

/**
 * @brief 获取 64-bit 计数器计数值
 */
#define Get_TSG_VALUE(timeL, timeH) \
    timeH = TSG_CNTCVU;            \
    timeL = TSG_CNTCVL

/**
 * @brief 获取CPU运行时间
 * @param 
 */
#define Get_CPU_Time                                                  \
        (double)(TSG_CNTCVU * 17.8956970666 + TSG_CNTCVL / 240000000.0)

void TSG_INIT(void);


#endif