#ifndef __COMM_H
#define __COMM_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* USER CODE BEGIN Includes */

#define     RB_DECL_FUNC
#define     RB_DECL_TYPE   
#define     RB_DECL_CONST

#define FLT64_INFINITY      INFINITY                //! Infinity
#define FLT64_isfinite(x)   isfinite(x)             //! Test for finite value (not infinite or NaN)
     
     //! Float 32 bit
typedef float RB_DECL_TYPE float32;

//! Float 64 bit
typedef double RB_DECL_TYPE float64;


/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
   
 typedef enum
{
    READOPERATION = 0,
    WRITEOPERATION
}ACCESSTYPE; 



/* USER CODE END Private defines */



/* USER CODE BEGIN Prototypes */

//void accessBRAMParameters(uint32_t address, uint8_t *parameter, uint8_t length, ACCESSTYPE type);
   
   
void displayZeroOperationErrorMessage(void);

void displayTareOperationErrorMessage(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */