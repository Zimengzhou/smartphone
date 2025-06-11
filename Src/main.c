/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include <stm32h7xx_hal_adc.h>
#include "tools.h"
#include "qspi_w25q128.h"
// #include "EventRecorder.h"
#include "keyboard.h"
#include "SEGGER_RTT.h"

// #define FONT_DEFINE

#define english_characters
#include "lcd_init.h"
#include "lcd.h"
#include "szmui.h"
#include "desktop.h"

ui_context ctx = {
  .activity = (void *)0,
  .button_behavior = UI_BUTTON_JUST_PRESSED,
  .widget_state_flag = UI_WIDGET_STATE_INACTIVE,
  .input = {
    .key_down = 0,
    .key_short_up = 0,
    .key_long_down = 0,
    .key_long_up = 0,
    .fn_flag = 0
  }
};
//#include "usbd_cdc_if.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;

SD_HandleTypeDef hsd2;

SDRAM_HandleTypeDef hsdram2;

/* USER CODE BEGIN PV */

USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SDMMC2_SD_Init(void);
static void MX_UART4_Init(void);
static void MX_SPI1_Init(void);
static void MX_FMC_Init(void);
static void MX_ADC3_Init(void);
/* USER CODE BEGIN PFP */

void SPI_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint16_t adc3_temperature_value[10];
__IO uint32_t time_stamppp_1, time_stamppp_4, time_stamppp_3, time_stamppp_2;
extern __IO uint32_t time_stamp_3;
unsigned char ascii_3216_[95][64];
unsigned char ascii_2412_[95][48];
unsigned char ascii_1608_[95][16];
unsigned char ascii_1206_[95][12];

extern struct Activity desktop_activity;
void uart_print(char *ch)
{
    while (*ch)
    {
        LL_USART_TransmitData8(UART4, *ch);
        while (!LL_USART_IsActiveFlag_TC(UART4))
            ;
        ch++;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

    uint32_t *SouceAddr = (uint32_t *)(0x90000000UL);
    uint32_t *DestAddr = (uint32_t *)D1_DTCMRAM_BASE;
    memcpy(DestAddr, SouceAddr, 0x400);
    SCB->VTOR = D1_DTCMRAM_BASE;
    // 注意更改分散加载文件中的起始地址，要加上0x400，总大小减去0x400
    // SCB->VTOR = 0x90000000; /* 设置中断向量表地址 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_UART4_Init();
  MX_SPI1_Init();
  MX_FMC_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  /* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);

  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

  /* Add CDC Interface Class */
  USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

  /* Start Device Process */
  USBD_Start(&USBD_Device);
  
  TSG_INIT();
  Lcd_Init();
  Lcd_Init();
  Lcd_Init();

  SEGGER_RTT_Init();
  SEGGER_RTT_SetTerminal(0);
  SEGGER_RTT_printf(0, "SEGGER_RTT_Viewer\n");

  //    EventRecorderInitialize(EventRecordAll, 1U);
  //    EventRecorderStart();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    char i_to_string[100];
    char frame_rate_string[100];
    HAL_ADC_Start_IT(&hadc3);

    uint16_t color = 0x841F;

    memcpy(ascii_3216_, ascii_3216, 95 * 64);
    memcpy(ascii_2412_, ascii_2412, 95 * 48);
    memcpy(ascii_1608_, ascii_1608, 95 * 16);
    memcpy(ascii_1206_, ascii_1206, 95 * 12);
    
    // LL_SPI_EnableIT_EOT(SPI1);
    LCD_FillRect(0, 0, 280, 240, 0xFFFF);
    lcd_buffer = lcd_buffer_2;
    LCD_FillRect(0, 0, 280, 240, 0xFFFF);
    lcd_buffer = lcd_buffer_1;
    SCB_CleanInvalidateDCache();
    LCD_Address_Set(0, 0, 239, 139);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, ((uint32_t)&lcd_buffer[LCD_DMA_INDEX * 140][0]));
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_0, LL_SPI_DMA_GetTxRegAddr(SPI1));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, 240 * 140);
    LL_SPI_SetTransferSize(SPI1, 240 * 140);

    LL_SPI_Enable(SPI1);
    LL_SPI_EnableIT_EOT(SPI1);
    LL_SPI_StartMasterTransfer(SPI1);
    LCD_DMA_INDEX = 1;
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);

    LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_5);

    // LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_3);
    // LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_4);
    // LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_6);
    // LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);
    // LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
    // LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_14);
    // LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_15);
    // LL_GPIO_ResetOutputPin(GPIOI, LL_GPIO_PIN_8);
    // LL_GPIO_ResetOutputPin(GPIOI, LL_GPIO_PIN_11);
    


    while (1)
    {
        static int8_t key_code = -1, key_event_type = 0;
        // 判断变量LCD_BUFFER_INDEX的值是否发生变化, 如果从0变1或者从1变成0，那么
        static uint8_t last_buffer_index = 0;
        if(last_buffer_index != LCD_BUFFER_INDEX)
        {
          Get_TSG_VALUE(time_stamppp_1, time_stamppp_2);
          time_stamppp_4 = time_stamppp_1;
          lcd_buffer = (LCD_BUFFER_INDEX == 1) ? lcd_buffer_1 : lcd_buffer_2;

          LCD_FillRect(0, 0, 280, 240, color);
          // LCD_DrawString(25, 0, "STM32H750,running in QSPIflash", BLACK, 12);
          desktop_display(&ctx);
          LCD_DrawString(28, 0, "21:25", BLACK, 16);
          LCD_DrawString(170, 0, i_to_string, BLACK, 12);
          LCD_DrawString(200, 0, frame_rate_string, BLACK, 12);
          
          SCB_CleanInvalidateDCache();
          Get_TSG_VALUE(time_stamppp_1, time_stamppp_2);
          time_stamppp_3 = time_stamppp_1 - time_stamppp_4;
        }
        last_buffer_index = LCD_BUFFER_INDEX;

        static uint16_t get_temperature_time_interval = 0,get_temperature_time_interval_2 = 0;
        // if (get_temperature_time_interval >= 65530)
        if (get_temperature_time_interval >= 22300)
        {
          get_temperature_time_interval = 0;
          if (get_temperature_time_interval_2 >= 100)
          {
            uint32_t temp_adc = 0;
            for (uint8_t i = 0; i < 10; i++)
              temp_adc += adc3_temperature_value[i];
            temp_adc = temp_adc / 10;
            get_temperature_time_interval_2 = 0;
            HAL_ADC_Stop_IT(&hadc3);
            sprintf(i_to_string, "T:%lu", __HAL_ADC_CALC_TEMPERATURE(2500, temp_adc, ADC_RESOLUTION_16B));
            sprintf(frame_rate_string, "Fps:%.1f", 240000000.0 / (time_stamp_3));
            // sprintf(frame_rate_string, "Fps : %f, time interval: %u", 240000000.0 / (time_stamp_3),time_stamppp_3);
            // CDC_Transmit(frame_rate_string, 12);

            // sprintf(frame_rate_string, "Fps : %.2f, text string show test", 240000000.0/(time_stamppp_4));
            // uart_print(i_to_string);
            HAL_ADC_Start_IT(&hadc3);
            // color += 1;
            // SEGGER_RTT_printf(0, "SEGGER_RTT_Viewer\n");

            
          }
          get_temperature_time_interval_2++;
        }
        get_temperature_time_interval++;

        while(!keyboard_event_over())
        {
          keyboard_event event = keyboardPollevent();
          key_code = event.key_code;
          key_event_type = event.key_event;
          switch(key_event_type)
          {
          case 0:
            SEGGER_RTT_printf(0, "key_%d DOWN\n", key_code);
            break;
          case 1:
            SEGGER_RTT_printf(0, "key_%d SHORT_UP\n", key_code);
            break;
          case 2:
            SEGGER_RTT_printf(0, "key_%d LONG_DOWN\n", key_code);
            break;
          case 3:
            SEGGER_RTT_printf(0, "key_%d LONG_UP\n", key_code);
            break;
          }
          // time_stamppp_4 = time_stamppp_1;
          if (event.key_code == 6)
          {
            if (event.key_event == KEY_DOWN)
            {
            }
            if (event.key_event == KEY_LONG_DOWN)
            {
            }
            if (event.key_event == KEY_SHORT_UP)
            {
              desktop_activity.selected_button_index--;
            }
            if (event.key_event == KEY_LONG_UP)
            {
            }
          }
          if (event.key_code == 8)
          {
            if (event.key_event == KEY_DOWN)
            {
            }
            if (event.key_event == KEY_LONG_DOWN)
            {
            }
            if (event.key_event == KEY_SHORT_UP)
            {
              desktop_activity.selected_button_index++;
            }
            if (event.key_event == KEY_LONG_UP)
            {
            }
          }
        }

        static uint32_t counter_keyscan = 0;
        counter_keyscan++;
        if(counter_keyscan >= 200000)
        {
          keyboard_scan();
          char ch = SEGGER_RTT_GetKey();
          if(ch == ' ')
          {
            desktop_activity.selected_button_index++;
          }
          else if(ch == 'b')
          {
            desktop_activity.selected_button_index--;
          }
          counter_keyscan = 0;
        }

        // LCD_Present_Buffer();
        
        // Get_TSG_VALUE(time_stamppp_3, time_stamppp_2);
        // time_stamppp_4 = time_stamppp_3 - time_stamppp_1;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_4)
  {
  }
  LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_HSI48_Enable();

   /* Wait till HSI48 is ready */
  while(LL_RCC_HSI48_IsReady() != 1)
  {

  }
  LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1Q_Enable();
  LL_RCC_PLL1R_Enable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_8_16);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(1);
  LL_RCC_PLL1_SetN(80);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(5);
  LL_RCC_PLL1_SetR(20);
  LL_RCC_PLL1_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

   /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
   LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {

  }
  LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_2);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);
  LL_SetSystemCoreClock(480000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  LL_RCC_PLL2P_Enable();
  LL_RCC_PLL2_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_8_16);
  LL_RCC_PLL2_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL2_SetM(1);
  LL_RCC_PLL2_SetN(20);
  LL_RCC_PLL2_SetP(4);
  LL_RCC_PLL2_SetQ(1);
  LL_RCC_PLL2_SetR(2);
  LL_RCC_PLL2_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL2_IsReady() != 1)
  {
  }

  LL_RCC_PLL3P_Enable();
  LL_RCC_PLL3_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
  LL_RCC_PLL3_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL3_SetM(3);
  LL_RCC_PLL3_SetN(68);
  LL_RCC_PLL3_SetP(2);
  LL_RCC_PLL3_SetQ(6);
  LL_RCC_PLL3_SetR(2);
  LL_RCC_PLL3_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL3_IsReady() != 1)
  {
  }

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.Resolution = ADC_RESOLUTION_16B;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */
    //   sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief SDMMC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC2_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC2_Init 0 */

  /* USER CODE END SDMMC2_Init 0 */

  /* USER CODE BEGIN SDMMC2_Init 1 */

  /* USER CODE END SDMMC2_Init 1 */
  hsd2.Instance = SDMMC2;
  hsd2.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd2.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd2.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd2.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd2.Init.ClockDiv = 1;
  if (HAL_SD_Init(&hsd2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDMMC2_Init 2 */

  /* USER CODE END SDMMC2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetSPIClockSource(LL_RCC_SPI123_CLKSOURCE_PLL3P);

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* SPI1 DMA Init */

  /* SPI1_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_STREAM_0, LL_DMAMUX1_REQ_SPI1_TX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_0, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_0, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_0, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_0, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_0, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_0, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_0, LL_DMA_MDATAALIGN_HALFWORD);

  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_0);

  /* SPI1 interrupt Init */
  NVIC_SetPriority(SPI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(SPI1_IRQn);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_SIMPLEX_TX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 0x0;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_SetFIFOThreshold(SPI1, LL_SPI_FIFO_TH_01DATA);
  LL_SPI_DisableNSSPulseMgt(SPI1);
  /* USER CODE BEGIN SPI1_Init 2 */
  LL_SPI_EnableGPIOControl(SPI1);
  //   LL_SPI_EnableIOLock(SPI1);

  LL_SPI_EnableDMAReq_TX(SPI1);

  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_0);

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  LL_USART_InitTypeDef UART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetUSARTClockSource(LL_RCC_USART234578_CLKSOURCE_PCLK1);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  /**UART4 GPIO Configuration
  PB8   ------> UART4_RX
  PB9   ------> UART4_TX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  UART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  UART_InitStruct.BaudRate = 115200;
  UART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  UART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  UART_InitStruct.Parity = LL_USART_PARITY_NONE;
  UART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  UART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  UART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(UART4, &UART_InitStruct);
  LL_USART_DisableFIFO(UART4);
  LL_USART_SetTXFIFOThreshold(UART4, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(UART4, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_ConfigAsyncMode(UART4);

  /* USER CODE BEGIN WKUPType UART4 */

  /* USER CODE END WKUPType UART4 */

  LL_USART_Enable(UART4);

  /* Polling UART4 initialisation */
  while((!(LL_USART_IsActiveFlag_TEACK(UART4))) || (!(LL_USART_IsActiveFlag_REACK(UART4))))
  {
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),6, 0));
  NVIC_EnableIRQ(DMA1_Stream0_IRQn);

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM2 memory initialization sequence
  */
  hsdram2.Instance = FMC_SDRAM_DEVICE;
  /* hsdram2.Init */
  hsdram2.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram2.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram2.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram2.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
  hsdram2.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram2.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram2.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram2.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram2.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram2.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 9;
  SdramTiming.SelfRefreshTime = 6;
  SdramTiming.RowCycleDelay = 8;
  SdramTiming.WriteRecoveryTime = 4;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram2, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
/**
 * @brief  FMC SDRAM 模式锟斤拷锟矫的寄达拷锟斤拷锟斤拷囟锟斤拷锟?
 */
#define SDRAM_MODEREG_BURST_LENGTH_1 ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2 ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t)0x0200)

#define SDRAM_TIMEOUT 1000

    uint32_t tmpr = 0;
    FMC_SDRAM_CommandTypeDef Command;

    /* Step 1 ----------------------------------------------------------------*/
    /* 锟斤拷锟斤拷锟斤拷锟筋：锟斤拷锟斤拷锟结供锟斤拷SDRAM锟斤拷时锟斤拷 */
    Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(&hsdram2, &Command, SDRAM_TIMEOUT);

    /* Step 2: 锟斤拷时100us */
    LL_mDelay(1);

    /* Step 3 ----------------------------------------------------------------*/
    /* 锟斤拷锟斤拷锟斤拷锟筋：锟斤拷锟斤拷锟叫碉拷bank预锟斤拷锟?*/
    Command.CommandMode = FMC_SDRAM_CMD_PALL;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(&hsdram2, &Command, SDRAM_TIMEOUT);

    /* Step 4 ----------------------------------------------------------------*/
    /* 锟斤拷锟斤拷锟斤拷锟筋：锟皆讹拷刷锟斤拷 */
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(&hsdram2, &Command, SDRAM_TIMEOUT);

    /* Step 5 ----------------------------------------------------------------*/
    /* 锟斤拷锟斤拷sdram锟侥达拷锟斤拷锟斤拷锟斤拷*/
    tmpr = (uint32_t)(SDRAM_MODEREG_BURST_LENGTH_1 |
                      SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
                      SDRAM_MODEREG_CAS_LATENCY_3 |
                      SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                      SDRAM_MODEREG_WRITEBURST_MODE_SINGLE);
    /* 锟斤拷锟斤拷锟斤拷锟筋：锟斤拷锟斤拷SDRAM锟侥达拷锟斤拷*/
    Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = tmpr;
    HAL_SDRAM_SendCommand(&hsdram2, &Command, SDRAM_TIMEOUT);

    /* Step 6 ----------------------------------------------------------------*/
    /* 锟斤拷锟斤拷刷锟铰硷拷锟斤拷 */
    /* 刷锟斤拷锟斤拷锟斤拷=64ms/8192=7.8125us */
    /* COUNT=(7.8125us x Freq) - 20 */
    /* 锟斤拷锟斤拷锟斤拷刷锟斤拷锟斤拷 */
    HAL_SDRAM_ProgramRefreshRate(&hsdram2, 918);

  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOI);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOH);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF);

  /**/
  LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_6);

  /**/
  LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);

  /**/
  LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_5);

  /**/
  LL_GPIO_ResetOutputPin(GPIOC, LCD_DC_Pin|LCD_RESET_Pin);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LCD_DC_Pin|LCD_RESET_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void SPI_Init(void)
{
}

// void SDRAM_Test(void)
//{

//    char i_to_string[200];

//    /*32位锟斤拷锟斤拷锟斤拷锟?*/
//    // __IO uint32_t time_start, time_end;
//    // uint32_t *sdram_address = buffer;
//    // __IO uint32_t data_write_to_ram = 0xAAAAAAAA;

//    /* 16 位锟斤拷锟斤拷锟斤拷锟?*/
//    __IO uint32_t time_start, time_end;
//    uint32_t *sdram_address = buffer;
//    __IO uint32_t data_write_to_ram = 0xAAAAAAAA;

//    // 锟斤拷锟斤拷写锟斤拷锟劫讹拷
//    /*Cache锟截闭ｏ拷
//    写锟斤拷时锟斤拷: 145 ms写锟斤拷锟劫度ｏ拷441 MB/s
//    锟斤拷取时锟斤拷: 1280 ms锟斤拷取锟劫度ｏ拷50 MB/s

//    Cache锟斤拷锟斤拷:
//    写锟斤拷时锟斤拷: 145 ms写锟斤拷锟劫度ｏ拷441 MB/s
//    锟斤拷取时锟斤拷: 290 ms锟斤拷取锟劫度ｏ拷220 MB/s

//    */

//    time_start = uwTick;
//    for (uint32_t i = 0; i < 65536; i++)
//    {
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;

//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//        *sdram_address++ = data_write_to_ram;
//    }
//    time_end = uwTick;
//    time_start = time_end - time_start;
////    sprintf(i_to_string, "写入速度测试, 写入大小: 64MB\n写入耗时: %d ms"
////                         "写入速度：%d MB/s\n\n",
////            time_start, (uint32_t)(64.0 / time_start * 1000));
////    uart_print(i_to_string);
//      printf("写入速度测试, 写入大小: 64MB\n写入耗时: %d ms"
//                         "写入速度：%d MB/s\n\n",
//            time_start, (uint32_t)(64.0 / time_start * 1000));
//    // for (uint32_t i = 0; i < 64 * 1024 * 256; i++)
//    // {
//    //     if (buffer[i] != data_write_to_ram)
//    //     {
//    //         uart_print("写锟斤拷锟斤拷锟捷筹拷锟斤拷\n");
//    //     }
//    // }

//    /*SDRAM锟斤拷取锟劫度诧拷锟斤拷*/
//    sdram_address = buffer;
//    time_start = uwTick;
//    for (uint32_t i = 0; i < 65536; i++)
//    {
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;

//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//        data_write_to_ram = *sdram_address++;
//    }
//    time_end = uwTick;
//    time_start = time_end - time_start;
////    sprintf(i_to_string, "读取速度测试, 读取大小: 64MB\n读取耗时: %d ms"
////                         "读取速度：%d MB/s\n\n",
////            time_start, (uint32_t)(64.0 / time_start * 1000));
////    uart_print(i_to_string);
//      printf("读取速度测试, 读取大小: 64MB\n读取耗时: %d ms"
//                         "读取速度：%d MB/s\n\n",
//            time_start, (uint32_t)(64.0 / time_start * 1000));
//}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0xD0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO_URO;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
