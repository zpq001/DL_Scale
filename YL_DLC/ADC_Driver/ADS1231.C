#include "ADS1231.h"
#include "ADS12XX.h"

//#define ADS_CLK_H    HAL_GPIO_WritePin(AD_SCLK_GPIO_Port, AD_SCLK_Pin,GPIO_PIN_SET)
//#define ADS_CLK_L    HAL_GPIO_WritePin(AD_SCLK_GPIO_Port, AD_SCLK_Pin,GPIO_PIN_RESET)
//#define ADS_DATA_IN  HAL_GPIO_ReadPin(AD_DOUT_GPIO_Port, AD_DOUT_Pin)
//
//#define ADS_DATA_READY    HAL_GPIO_ReadPin(AD_DOUT_GPIO_Port, AD_DOUT_Pin)
//
//#define ADS_GAIN_128  
//#define ADS_GAIN_64
//
//#define ADS_SPEED_80    HAL_GPIO_WritePin(AD_SPEED_GPIO_Port, AD_SPEED_Pin ,GPIO_PIN_SET)
//#define ADS_SPEED_10    HAL_GPIO_WritePin(AD_SPEED_GPIO_Port, AD_SPEED_Pin ,GPIO_PIN_RESET)
//
////#define AD_DOWN_Pin GPIO_PIN_6
////#define AD_DOWN_GPIO_Port GPIOA
//
//#define ADS_POWND_ENABLE    HAL_GPIO_WritePin(AD_DOWN_GPIO_Port, AD_DOWN_Pin ,GPIO_PIN_RESET)
//#define ADS_POWND_DISABLE   HAL_GPIO_WritePin(AD_DOWN_GPIO_Port, AD_DOWN_Pin ,GPIO_PIN_SET)

extern void My_us_Delay(__IO uint32_t Delay);
extern void delay_us(uint32_t Number);




void InitADS1231(void)
{
    //128GAIN
    ADS_POWND_DISABLE;
    
    ADS_SPEED_80;
    // SPEED 80HZ
//    ADS_SPEED_10; //10HZ
    
}

int ReadADS1231Value(void)
{
    int ADvalue = 0;
    int i = 0;
    ADS_CLK_L;
    for(i=0;i<24;i++)
    {
        ADS_CLK_H;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,SET);
        // delay 1us
        delay_us(2);
        ADS_CLK_L;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,RESET);
        delay_us(2);
        ADvalue <<= 1;
        if(ADS_DATA_IN == 1)
            ADvalue++;      
    }
    // 第25个CLK强制DRDY 到高电平
    ADS_CLK_H;
    delay_us(2);
    ADS_CLK_L;
    ADvalue >>= 4;  //20bit 
    return ADvalue;
}


int OffsetADS1231()
{
    
    return 0;
}