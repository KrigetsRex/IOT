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
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "stm32l475e_iot01_tsensor.h"

void SystemClock_Config(void);
void MX_TIM2_Init(void);
void MX_GPIO_Init(void);
void MX_SPI3_Init(void);
void MX_USART1_UART_Init(void);
void MX_USB_OTG_FS_PCD_Init(void);
void append_string(uint8_t num, uint8_t* arr);
void thingSpeakUpdate(uint8_t* wifi_xmit);
void tim2_init(void);

/*Public variables*/
SPI_HandleTypeDef hspi3;
//UART_HandleTypeDef huart1;
TIM_HandleTypeDef htim2;
PCD_HandleTypeDef hpcd_USB_OTG_FS;
static uint16_t XferSize;
static uint32_t Timeout = 10000;

//modes of operation
#define ALL_OFF 0
#define WINDOW_COOL 1
#define WINDOW_HEAT 2
#define AC_ON 3
#define FURNACE_ON 4

//window state
#define OPEN 0
#define CLOSED 1

//timer state
#define TIMER_OFF 0x220
#define TIMER_ON 0x2A1

//error numbers
#define NO_ERROR 0
#define WINDOW_NOT_CLOSED 1
#define WINDOW_NOT_OPEN 2
#define WINDOW_UNK 3
#define FURNACE_NOT_OFF 4
#define FURNACE_NOT_ON 5
#define FURNACE_UNK 6
#define AC_NOT_OFF 7
#define AC_NOT_ON 8
#define AC_UNK 9

#endif /* SETUP_H_ */
