/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
UART_HandleTypeDef huart1;
I2C_HandleTypeDef hi2c1;
uint8_t out[] = {0,0,'.',0,0,'\r','\n'};
int thresholdi;
int ready = 0;
float currenttempf;
uint8_t threshold[4] = {'-', '1'};
int on = 0;
uint8_t f[10];
uint8_t newline[2] = {10, 13};
uint8_t tempbuffer1[2], tempbuffer2[2];
int i = 0;
uint8_t hexToAscii(uint8_t n) //4-bit hex value converted to an ascii character
{
    if (n >= 0 && n <= 9)
        n = n + '0';
    else
        n = n - 10 + 'A';
    return n;
}
float hextoint(uint8_t* in)
{
	float out = 0.0;
	if(in[0]>=48 && in[0]<=57)
		out+=((in[0]-'0')*16);
	else out+=((in[0]-'A'+10)*16);
	if(in[1]>=48 && in[1]<=57)
		out+=(in[1]-'0');
	else out+=(in[1]-'A'+10);
	if(in[3]>=48 && in[3]<=57)
		out+=((in[1]-'0')/16.0);
	else out+=((in[1]-'A'+10)/16.0);
	return out;
	
}
void ReadTemp(void)
{
    if (ready)
    {
        uint8_t controlReg[2], statusReg;
        statusReg = 0x0F;
        HAL_I2C_Master_Receive(&hi2c1, 0xD0, &statusReg, 1, 10);
        if (!(statusReg & 0x04))
        {
            controlReg[0] = 0x0E;
            controlReg[1] = 0x3F;
            HAL_I2C_Master_Transmit(&hi2c1, 0xD0, controlReg, 2, 10);
        }

        //send temp register address 11h to read from
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempbuffer1, 1, 10);
        //read data of register 11h to tempbuffer1[1]
        HAL_I2C_Master_Receive(&hi2c1, 0xD1, tempbuffer1 + 1, 1, 10);
        //send temp register address 12h to read from
        HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempbuffer2, 1, 10);
        //read data of register 12h to tempbuffer2[1]
        HAL_I2C_Master_Receive(&hi2c1, 0xD1, tempbuffer2 + 1, 1, 10);
				out[3] = hexToAscii(tempbuffer2[1] >> 4 );
				out[4] = hexToAscii(tempbuffer2[1] & 0x0F );
				out[0] = hexToAscii((tempbuffer1[1] >> 4) & 1);
				out[1] = hexToAscii(tempbuffer1[1] & 0x0F);
				
        currenttempf = hextoint(out);
        if (currenttempf > thresholdi)
            on = 1;
        else
            on = 0;
        volatile taskStruct t = {&ReadTemp, 1, 6};
        rerunMe(&delayed, t);
    }
    else
    {
        volatile taskStruct t = {&ReadTemp, 1, 4};
        rerunMe(&delayed, t);
        thresholdi = atoi((char *)threshold);
        if (thresholdi > -1)
        {
            HAL_UART_Transmit(&huart1, newline, sizeof(newline), 10);
            ready = 1;
        }
    }
    //control registers to force the temprature conversion
}

void TriggerAlarm(void)
{
    if (on)
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
    else
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
    volatile taskStruct t = {&TriggerAlarm, 2, 4};
    rerunMe(&delayed, t);
}

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
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
    MX_USART1_UART_Init();
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    HAL_InitTick(TICK_INT_PRIORITY);
    SysTick_Config(SystemCoreClock / 20);
    init();
    enqueue(&ReadTemp, 1, 1);
    enqueue(&TriggerAlarm, 2, 1);
    tempbuffer1[0] = 0x11;
    tempbuffer2[0] = 0x12;
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempbuffer1, 2, 10);
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, tempbuffer2, 2, 10);
    while (1)
    {
        /* USER CODE END WHILE */

        if (i)
        {
					dispatch();
					i=0;
        }
        /* USER CODE END 3 */
    }

    /**
  * @brief System Clock Configuration
  * @retval None
  */
	}
    void SystemClock_Config(void)
    {
        RCC_OscInitTypeDef RCC_OscInitStruct = {0};
        RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
        RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

        /** Configure LSE Drive Capability
  */
        HAL_PWR_EnableBkUpAccess();
        __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
        /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
        RCC_OscInitStruct.LSEState = RCC_LSE_ON;
        RCC_OscInitStruct.MSIState = RCC_MSI_ON;
        RCC_OscInitStruct.MSICalibrationValue = 0;
        RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        {
            Error_Handler();
        }
        /** Initializes the CPU, AHB and APB buses clocks
  */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
        {
            Error_Handler();
        }
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1;
        PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
        PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        {
            Error_Handler();
        }
        /** Configure the main internal regulator output voltage
  */
        if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
        {
            Error_Handler();
        }
        /** Enable MSI Auto calibration
  */
        HAL_RCCEx_EnableMSIPLLMode();
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
        hi2c1.Init.Timing = 0x00000E14;
        hi2c1.Init.OwnAddress1 = 0;
        hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        hi2c1.Init.OwnAddress2 = 0;
        hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
        hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
        if (HAL_I2C_Init(&hi2c1) != HAL_OK)
        {
            Error_Handler();
        }
        /** Configure Analogue filter
  */
        if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
        {
            Error_Handler();
        }
        /** Configure Digital filter
  */
        if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
        {
            Error_Handler();
        }
        /* USER CODE BEGIN I2C1_Init 2 */

        /* USER CODE END I2C1_Init 2 */
    }

    /**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
    static void MX_USART1_UART_Init(void)
    {

        /* USER CODE BEGIN USART1_Init 0 */

        /* USER CODE END USART1_Init 0 */

        /* USER CODE BEGIN USART1_Init 1 */

        /* USER CODE END USART1_Init 1 */
        huart1.Instance = USART1;
        huart1.Init.BaudRate = 9600;
        huart1.Init.WordLength = UART_WORDLENGTH_8B;
        huart1.Init.StopBits = UART_STOPBITS_1;
        huart1.Init.Parity = UART_PARITY_NONE;
        huart1.Init.Mode = UART_MODE_TX_RX;
        huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart1.Init.OverSampling = UART_OVERSAMPLING_16;
        huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
        if (HAL_UART_Init(&huart1) != HAL_OK)
        {
            Error_Handler();
        }
        /* USER CODE BEGIN USART1_Init 2 */

        /* USER CODE END USART1_Init 2 */
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
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

        /*Configure GPIO pin : PA3 */
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    /* USER CODE BEGIN 4 */

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

#ifdef USE_FULL_ASSERT
    /**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
    void assert_failed(uint8_t * file, uint32_t line)
    {
        /* USER CODE BEGIN 6 */
        /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
        /* USER CODE END 6 */
    }
#endif /* USE_FULL_ASSERT */

    /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
