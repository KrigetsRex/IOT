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
#include "stm32l475e_iot01_accelero.h"
#include "stm32l4xx_hal.h"
#include "setup.h"

/* Private variables ---------------------------------------------------------*/
int16_t pDataXYZ[3] = {0};
int16_t qDataXYZ[3] = {0};
uint8_t DeltaX;
uint8_t DeltaY;
uint8_t DeltaZ;
static char* WIFI_xmit[68];
static uint8_t WIFI_X_xmit[] = "https://api.thingspeak.com/update?api_key=YF7HOW1VSKR4Y8H8&field1=000";
static uint8_t WIFI_Y_xmit[] = "https://api.thingspeak.com/update?api_key=YF7HOW1VSKR4Y8H8&field2=000";
static uint8_t WIFI_Z_xmit[] = "https://api.thingspeak.com/update?api_key=YF7HOW1VSKR4Y8H8&field3=000";
static uint16_t failed;
static WIFI_Status_t stat;
const char* server = "api.thingspeak.com";
uint8_t  IP_Addr[4] = {184,106,153,149};

/* Private function prototypes -----------------------------------------------*/

/**
  * @breif: start wifi and upload accelerometer data if bump detected
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
  BSP_ACCELERO_Init();
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
	  BSP_ACCELERO_AccGetXYZ(pDataXYZ);
	  BSP_ACCELERO_AccGetXYZ(qDataXYZ);
	  DeltaX = abs_val(qDataXYZ[0] - pDataXYZ[0]);
	  DeltaY = abs_val(qDataXYZ[1] - pDataXYZ[1]);
	  DeltaZ = abs_val(qDataXYZ[2] - pDataXYZ[2]);

	  if ((DeltaX > 20) | (DeltaY > 20) | (DeltaZ > 20)){
		  /*append_string(DeltaX, WIFI_X_xmit);
		  append_string(DeltaY, WIFI_Y_xmit);
		  append_string(DeltaZ, WIFI_Z_xmit);
		  WIFI_SendData((uint8_t)0, WIFI_X_xmit, sizeof(WIFI_X_xmit), &XferSize, Timeout);
		  WIFI_SendData((uint8_t)0, WIFI_Y_xmit, sizeof(WIFI_Y_xmit), &XferSize, Timeout);
		  WIFI_SendData((uint8_t)0, WIFI_Z_xmit, sizeof(WIFI_Z_xmit), &XferSize, Timeout);
		  */
		  sprintf(WIFI_xmit, "field1=%u&field2=%u&field3=%u", DeltaX,DeltaY,DeltaZ);
		  thingSpeakUpdate(WIFI_xmit);
	  }
  }
}
