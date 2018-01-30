/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Setup.h"
#include "stm32l4xx_hal.h"

/* Private variables ---------------------------------------------------------*/
static uint16_t XferSize = 1000;
static uint32_t SPI_Timeout = 1000;
static uint8_t WIFI_return[1000];

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
  MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_USB_OTG_FS_PCD_Init();

  /* Initialize Wi-Fi module */
  HAL_GPIO_WritePin(GPIOB, ISM43362_WAKEUP_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi3, (uint8_t *)"?\r", (uint16_t)2, SPI_Timeout);
  HAL_SPI_Receive(&hspi3, WIFI_return, XferSize, SPI_Timeout);

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
