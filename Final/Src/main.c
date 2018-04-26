/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** Temperature control for a house using an actuated window, AC, and furnace
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "dfsdm.h"
//#include "quadspi.h"
//#include "usart.h"
#include "usb_otg.h"
#include "Setup.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t WIFI_xmit[68];
static WIFI_Status_t stat;
static const char* server = "api.thingspeak.com";
static const uint8_t  IP_Addr[4] = {184,106,153,149};
static uint16_t ext_temp = 0;
static float int_temp = 0;
static uint8_t desired_temp = 21;
static uint8_t mode = ALL_OFF;
static uint8_t errorNum = NO_ERROR;
static uint8_t windowState = CLOSED;
static uint8_t errorCorrection = 0;

/* Private functions -----------------------------------------------*/
void openWindow(){
	//manual pin toggling
	for (uint8_t count = 0; count < 50; count++){
		HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_SET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_RESET);
		HAL_Delay(2);
	}

	//pwm
//	TIM2->ARR = 4;
//	TIM2->CCR1 = 2;
//	TIM2->CR1 = TIMER_ON;
//	HAL_Delay(10);
//	TIM2->CR1 = TIMER_OFF;
}

void closeWindow(){
	//manually toggling pin
	for (uint8_t count = 0; count < 50; count++){
		HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_RESET);
		HAL_Delay(1);
	}

	//pwm
//	TIM2->ARR = 2;
//	TIM2->CCR1 = 1;
//	TIM2->CR1 = TIMER_ON;
//	HAL_Delay(10);
//	TIM2->CR1 = TIMER_OFF;
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
	uint8_t buf[2];
	//HAL_GPIO_WritePin(GPIOC,ARD_D10_Pin,GPIO_PIN_RESET);
	HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi1, buf, 2, 1000);
	//HAL_GPIO_WritePin(GPIOC,ARD_D10_Pin,GPIO_PIN_SET);
	temp = (((uint16_t)buf[1]) << 5) + (buf[0] >> 3);
	if (status == HAL_OK){
		return temp;
	}
	else{
		return -1;
	}
}

uint8_t getWindowState(){
	uint8_t state = 2;
	HAL_GPIO_WritePin(GPIOC,ARD_A3_Pin,GPIO_PIN_SET);
	GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOC,ARD_A4_Pin);
	if (pinState == GPIO_PIN_SET){
		state = CLOSED;
	}
	else if (pinState == GPIO_PIN_RESET){
		state = OPEN;
	}
	HAL_GPIO_WritePin(GPIOC,ARD_A3_Pin,GPIO_PIN_RESET);
	return state;
}

void fixWindow(){
	switch (errorNum){
	case WINDOW_NOT_OPEN:
		openWindow();
		break;
	case WINDOW_NOT_CLOSED:
		closeWindow();
		break;
	default:
		break;
	}
	HAL_Delay(15000);  //takes ~12 sec
	windowState = getWindowState();
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
  //MX_DFSDM1_Init();
  MX_I2C2_Init();
  //MX_QUADSPI_Init();
  //MX_SPI3_Init();
  MX_SPI1_Init();
  //MX_USART1_UART_Init();
  //MX_USART3_UART_Init();
  //MX_USB_OTG_FS_PCD_Init();
  BSP_TSENSOR_Init();
  //MX_TIM2_Init();
  //tim2_init();
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
  SPI_WIFI_Init();
  WIFI_Init();

  //functionality test
  ext_temp = getExtTemp();
  BSP_TSENSOR_ReadTemp(&int_temp);
  openWindow();
  closeWindow();
  furnaceON();
  furnaceOFF();
  acON();
  acOFF();

  //Connect to Access Point
  stat = WIFI_Connect("LukeandMichelle","10122010",  WIFI_ECN_WPA2_PSK);
  if (stat != WIFI_STATUS_OK){
	  stat = WIFI_Connect("iotclass","myiotclass",  WIFI_ECN_WPA2_PSK);
  }

  while (1){
	  BSP_TSENSOR_ReadTemp(&int_temp);
	  ext_temp = getExtTemp();
	  windowState = getWindowState();
	  //correct temperature
	  int_temp = int_temp - 3;

	  //check temp
	  if (int_temp < desired_temp - 1 || int_temp > desired_temp + 1){
		  switch (mode){
		  case ALL_OFF:
			  if (int_temp > desired_temp){ //too hot
				  if (ext_temp < int_temp){ //cool outside
					  openWindow();
					  mode = WINDOW_COOL;
				  }
				  else{  //hot outside
					  acON();
					  mode = AC_ON;
				  }
			  }
			  else{  //too cold
				  if (ext_temp > int_temp){ //hot outside
					  openWindow();
					  mode = WINDOW_HEAT;
				  }
				  else{  //cold outside
					  furnaceON();
					  mode = FURNACE_ON;
				  }
			  }
			  break;
		  case WINDOW_COOL || WINDOW_HEAT:
			  if (windowState != OPEN){
				  windowState = getWindowState();
				  if (windowState == CLOSED){
					  errorNum = WINDOW_NOT_OPEN;
				  }
				  else if (windowState == OPEN){
					  errorNum = NO_ERROR;
				  }
				  else{
					  errorNum = WINDOW_UNK;
				  }
			  }
			  else{
				  errorNum = NO_ERROR;
			  }
			  break;
		  case AC_ON:
			  //TODO: figure out how to check
			  errorNum = AC_UNK;
			  break;
		  case FURNACE_ON:
			  //TODO: figure out how to check
			  errorNum = FURNACE_UNK;
			  break;
		  }
	  }
	  else if (mode != ALL_OFF) {
		  if (windowState != CLOSED){
			  closeWindow();
		  }
		  acOFF();
		  furnaceOFF();
		  mode = ALL_OFF;
	  }

	  //error correction
	  if (errorCorrection){
		  switch (errorNum){
		  case NO_ERROR:
			  break;
		  case WINDOW_NOT_CLOSED:
			  fixWindow();
			  if (windowState == CLOSED){
				  errorNum = NO_ERROR;
			  }
			  break;
		  case WINDOW_NOT_OPEN:
			  fixWindow();
			  if (windowState == OPEN){
				  errorNum = NO_ERROR;
			  }
			  break;
		  case WINDOW_UNK:
			  windowState = getWindowState();
			  switch(windowState){
			  case OPEN:
				  if (mode == WINDOW_COOL || mode == WINDOW_HEAT){
					 errorNum = NO_ERROR;
				  }
				  else{
					  errorNum = WINDOW_NOT_CLOSED;
					  fixWindow();
					  if (windowState == CLOSED){
						  errorNum = NO_ERROR;
					  }
				  }
				  break;
			  case CLOSED:
				  if (mode == WINDOW_COOL || mode == WINDOW_HEAT){
					 errorNum = WINDOW_NOT_OPEN;
					 fixWindow();
					 if (windowState == OPEN){
						 errorNum = NO_ERROR;
					 }
				  }
				  else {
					  errorNum = NO_ERROR;
				  }
				  break;
			  default:
				  break;
			  }
			  break;
		  case FURNACE_NOT_OFF:
			  furnaceOFF();
			  break;
		  case FURNACE_NOT_ON:
			  furnaceON();
			  break;
		  case FURNACE_UNK:
			  errorNum = NO_ERROR;
			  break;
		  case AC_NOT_OFF:
			  acOFF();
			  break;
		  case AC_NOT_ON:
			  acON();
			  break;
		  case AC_UNK:
			  errorNum = NO_ERROR;
			  break;
		  }
		  errorCorrection = 0;
	  }


	  //reporting
	  sprintf(WIFI_xmit, "field1=%u&field2=%.2f&field3=%u&field4=%u",
	  mode, int_temp, ext_temp, errorNum);
	  WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, server, IP_Addr, 80, 0);
	  thingSpeakUpdate(WIFI_xmit);
	  WIFI_CloseClientConnection(0);
	  HAL_Delay(15000);
  }
}

void ISR(USER_BUTTON_EXTI_IRQn){
	errorCorrection = 1;
}
