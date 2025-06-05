/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/usbd_cdc_interface.c
  * @author  MCD Application Team
  * @brief   Source file for USBD CDC interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include "main.h"
#include "usbd_cdc.h"

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048

/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
USBD_CDC_LineCodingTypeDef LineCoding = {
  115200,                       /* baud rate */
  0x00,                         /* stop bits-1 */
  0x00,                         /* parity - none */
  0x08                          /* nb. of bits 8 */
};

/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;

uint8_t UserRxBuffer[APP_RX_DATA_SIZE]; /* Received Data over USB are stored in
                                         * this buffer */
uint8_t UserTxBuffer[APP_TX_DATA_SIZE]; /* Received Data over UART (CDC
                                         * interface) are stored in this buffer
                                         */

uint32_t UserTxBufPtrIn = 0;    /* Increment this pointer or roll it back to
                                 * start address when data are received over
                                 * USART */
uint32_t UserTxBufPtrOut = 0;   /* Increment this pointer or roll it back to
                                 * start address when data are sent over USB */

/* UART handler declaration */
//UART_HandleTypeDef UartHandle;
/* TIM handler declaration */
//TIM_HandleTypeDef TimHandle;
/* USB handler declaration */
extern USBD_HandleTypeDef USBD_Device;

/* Private function prototypes ----------------------------------------------- */
//static void Error_Handler(void);
static int8_t CDC_Itf_Init(void);
static int8_t CDC_Itf_DeInit(void);
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t * pbuf, uint16_t length);
static int8_t CDC_Itf_Receive(uint8_t * pbuf, uint32_t * Len);
static int8_t CDC_TransmitCplt(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);


USBD_CDC_ItfTypeDef USBD_CDC_fops = {
  CDC_Itf_Init,
  CDC_Itf_DeInit,
  CDC_Itf_Control,
  CDC_Itf_Receive,
  CDC_TransmitCplt
};

/* Private functions --------------------------------------------------------- */

/**
  * @brief  Initializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Init(void)
{
  /* ##-5- Set Application Buffers ############################################ */
  USBD_CDC_SetTxBuffer(&USBD_Device, UserTxBuffer, 0);
  USBD_CDC_SetRxBuffer(&USBD_Device, UserRxBuffer);

  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_DeInit(void)
{
  /* DeInitialize the UART peripheral */
  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_Control
  *         Manage the CDC class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t * pbuf, uint16_t length)
{
  switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:
    /* Add your code here */
    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
    /* Add your code here */
    break;

  case CDC_SET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_GET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_CLEAR_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_SET_LINE_CODING:
    LineCoding.bitrate = (uint32_t) (pbuf[0] | (pbuf[1] << 8) |
                                     (pbuf[2] << 16) | (pbuf[3] << 24));
    LineCoding.format = pbuf[4];
    LineCoding.paritytype = pbuf[5];
    LineCoding.datatype = pbuf[6];

    /* Set the new configuration */
//    ComPort_Config();
    break;

  case CDC_GET_LINE_CODING:
    pbuf[0] = (uint8_t) (LineCoding.bitrate);
    pbuf[1] = (uint8_t) (LineCoding.bitrate >> 8);
    pbuf[2] = (uint8_t) (LineCoding.bitrate >> 16);
    pbuf[3] = (uint8_t) (LineCoding.bitrate >> 24);
    pbuf[4] = LineCoding.format;
    pbuf[5] = LineCoding.paritytype;
    pbuf[6] = LineCoding.datatype;
    break;

  case CDC_SET_CONTROL_LINE_STATE:
    /* Add your code here */
    break;

  case CDC_SEND_BREAK:
    /* Add your code here */
    break;

  default:
    break;
  }

  return (USBD_OK);
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)USBD_Device.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&USBD_Device, Buf, Len);
  result = USBD_CDC_TransmitPacket(&USBD_Device);
  /* USER CODE END 7 */
  return result;
}


/**
  * @brief  CDC_Itf_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Receive(uint8_t * Buf, uint32_t * Len)
{
  SCB_CleanDCache_by_Addr((uint32_t *)Buf, *Len);

//  HAL_UART_Transmit_DMA(&UartHandle, Buf, *Len);
  return (USBD_OK);
}


/**
  * @brief  CDC_TransmitCplt
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);

  return (0);
}

