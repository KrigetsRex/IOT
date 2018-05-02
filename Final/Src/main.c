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
#include "usb_otg.h"
#include "Setup.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t WIFI_xmit[68];
static WIFI_Status_t stat;
static const char* server = "api.thingspeak.com";
static const uint8_t  IP_Addr[4] = {184,106,153,149};
static float ext_temp = 0;
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
}

void closeWindow(){
	//manually toggling pin
	for (uint8_t count = 0; count < 50; count++){
		HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOC,ARD_A2_Pin,GPIO_PIN_RESET);
		HAL_Delay(1);
	}
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

/*Thermocouple getExtTemp method*/
//float getExtTemp(){
//	float temp = -1;
//	uint8_t buf[2];
//	buf[0] = 0;
//	buf[1] = 0;
//	HAL_GPIO_WritePin(GPIOC,ARD_D10_Pin,GPIO_PIN_RESET);
//	HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi1, buf, 2, 1000);
//	HAL_GPIO_WritePin(GPIOC,ARD_D10_Pin,GPIO_PIN_SET);
//	temp = (((uint16_t)buf[1]) << 5) + (buf[0] >> 3);
//	temp = temp / 4;
//	if (status == HAL_OK){
//		return temp;
//	}
//	else{
//		return -1;
//	}
//}

float getExtTemp(){
	float temp = -1;
	HAL_GPIO_WritePin(GPIOA,ARD_D12_Pin,GPIO_PIN_SET);
	HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi1, &temp, 4, 1000);
	HAL_GPIO_WritePin(GPIOA,ARD_D12_Pin,GPIO_PIN_RESET);
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
	HAL_GPIO_WritePin(GPIOC,ARD_A3_Pin,GPIO_PIN_RESET);
	if (pinState == GPIO_PIN_SET){
		state = CLOSED;
	}
	else if (pinState == GPIO_PIN_RESET){
		state = OPEN;
	}
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
  MX_SPI1_Init();
  MX_GPIO_Init();
  MX_I2C2_Init();
  BSP_TSENSOR_Init();
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
  BSP_LED_Init(LED2);
  SPI_WIFI_Init();
  WIFI_Init();

  //Connect to Access Point
  stat = WIFI_Connect("LukeandMichelle","10122010",  WIFI_ECN_WPA2_PSK);
  if (stat != WIFI_STATUS_OK){
	  stat = WIFI_Connect("iotclass","myiotclass",  WIFI_ECN_WPA2_PSK);
  }

  while (1){
	  BSP_TSENSOR_ReadTemp(&int_temp);
	  ext_temp = getExtTemp();
	  windowState = getWindowState();

	  if (windowState){
		  ext_temp = 40;
	  }

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
					  //if(windowState == OPEN){
						  closeWindow();
					  //}
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
					  //if(windowState == OPEN){
						  closeWindow();
					  //}
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
					  BSP_LED_On(LED2);
				  }
				  else if (windowState == OPEN){
					  errorNum = NO_ERROR;
				  }
				  else{
					  errorNum = WINDOW_UNK;
					  BSP_LED_On(LED2);
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
		  //if (windowState != CLOSED){
			  closeWindow();
		  //}
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
		  BSP_LED_Off(LED2);
		  errorCorrection = 0;
	  }


	  //reporting
	  if (stat == WIFI_STATUS_OK){
		  HAL_Delay(5000);
		  sprintf(WIFI_xmit, "field1=%u&field2=%u&field3=%u&field4=%u",
		  (int8_t)int_temp, (int8_t)ext_temp, mode, errorNum);
		  WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, server, IP_Addr, 80, 0);
		  thingSpeakUpdate(WIFI_xmit);
		  WIFI_CloseClientConnection(0);
		  HAL_Delay(5000);
	  }
	  HAL_Delay(5000);
  }
}

void ISR(USER_BUTTON_EXTI_IRQn){
	errorCorrection = 1;
}
