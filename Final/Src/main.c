/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "dfsdm.h"
#include "i2c.h"
#include "quadspi.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l4xx_hal.h"
#include "setup.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t WIFI_xmit[68];
static WIFI_Status_t stat;
const char* server = "api.thingspeak.com";
uint8_t  IP_Addr[4] = {184,106,153,149};
float ext_temp = 0;
float int_temp = 0;
uint8_t mode = 0;
uint8_t errorNum = 0;

/* Private functions -----------------------------------------------*/
void openWindow(){
	HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_SET);
	HAL_delay(1);
	HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_RESET);
}

void closeWindow(){
	HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_SET);
	HAL_delay(2);
	HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_RESET);
}

void furnaceON(){
	HAL_GPIO_WritePin(GPIOC,ARD_A0_Pin,GPIO_PIN_SET);
}

void furnaceOFF(){
	HAL_GPIO_WritePin(GPIOC,ARD_A0_Pin,GPIO_PIN_RESET);
}

void acON(){
	HAL_GPIO_WritePin(GPIOC,ARD_A1_Pin,GPIO_PIN_RESET);
}

void acOFF(){
	HAL_GPIO_WritePin(GPIOC,ARD_A1_Pin,GPIO_PIN_SET);
}

uint16_t getExtTemp(){
	uint16_t temp = -1;
	HAL_SPI_Receive(&hspi1, &temp, 12, 1);
	return temp;
}

/**
  * @breif: main function which sets up peripherals and periodically
  * monitors the internal and external temp and performs the appropriate
  * action as required
  *
  * @retval None
  */
int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DFSDM1_Init();
  MX_I2C2_Init();
  MX_QUADSPI_Init();
  //MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  BSP_TSENSOR_Init();
  SPI_WIFI_Init();
  WIFI_Init();

  //Connect to Access Point
  stat = WIFI_Connect("LukeandMichelle","10122010",  WIFI_ECN_WPA2_PSK);
  if (stat != WIFI_STATUS_OK){
	  stat = WIFI_Connect("iotclass","myiotclass",  WIFI_ECN_WPA2_PSK);
  }

  //Connect to ThingSpeak
  WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, server, IP_Addr, 80, 0);
  //WIFI_SendData((uint8_t)0, WIFI_connection, sizeof(WIFI_connection), &XferSize, Timeout);

  while (1){
	  BSP_TSENSOR_ReadTemp(&int_temp);
	  sprintf(WIFI_xmit, "field1=%u&field2=%.2f&field3=%.2f&field4=%u",
	  mode, int_temp, ext_temp, errorNum);
	  WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, server, IP_Addr, 80, 0);
	  thingSpeakUpdate(WIFI_xmit);
	  WIFI_CloseClientConnection(0);
	  HAL_Delay(15000);
  }
}
