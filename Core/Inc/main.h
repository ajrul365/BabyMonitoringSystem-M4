/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "Display.h"
#include "RAiO.h"
#include "neuralNetwork.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA
#define Buzzer_Pin GPIO_PIN_6
#define Buzzer_GPIO_Port GPIOA
#define RST_Pin GPIO_PIN_8
#define RST_GPIO_Port GPIOA
#define R_RST_Pin GPIO_PIN_9
#define R_RST_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
static const uint8_t RA_ADDR= 0x60;		//DB[5:0] = 0110 00XX
static const uint8_t RA_COMD= 0x00;
static const uint16_t RA_DATA= 0x0000;

static const uint8_t RA_Data_Write 	= 0x60 | 0x02;
static const uint8_t RA_Data_Read 	= 0x60 | 0x03;
static const uint8_t RA_CMD 		= 0x60 | 0x00;
static const uint8_t RA_Status 		= 0x60 | 0x01;

static const uint32_t SLEEP= 0x00000000;
static const uint32_t TEMPER= 0x00041EB0;
static const uint32_t NOIS= 0x0006FB1C;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
