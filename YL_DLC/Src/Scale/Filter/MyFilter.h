#ifndef __MYFILTER_
#define __MYFILTER_

#include <stdint.h>

#define FILTER_BUF_NUM 80

typedef struct FilterAryy
{
  int32_t valuebuf[FILTER_BUF_NUM+1]; // 滤波缓存
  uint32_t headpt; // 当前写入的位置
  uint32_t tailpt;
  uint32_t sampN;
  int32_t sum;
  int32_t filtervalue;
}strFiltertype;

// 移动平均滤波
int32_t CountsFilter(strFiltertype *PFilter,int32_t adcvalue);
// 中位值平均滤波
int32_t MidAverageFilter(strFiltertype *PFilter,int32_t adcvalue);

#endif