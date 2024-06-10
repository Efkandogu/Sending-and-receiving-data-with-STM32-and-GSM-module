/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "i2c-lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
char GSM_AT_Send(char *Cmd, char CmdSize, char *rData, char RetSize, char trial);

char GSM_AT_Send_SMS(char *phoneNumber, char *message, char *response, char responseSize, uint32_t timeout);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char Uart_Data[255];
char returnData[25];
char Rx_Cnt=0;
char Rx_Flag=0;
char lcd_buf[25];
int count=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(20);
  ssd1306_Init();
  HAL_Delay(20);
  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
  HAL_Delay(20);
  __HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
  HAL_Delay(3000);
  GSM_AT_Send("AT\r\n", 4, "\r\nOK\r\n", 6, 5);
  HAL_Delay(200);
  GSM_AT_Send("AT+CPIN?\r\n", 10,"READY\r\n" ,7, 5);
  HAL_Delay(200);
  GSM_AT_Send("AT+CREG?\r\n", 10,"+CREG: 0,1\r\n" ,7, 5);
  HAL_Delay(200);
  GSM_AT_Send("AT+CMGF=1\r\n", 11,"\r\nOK\r\n", 6, 5);
  HAL_Delay(200);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
	  {

		  ssd1306_Fill(Black);
		 	          ssd1306_SetCursor(0, 0);
		 	          ssd1306_WriteString("Mesaj gonderiliyor: ", Font_7x10, White);
		 	          ssd1306_UpdateScreen();

		 	         char phoneNumber[] = "+905057874146";
		 	         char message[] = "Merhaba, bu bir test mesajidir.";

		 	         if (GSM_AT_Send_SMS(phoneNumber, message, "\r\nOK\r\n", 6, 5000) == 0)
		 	         {
		 	        	ssd1306_SetCursor(0, 0);
		 	        			 	          ssd1306_WriteString("Mesaj gonderildi ", Font_7x10, White);
		 	        			 	          ssd1306_UpdateScreen();
		 	        			 	        HAL_Delay(1000);
		 	         }
		 	         else
		 	         {
		 	        	ssd1306_SetCursor(0, 0);
		 	        			 	          ssd1306_WriteString("sms hatası ", Font_7x10, White);
		 	        			 	          ssd1306_UpdateScreen();
		 	        			 	        HAL_Delay(1000);
		 	         }
	  }
	    HAL_Delay(500);

	  if (GSM_AT_Send("AT+CMGR=1\r\n", 11, "\r\nOK\r\n", 6, 5) == 0)
	    {
	      if (strstr(Uart_Data, "+CMGR") != NULL)
	      {
	        char *startPtr = &Uart_Data[75];
	        char *endPtr = strstr(startPtr, "\r\n");

	        if (startPtr != NULL && endPtr != NULL)
	        {
	          int messageLength = endPtr - startPtr;
	          char message[messageLength + 1];
	          strncpy(message, startPtr, messageLength);
	          message[messageLength] = '\0';

	          ssd1306_Fill(Black);
	          ssd1306_SetCursor(0, 0);
	          ssd1306_WriteString("1 yeni mesaj: ", Font_7x10, White);
	          ssd1306_SetCursor(0, 12);
	          ssd1306_WriteString(message, Font_7x10, White);
	          ssd1306_UpdateScreen();
	          HAL_Delay(5000);

	          // Mesajı sil
	          GSM_AT_Send("AT+CMGD=1\r\n", 12, "\r\nOK\r\n", 6, 5);

	          // Mesaj hafızasını güncelle
	          HAL_Delay(1000);
	          memset(Uart_Data, 0, sizeof(Uart_Data));
	          GSM_AT_Send("AT+CMGL=\"REC UNREAD\"\r\n", 22, "\r\nOK\r\n", 6, 5);
	        }
	      }
	      else
	      {

	        // Mesaj hafızasını güncelle
	        HAL_Delay(1000);
	        memset(Uart_Data, 0, sizeof(Uart_Data));
	        GSM_AT_Send("AT+CMGL=\"REC UNREAD\"\r\n", 22, "\r\nOK\r\n", 6, 5);
	      }
	      memset(Uart_Data, 0, sizeof(Uart_Data));
	    }
	  ssd1306_Fill(Black);
	   ssd1306_SetCursor(0, 0);
		        ssd1306_WriteString("Mesajiniz Yok", Font_7x10, White);
		        ssd1306_UpdateScreen();
		        HAL_Delay(1000);
	    HAL_Delay(500);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : Push_Button_Pin */
  GPIO_InitStruct.Pin = Push_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Push_Button_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
char GSM_AT_Send(char *Cmd, char CmdSize, char *rData, char RetSize, char trial)
{
	char cnt=0;

	memset(returnData,0x00,sizeof(returnData));
	memcpy(returnData,rData,RetSize);

	while(cnt<trial)
	{

		Rx_Cnt=0;
		memset(Uart_Data,0x00,sizeof(Uart_Data));
		HAL_UART_Transmit(&huart2, Cmd, CmdSize, 50);
		HAL_Delay(500);
		if(Rx_Flag)
		{
			Rx_Flag=0;
			return 0;
		}

		cnt++;

	}

	return 1;
}
char GSM_AT_Send_SMS(char *phoneNumber, char *message, char *response, char responseSize, uint32_t timeout)
{
    char command[32];
    memset(command, 0x00, sizeof(command));
    snprintf(command, sizeof(command), "AT+CMGS=\"%s\"\r\n", phoneNumber);

    char ctrlZ[] = { 0x1A, '\0' };

    uint32_t startMillis = HAL_GetTick();
    while (HAL_GetTick() - startMillis < timeout)
    {
        memset(Uart_Data, 0x00, sizeof(Uart_Data));

        HAL_UART_Transmit(&huart2, (uint8_t*)command, strlen(command), timeout);
        HAL_Delay(1000);

        if (strstr(Uart_Data, ">") != NULL)
        {
            HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), timeout);
            HAL_UART_Transmit(&huart2, (uint8_t*)ctrlZ, 1, timeout);

            uint32_t responseStartMillis = HAL_GetTick();
            while (HAL_GetTick() - responseStartMillis < timeout)
            {
                if (strstr(Uart_Data, response) != NULL)
                {
                    return 0;
                }
            }

            break;
        }
    }

    return 1;
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
