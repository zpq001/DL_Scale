#ifndef __ADS1231_H
#define __ADS1231_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"




/* USER CODE BEGIN Prototypes */

void InitADS1231(void);
int ReadADS1231Value(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */