/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Setup.h"
#include "stm32l4xx_hal.h"
#include "wifi.h"

/* Private variables ---------------------------------------------------------*/
static uint16_t XferSize;
static uint32_t Timeout = 10000;
static uint8_t WIFI_xmit[] = "https://api.thingspeak.com/update?api_key=YF7HOW1VSKR4Y8H8&field1=7";
static uint16_t failed;
static WIFI_Status_t stat;
const int channelID = 426930;
const uint8_t writeAPIKey[] = "YF7HOW1VSKR4Y8H8"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
uint8_t  IP_Addr[4] = {184,106,153,149};
static uint8_t WIFI_connection[] = "POST /update HTTP/1.1\n"
		                           "Host: api.thingspeak.com\n"
								   "User-Agent: ESP8266 (nothans)/1.0\n"
								   "Connection: close\n"
								   "X-THINGSPEAKAPIKEY: YF7HOW1VSKR4Y8H8\n"
								   "Content-Type: application/x-www-form-urlencoded\n"
								   "Content-Length: 1\n\n1";

/* Private function prototypes -----------------------------------------------*/


/**
  * @brief  The application entry point.
  *
  * @retval 0 if exited normally else error
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
  //MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  //HAL_GPIO_WritePin(GPIOB, ISM43362_WAKEUP_Pin, GPIO_PIN_SET);

  /* Initialize Wi-Fi module */
  failed = (uint16_t)SPI_WIFI_Init();
  stat = WIFI_Init();


  /* Debug Stuff */
  /*HAL_SPI_Transmit(&hspi3, (uint8_t *)"F0", (uint16_t)2, SPI_Timeout);
  SPI_WIFI_Delay(1000);
  HAL_SPI_Receive(&hspi3, WIFI_return, XferSize, SPI_Timeout);
  failed = SPI_WIFI_SendData((uint8_t *)"F0", (uint16_t)2, SPI_Timeout);
  SPI_WIFI_Delay(1000);
  failed = SPI_WIFI_ReceiveData(WIFI_return, XferSize, SPI_Timeout);
  WIFI_APs_t* APs;
  stat = WIFI_ListAccessPoints(APs, (uint8_t)15);
  */

  //Connect to Access Point
  stat = WIFI_Connect("LukeandMichelle","10122010",  WIFI_ECN_WPA2_PSK);
  if (stat != WIFI_STATUS_OK){
	  stat = WIFI_Connect("iotclass","myiotclass",  WIFI_ECN_WPA2_PSK);
  }

  //send Data to Think Speak
  //WIFI_StartServer(0, WIFI_TCP_PROTOCOL, "", 80);
  //stat = WIFI_StartServer((uint32_t)0, WIFI_TCP_PROTOCOL, "", (uint32_t)80);
  stat = WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, server, IP_Addr, 80, 0);
  WIFI_SendData((uint8_t)0, WIFI_connection, sizeof(WIFI_connection), &XferSize, Timeout);
  WIFI_SendData((uint8_t)0, WIFI_xmit, sizeof(WIFI_xmit), &XferSize, Timeout);

  /* Infinite loop */
  while (1)
  {


  }
}

/* notes
 * SPI I/O functions:
 * HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size,
uint32_t Timeout);

* GPIO write_pin
*HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
*wi-fi wakeup on pin PB13 ->
*HAL_GPIO_WritePin(GPIOB, ISM43362_WAKEUP_Pin, GPIO_PIN_SET);
*/
