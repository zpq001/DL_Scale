#include "ADS1230.h"

#include "ADS12XX.h"



extern void My_us_Delay(__IO uint32_t Delay);
extern void delay_us(uint32_t Number);


int ReadADS1230Value1()
{
    int ADvalue = 0;
    int i = 0;
    for(i=0;i<24;i++)
    {
        ADS1_CLK_H;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,SET);
        // delay 1us
        delay_us(2);
        ADS1_CLK_L;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,RESET);
        delay_us(2);
        ADvalue <<= 1;
        if(ADS1_DATA_IN == 1)
            ADvalue++;      
    }
    ADvalue&=0x00FFFFFF;
     
    if(ADvalue >0x7FFFFF)
      ADvalue|=0XFF000000;
    ADvalue >>= 5;  //20bit ;22bit
    return ADvalue;
}

//int orgvalue =0;
int ReadADS1230Value2()
{
  
    int ADvalue2 = 0;
    int i = 0;
    for(i=0;i<24;i++)
    {
        ADS2_CLK_H;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,SET);
        // delay 1us
        delay_us(2);
        ADS2_CLK_L;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,RESET);
        delay_us(2);
        ADvalue2 <<= 1;
        if(ADS2_DATA_IN == 1)
            ADvalue2++;      
    }
//    orgvalue = ADvalue2;

    ADvalue2&=0x00FFFFFF;
    if(ADvalue2 >0x7FFFFF)
      ADvalue2|=0XFF000000;    
    ADvalue2 >>= 5;  //20bit ;22bit
    return ADvalue2;
}


int ReadADS1230Value3()
{
    int ADvalue3 = 0;
    int i = 0;
    for(i=0;i<24;i++)
    {
        ADS3_CLK_H;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,SET);
        // delay 1us
        delay_us(2);
        ADS1_CLK_L;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,RESET);
        delay_us(2);
        ADvalue3 <<= 1;
        if(ADS1_DATA_IN == 1)
            ADvalue3++;      
    }
    ADvalue3&=0x000FFFFF;
    if(ADvalue3 >0x7FFFF)
      ADvalue3|=0XFFF00000;    
    ADvalue3 >>= 4;  //20bit ;22bit
    return ADvalue3;
}


int ReadADS1230Value4()
{
    int ADvalue4 = 0;
    int i = 0;
    for(i=0;i<24;i++)
    {
        ADS4_CLK_H;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,SET);
        // delay 1us
        delay_us(2);
        ADS4_CLK_L;
//        HAL_GPIO_WritePin(GPIOA, AD_SCLK_Pin,RESET);
        delay_us(2);
        ADvalue4 <<= 1;
        if(ADS4_DATA_IN == 1)
            ADvalue4++;      
    }
    ADvalue4&=0x000FFFFF;
    if(ADvalue4 >0x7FFFF)
      ADvalue4|=0XFFF00000;    
    ADvalue4 >>= 4;  //20bit ;22bit
    return ADvalue4;
}

int OffsetADS1230()
{
    
    return 0;
}