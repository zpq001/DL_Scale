
#include "MyFilter.h"
// 移动平均滤波
int32_t CountsFilter(strFiltertype *PFilter,int32_t adcvalue)
{
//  uint32_t i;
  uint32_t sum = 0;
  
  
   
//    PFilter->filtervalue = adcvalue;
    PFilter->valuebuf[PFilter->headpt++] = adcvalue;
    
    if (PFilter->headpt >= FILTER_BUF_NUM)
		PFilter->headpt = 0;   
     PFilter->sum=PFilter->sum - PFilter->valuebuf[PFilter->tailpt]+adcvalue;
     
     sum  = PFilter->sum ;
     sum <<=3;
	 PFilter->filtervalue   = sum /(FILTER_BUF_NUM-1);        
      PFilter->tailpt++;
    if (PFilter->tailpt >= FILTER_BUF_NUM)
		PFilter->tailpt = 0;
    
    return PFilter->filtervalue;

  
}
// 中位值平均滤波
//uint32_t MidAverageFilter(strFiltertype *PFilter,uint32_t adcvalue)
//{
//  uint32_t maxvalue = PFilter->valuebuf[0], minvalue = PFilter->valuebuf[0];
//  uint32_t i;
//  uint32_t sum = 0,average=0;
//  if(PFilter->sampN<=2)
//    return adcvalue;
//  if(PFilter->writep >= PFilter->sampN)
//    PFilter->writep = 0;
//  PFilter->valuebuf[PFilter->writep++]= adcvalue;
//  for(i=0;i<PFilter->sampN;i++)
//  {
//    sum+=PFilter->valuebuf[i];
//    if(PFilter->valuebuf[i]>maxvalue)
//      maxvalue = PFilter->valuebuf[i];
//    if(PFilter->valuebuf[i]<minvalue)
//      minvalue = PFilter->valuebuf[i];
//  }
//  sum-=maxvalue;
//  sum-=minvalue;
//  average=sum/(PFilter->sampN-2);
//  return average;
//  
//}