/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include <string.h>
#include "gpio.h"
#include "usart.h"
#include "CmdProcess.h"
#include "scale.h"
#include "ADS12xx.h"
#include "ADS1230.h"    
#include "MyFilter.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId WeighProcessHandle;
osThreadId Uart1_ProcessHandle;
osThreadId Uart2_ProcessHandle;

osSemaphoreId uart1BinarySemHandle;

/* USER CODE BEGIN Variables */

osThreadId ADC_ProcessHandle;
int32_t adcvalue1;
int32_t adcvalue2;
int32_t sumvalue;
double dFilerAdcValue;
double sFilerAdcValue;
//strFiltertype FisrtFilter;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void WeighProcessTask(void const * argument);
void Uart1_ProcessTask(void const * argument);
void Uart2_ProcessTask(void const * argument);




void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void ADC_ProcessTask(void const * argument);

extern void FreshRegArry();
extern int  ModbusRTU_Process( int com, unsigned char * rebuf,int receivelenth );//接收解析帧头程序
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of uart1BinarySem */
  osSemaphoreDef(uart1BinarySem);
  uart1BinarySemHandle = osSemaphoreCreate(osSemaphore(uart1BinarySem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of WeighProcess */
  osThreadDef(WeighProcess, WeighProcessTask, osPriorityNormal, 0, 128);
  WeighProcessHandle = osThreadCreate(osThread(WeighProcess), NULL);

  /* definition and creation of Uart1_Process */
  osThreadDef(Uart1_Process, Uart1_ProcessTask, osPriorityIdle, 0, 128);
  Uart1_ProcessHandle = osThreadCreate(osThread(Uart1_Process), NULL);

  /* definition and creation of Uart2_Process */
  osThreadDef(Uart2_Process, Uart2_ProcessTask, osPriorityIdle, 0, 128);
  Uart2_ProcessHandle = osThreadCreate(osThread(Uart2_Process), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
  osThreadDef(ADC_Process, ADC_ProcessTask, osPriorityIdle, 0, 128);
  Uart2_ProcessHandle = osThreadCreate(osThread(ADC_Process), NULL);
  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* WeighProcessTask function */
void WeighProcessTask(void const * argument)
{

  /* USER CODE BEGIN WeighProcessTask */
  double filteredCounts;  
  double stabfilercounts;
  int runtime;
  /* Infinite loop */
  for(;;)
  {
     filteredCounts = dFilerAdcValue;
     stabfilercounts = FilterWeight(&filteredCounts);
     sFilerAdcValue = stabfilercounts;
    SCALE_PostProcess(&g_ScaleData, (long)stabfilercounts);
    runtime++;
    if(runtime==10)
    {
     HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
     runtime = 0;
    }
    
    osDelay(100); //10HZ
  }
  /* USER CODE END WeighProcessTask */
}

/* Uart1_ProcessTask function */
void Uart1_ProcessTask(void const * argument)
{
  /* USER CODE BEGIN Uart1_ProcessTask */
  /* Infinite loop */
  for(;;)
  {
    FreshRegArry();
    if(usart1_rx_flag == 1)  // 接受有数据
    {
         
        ModbusRTU_Process(0, usart1_rx_FIFO,usart1_rx_len);
        usart1_rx_flag = 0;
        memset(usart1_rx_FIFO,0,1024);
    }
    osDelay(10);
  }
  /* USER CODE END Uart1_ProcessTask */
}

/* Uart2_ProcessTask function */
void Uart2_ProcessTask(void const * argument)
{
  /* USER CODE BEGIN Uart2_ProcessTask */
  /* Infinite loop */
  for(;;)
  {
    
       if(usart2_rx_flag == 1)  // 接受有数据
      {
          usart2_rx_flag = 0;  
          SetCmdProcess((char*)usart2_rx_FIFO,SetCmdArry);
          memset(usart2_rx_FIFO,0,1024);
          
      }      
    
    osDelay(20);
  }
  /* USER CODE END Uart2_ProcessTask */
}

/* USER CODE BEGIN Application */
 
/* ADC_ProcessTask function */
void ADC_ProcessTask(void const * argument)
{
  /* USER CODE BEGIN ADC_ProcessTask */
  // read adc
   bool adc1flag1 = false;
   bool adc1flag2 = false;
//   int readtimes = 0;
//   int sumfilter = 0;
//   memset(&FisrtFilter,0,sizeof(FisrtFilter));
//   FisrtFilter.headpt = FILTER_BUF_NUM-1;
  /* Infinite loop */
  for(;;)
  {
    if(ADS1_DATA_READY == 0)
    {
      adcvalue1 = ReadADS1230Value1();
      adc1flag1 = true;
    }
    if(ADS2_DATA_READY == 0)
    {
      adcvalue2 = ReadADS1230Value2();
      adc1flag2 = true;
    }
    
    if(adc1flag1&adc1flag2)
    {
//       readtimes++;
       sumvalue = (int)(adcvalue1 + g_ScaleData.adjutk2*adcvalue2+2000); 
//       sumfilter +=  CountsFilter(&FisrtFilter,sumvalue);
//      if(readtimes == 64)
//      {
//         sumfilter >>=3;
//         readtimes = 0;
//         dFilerAdcValue = sumfilter;
//         sumfilter = 0;
//         
//      }
      
      dFilerAdcValue = execute_filter(sumvalue);
//      dFilerAdcValue =  CountsFilter(strFiltertype *PFilter,int32_t adcvalue)
//      stabfilercounts = dFilerAdcValue;
      
      adc1flag1 = false;
      adc1flag2 = false;
    }
    
    osDelay(10);
  }
  /* USER CODE END ADC_ProcessTask */
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
