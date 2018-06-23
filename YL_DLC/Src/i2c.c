/**
  ******************************************************************************
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */

#define EE_PAGE_SIZE 64

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */


HAL_StatusTypeDef  EEPROM_Read(uint16_t address,uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    
     uint16_t i;
     HAL_StatusTypeDef sta;
     for(i=0;i<Size;i++) 
     {
         sta = HAL_I2C_Mem_Read(&hi2c1, 0xA1, address++,I2C_MEMADD_SIZE_16BIT, pData++, 1, Timeout);
         if(sta!=HAL_OK)
             return sta;
          delay_us(4000);
     }
     return sta;    
//    return HAL_I2C_Mem_Read(&hi2c1, 0xA1, address,I2C_MEMADD_SIZE_16BIT, pData, Size, Timeout);
}





void I2C2_WriteBuff16(uint8_t deviceAddr,uint16_t writeAddr,uint8_t *pData,uint8_t size)
{        
//uint8_t temp;
        
if (HAL_I2C_Mem_Write(&hi2c1,0xA0,writeAddr,I2C_MEMADD_SIZE_16BIT,pData,size,0XFFFF)!= HAL_OK)        
{}        
while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

}




/* EEPROM写数据 */
HAL_StatusTypeDef EEPROM_Write(uint16_t writeAddr,uint8_t *pData,uint16_t size,uint32_t Timeout)
{
        uint8_t index;
        uint8_t temp1,temp2,temp3;
        uint16_t addrTemp = writeAddr;

        temp1 = EE_PAGE_SIZE - writeAddr % EE_PAGE_SIZE;
        if(size > temp1)                                //写数据超过当前页能写数据
        {
                temp2 = (size-temp1) / EE_PAGE_SIZE;        //整页数
                temp3 = (size-temp1) % EE_PAGE_SIZE;        //最后页写字节数
        }
        else                                                                                //写数据不跨页
        {
                temp1 = size;                                        //单页
                temp2 = 0;
                temp3 = 0;
        }
        if(temp1)                                                                //写开始页
        {
                I2C2_WriteBuff16(0xa0,addrTemp,pData,temp1);
                pData = pData+temp1;        //数据指针偏移
                addrTemp += temp1;
                HAL_Delay(8);                                        //必须，5ms写周期，否则写出错
        }
        if(temp2)                                                                //写整数页
        {
                for(index = 0;index<temp2;index++)
                {
                        I2C2_WriteBuff16(0xa0,addrTemp,pData,EE_PAGE_SIZE);
                        pData = pData+EE_PAGE_SIZE;
                        addrTemp += EE_PAGE_SIZE;
                        HAL_Delay(8);
                }
        }
        if(temp3)                                                                //写最后剩余数据
        {
                I2C2_WriteBuff16(0xa0,addrTemp,pData,temp3);
                HAL_Delay(8);
        }
        return HAL_OK;
}


/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
