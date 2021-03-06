/*
 * Setup.h
 *
 *  Created on: Jan 30, 2018
 *      Author: Luke
 */

#ifndef SETUP_H_
#define SETUP_H_

#include "wifi.h"
#include "stm32l4xx_hal.h"

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_SPI3_Init(void);
void MX_USART1_UART_Init(void);
void MX_USB_OTG_FS_PCD_Init(void);
uint8_t abs_val(int16_t num);
void append_string(uint8_t num, uint8_t* arr);
void thingSpeakUpdate(uint8_t* wifi_xmit);

/*Public variables*/
SPI_HandleTypeDef hspi3;
UART_HandleTypeDef huart1;
PCD_HandleTypeDef hpcd_USB_OTG_FS;
static uint16_t XferSize;
static uint32_t Timeout = 10000;

#endif /* SETUP_H_ */
