/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ESTADO_VER_RELOGIO 0
#define ESTADO_AJUSTA_HORA 1
#define ESTADO_AJUSTA_DATA 2
#define ESTADO_AGUARDA_HORA 3
#define ESTADO_AGUARDA_DATA 4
#define ESTADO_PROCESSA_OPCAO 5
#define ESTADO_ESPERA_COMANDO 6
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
RTC_TimeTypeDef horaAtual;
RTC_DateTypeDef dataAtual;
uint8_t uartRx[6];
uint8_t estado_atual = ESTADO_ESPERA_COMANDO;

uint8_t transicao_ESTADO_AGUARDA_HORA = 0;
uint8_t transicao_ESTADO_AGUARDA_DATA = 0;
uint8_t transicao_ESTADO_ESPERA_COMANDO = 1;

char relogio[30];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //printf("Teste\r\n");
	  switch(estado_atual){
		case ESTADO_VER_RELOGIO:
			HAL_RTC_GetTime(&hrtc, &horaAtual, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &dataAtual, RTC_FORMAT_BIN);
			sprintf(relogio, "%0*i/%0*i/20%0*i - %0*i:%0*i:%0*i\n\r", 2, dataAtual.Date, 2, dataAtual.Month, 2, dataAtual.Year, 2, horaAtual.Hours, 2, horaAtual.Minutes, 2, horaAtual.Seconds);
			//printf("%s", relogio);
			HAL_UART_Transmit(&huart2, (uint8_t*)relogio, strlen(relogio), 1000);
			estado_atual = ESTADO_ESPERA_COMANDO;
			transicao_ESTADO_ESPERA_COMANDO = 1;
			break;
		case ESTADO_AJUSTA_HORA:
			horaAtual.Hours = (((uint8_t)uartRx[0]-48)*10) + ((uint8_t)uartRx[1]-48);
			horaAtual.Minutes = (((uint8_t)uartRx[2]-48)*10) + ((uint8_t)uartRx[3]-48);
			horaAtual.Seconds = (((uint8_t)uartRx[4]-48)*10) + ((uint8_t)uartRx[5]-48);
			HAL_RTC_SetTime(&hrtc, &horaAtual, RTC_FORMAT_BIN);
			char messageHoraAjustada[] = "Hora Ajustada:\r\n";
			printf("%s", messageHoraAjustada);
			//HAL_UART_Transmit(&huart2, (uint8_t*)messageHoraAjustada, strlen(messageHoraAjustada), 1000);
			estado_atual = ESTADO_VER_RELOGIO;
			break;
		case ESTADO_AJUSTA_DATA:
			dataAtual.Date = (((int)uartRx[0]-48)*10) + ((int)uartRx[1]-48);
			dataAtual.Month = (((int)uartRx[2]-48)*10) + ((int)uartRx[3]-48);
			dataAtual.Year = (((int)uartRx[4]-48)*10) + ((int)uartRx[5]-48);
			HAL_RTC_SetDate(&hrtc, &dataAtual, RTC_FORMAT_BIN);
			char messageDataAjustada[] = "Data Ajustada:\r\n";
			printf("%s", messageDataAjustada);
			//HAL_UART_Transmit(&huart2, (uint8_t*)messageDataAjustada, strlen(messageDataAjustada), 1000);
			estado_atual = ESTADO_VER_RELOGIO;
			break;
		case ESTADO_AGUARDA_HORA:
			if(transicao_ESTADO_AGUARDA_HORA == 1){
				transicao_ESTADO_AGUARDA_HORA = 0;
				char message[] = "Digite a nova hora no formato: HHMMSS\r\n";
				printf("%s", message);
				//HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 1000);
				HAL_UART_Receive_IT(&huart2, uartRx, 6);
			}
			break;
		case ESTADO_AGUARDA_DATA:
			if(transicao_ESTADO_AGUARDA_DATA == 1){
				transicao_ESTADO_AGUARDA_DATA = 0;
				char message[] = "Digite a nova data no formato: DDMMAA\r\n";
				printf("%s", message);
				//HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 1000);
				HAL_UART_Receive_IT(&huart2, uartRx, 6);
			}
			break;
		case ESTADO_PROCESSA_OPCAO:
			switch(uartRx[0]){
				case (uint8_t)'H':
					estado_atual = ESTADO_AGUARDA_HORA;
					transicao_ESTADO_AGUARDA_HORA = 1;
					break;
				case (uint8_t)'D':
					estado_atual = ESTADO_AGUARDA_DATA;
					transicao_ESTADO_AGUARDA_DATA = 1;
					break;
				case (uint8_t)'V':
					estado_atual = ESTADO_VER_RELOGIO;
					break;
				default:;
					char message[] = "Comando Invalido. Por favor digite H, D ou V";
					printf("%s", message);
					//HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 1000);
					estado_atual = ESTADO_ESPERA_COMANDO;
					transicao_ESTADO_ESPERA_COMANDO = 1;
					break;
			}
			break;
		case ESTADO_ESPERA_COMANDO:
			if(transicao_ESTADO_ESPERA_COMANDO == 1){
				HAL_UART_Receive_IT(&huart2, uartRx, 1);
				transicao_ESTADO_ESPERA_COMANDO = 0;
			}
			break;
		default:;
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int __io_putchar(int ch) {
	uint8_t c[1];
	c[0] = ch & 0x00FF;
	HAL_UART_Transmit(&huart2, &*c, 1, 10);
	return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	switch(estado_atual){
		case ESTADO_VER_RELOGIO:
			break;
		case ESTADO_AJUSTA_HORA:
			break;
		case ESTADO_AJUSTA_DATA:
			break;
		case ESTADO_AGUARDA_HORA:
			estado_atual = ESTADO_AJUSTA_HORA;
			break;
		case ESTADO_AGUARDA_DATA:
			estado_atual = ESTADO_AJUSTA_DATA;
			break;
		case ESTADO_PROCESSA_OPCAO:
			break;
		case ESTADO_ESPERA_COMANDO:
			estado_atual = ESTADO_PROCESSA_OPCAO;
			break;
		default:;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
