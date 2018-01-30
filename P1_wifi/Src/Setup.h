/*
 * Setup.h
 *
 *  Created on: Jan 30, 2018
 *      Author: Luke
 */

#ifndef SETUP_H_
#define SETUP_H_

#include "stm32l4xx_hal.h"

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_SPI3_Init(void);
void MX_USART1_UART_Init(void);
void MX_USB_OTG_FS_PCD_Init(void);

/*Public variables*/
SPI_HandleTypeDef hspi3;
UART_HandleTypeDef huart1;
PCD_HandleTypeDef hpcd_USB_OTG_FS;

#endif /* SETUP_H_ */
