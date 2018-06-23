#ifndef __ADS12XX_H
#define __ADS12XX_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

     
#define ADS1_CLK_H          HAL_GPIO_WritePin(LC1_SCLK_GPIO_Port, LC1_SCLK_Pin,GPIO_PIN_SET)
#define ADS1_CLK_L          HAL_GPIO_WritePin(LC1_SCLK_GPIO_Port, LC1_SCLK_Pin,GPIO_PIN_RESET)
#define ADS1_DATA_IN        HAL_GPIO_ReadPin(LC1_DOUT_GPIO_Port, LC1_DOUT_Pin)
#define ADS1_DATA_READY     HAL_GPIO_ReadPin(LC1_DOUT_GPIO_Port, LC1_DOUT_Pin)

 
#define ADS2_CLK_H          HAL_GPIO_WritePin(LC2_SCLK_GPIO_Port, LC2_SCLK_Pin,GPIO_PIN_SET)
#define ADS2_CLK_L          HAL_GPIO_WritePin(LC2_SCLK_GPIO_Port, LC2_SCLK_Pin,GPIO_PIN_RESET)
#define ADS2_DATA_IN        HAL_GPIO_ReadPin(LC2_DOUT_GPIO_Port, LC2_DOUT_Pin)
#define ADS2_DATA_READY     HAL_GPIO_ReadPin(LC2_DOUT_GPIO_Port, LC2_DOUT_Pin)

#define ADS3_CLK_H          HAL_GPIO_WritePin(LC3_SCLK_GPIO_Port, LC3_SCLK_Pin,GPIO_PIN_SET)
#define ADS3_CLK_L          HAL_GPIO_WritePin(LC3_SCLK_GPIO_Port, LC3_SCLK_Pin,GPIO_PIN_RESET)
#define ADS3_DATA_IN        HAL_GPIO_ReadPin(LC3_DOUT_GPIO_Port, LC3_DOUT_Pin)
#define ADS3_DATA_READY     HAL_GPIO_ReadPin(LC3_DOUT_GPIO_Port, LC3_DOUT_Pin)

#define ADS4_CLK_H          HAL_GPIO_WritePin(LC4_SCLK_GPIO_Port, LC4_SCLK_Pin,GPIO_PIN_SET)
#define ADS4_CLK_L          HAL_GPIO_WritePin(LC4_SCLK_GPIO_Port, LC4_SCLK_Pin,GPIO_PIN_RESET)
#define ADS4_DATA_IN        HAL_GPIO_ReadPin(LC4_DOUT_GPIO_Port, LC4_DOUT_Pin)
#define ADS4_DATA_READY     HAL_GPIO_ReadPin(LC4_DOUT_GPIO_Port, LC4_DOUT_Pin)
   
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */