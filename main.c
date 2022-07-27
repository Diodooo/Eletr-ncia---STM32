/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 4422 Diogo da Silveira e Pedro LuÃ­s Ourique dos Santos Francisco
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
// CÃ³digo desenvolvido para a matÃ©ria de Sistemas Microprocessados do curso de EletrÃ´nica
// O mesmo busca integrar os valores de centimetragem, sensor de rÃ© e direÃ§Ã£o do pisca em
//um display NOKIA..
/* PerifÃ©ricos Utilizados:
 * - PWM
 * - IC
 * - OC
 * - GPIO
 * - TIM
 * - ENCODER
 * Extras:
 * - Controle de Sensor HC - SR04
 * - UtilizaÃ§Ã£o do Display NOKIA
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "nokia5110_LCD.h" //BIBLIOTECA NOKIA - DisponÃ­vel em: https://github.com/Zeldax64/Nokia-LCD5110-HAL (Todos os creditos relacionados as funÃ§Ãµes NOKIA
						   //					sÃ£o destinados ao autor Caio Rodrigo)
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TAM_MSG 100
#define TIMEOUT 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t rot = 0;                                                          //ENCODER
uint8_t direcao = 0;                                                       //ENCODER
uint16_t count = 0;                                                        //ENCODER
uint8_t tmed = 0;                                                          //SENSOR
float borda[2] = {0,0};                                                    //SENSOR
float periodo = 0;                                                         //SENSOR
float re =  0;                                                   		   //SENSOR
float distancia = 0;                                                       //ENCODER
char msg[TAM_MSG] = {"Inicializando codigo, por favor aguarde...\n\r"};    //SENSOR
char dir[20] = {"Direita"};                                                //ENCODER
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int debounce (int chave, long estado) 								    //FUNÃ‡ÃƒO PARA LEITURA DA TROCA DE ESTADO
{                                   									//Ã‰ CAPAZ DE LER ALTERAÃ‡Ã•ES POSITIVAS E NEGATIVAS NO CNT DO ENCODER
    static long estados[1] = {0}; 										//VETOR DE 1 "SLOT"
    if (estados[chave] == estado) 										//ESTADOS ANTERIOR E ATUAL IGUAIS?
    {
        return 0;														//FUNÃ‡ÃƒO = FALSA
    }
    estados[chave] = estado;											//SE SÃƒO DIFERENTES IGUALA O ESTADO ATUAL AO ANTETERIOR
    return 1;															//FUNÃ‡ÃƒO = TRUE
}

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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);				//DISPARO DO SENSOR HC - SR04
  HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_2);			//LEITURA DO SENSOR HC - SR04
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);			//LEITURA DO ENCODER
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_2);			//LEITURA DO ENCODER

  //Configs do Display
   LCD_setRST(GPIOB, GPIO_PIN_13);
   LCD_setCE(GPIOB, GPIO_PIN_14);
   LCD_setDC(GPIOB, GPIO_PIN_15);
   LCD_setDIN(GPIOB, GPIO_PIN_1);
   LCD_setCLK(GPIOB, GPIO_PIN_2);
   LCD_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  count = htim2.Instance->CNT;								//VARIÃ?VEL DE TESTE
	  direcao =! (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2));		//DIREÃ‡ÃƒO DO ENCODER
	  distancia = (1.9*(rot* 4.5))/360;							//CÃ?LCULO DA CENTIMETRAGEM
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	/* pi * 0,6cm =  1,9cm (CircunferÃªncia)  |
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	|  										 |
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	|  80 pulsos -> 1 volta -> 360Âº -> 1,9cm |
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	|  										 |
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	|  80 - 360Âº							 |
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	|  1  -  x    ->    x = 4,5Âº 	         |
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	|  										 |
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	|  1,9cm -     360Âº						 |
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	|    x	 - (rot * 4,5Âº)        			 |
																|							            */
	  if (debounce(0,htim2.Instance->CNT))						//HOUVE ALTERAÃ‡ÃƒO DE ESTADO?
	  {
		  rot++;
	  }
	  if(direcao == 1)											//SE ENCODER GIROU PARA A ESQUERDA
	  {
		  HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);				//LIGA PISCA ESQUERDO
		  HAL_TIM_OC_Stop(&htim3, TIM_CHANNEL_2);				//DESLIGA PISCA DIREITO
		  sprintf(dir, "Left ");								//PRINTA A DIREÃ‡ÃƒO NO DISPLAY
	  }
	  if(direcao == 0)											//SE ENCODER GIROU PARA A DIREITA
	  {
		  HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_2);				//LIGA PISCA DIREITO
		  HAL_TIM_OC_Stop(&htim3, TIM_CHANNEL_1);				//DESLIGA PISCA ESQUERDO
		  sprintf(dir, "Right");								//PRINTA A DIREÃ‡ÃƒO NO DISPLAY
	  }

	  //PRINTANDO DEMAIS VALORES NO DISPLAY
	  sprintf(msg, "Direcao:%s", dir);
	  LCD_print(msg, 0, 0);
	  sprintf(msg, "Centimetragem:%0.2f cm", distancia);
	  LCD_print(msg, 0, 1);
	  sprintf(msg, "Re:%0.1f cm    ", re);
      LCD_print(msg, 0, 3);

	  if(re <= 10)												//SE O OBJETO ESTA DENTRO DE 10CM
	  {
		  sprintf(msg, "Muito Perto");
		  LCD_print(msg, 0, 4);
	  }
	  if(re > 10 && re < 30)									//SE O OBJETO ESTA ENTRE 10CM E 30CM
	  {
		  sprintf(msg, "Perto      ");
		  LCD_print(msg, 0, 4);
	  }
	  if(re >= 30)												//SE O OBJETO ESTA MAIS AFASTADO QUE 30CM
	  {
		  sprintf(msg, "Longe      ");
		  LCD_print(msg, 0, 4);
	  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)				//DETECTOU ALGO NA SÃ?IDA ECHO DO SENSOR
{
	borda[tmed] = HAL_TIM_ReadCapturedValue(&htim8, TIM_CHANNEL_2);		//BORDA[0] = PRIMEIRA DETECÃ‡ÃƒO (SUBIDA)
																		//BORDA[1] = SEGUNDA DETECÃ‡ÃƒO  (QUEDA)
	if(tmed >= 1)														//Ã‰ A SEGUNDA ALTERAÃ‡ÃƒO DE ESTADO?
	{
		periodo = (borda[1] - borda[0]);								//PRIMEIRO INSTANTE MENOS SEGUNDO INSTANTE
		re = (periodo / 58)- 0.5;										//DisponÃ­vel em -> https://www.robocore.net/sensor-robo/sensor-de-distancia-ultrassonico-hc-sr04
		tmed = 0;														//RETORNA O VETOR BORDA PARA O INÃ?CIO
	}else{																//PRIMEIRA LEITURA?
		tmed++;															//AVANÃ‡A O "SLOT" DO VETOR BORDA
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
