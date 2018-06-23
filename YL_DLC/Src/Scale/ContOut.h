#ifndef _CONT_OUT_H
#define _CONT_OUT_H

#include "RB_Type.h"
typedef enum
{
    ASCII_QUANTITY_OF_ANIMAL  = 1,    
    ASCII_AVERAGE_WEIGHT_PER_ANIMAL,
    ASCII_AVERAGE_TOTAL_WEIGHT,
}CON1_DATA_TYPE;


//extern void continuousOutput_Process(void);
extern void ContinusOutPutProcess(void);
extern uint8_t buildContinuousOutputWordA(SCALE *pScale);

#endif
