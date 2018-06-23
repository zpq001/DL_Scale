#include "comm.h"

//void accessBRAMParameters(uint32_t address, uint8_t *parameter, uint8_t length, ACCESSTYPE type)
//{
//	uint8_t *bram; 
//	uint8_t *tmPtr; 
////	uint32_t BRAMChecksum = 0; 
//	uint8_t  u8Parameter  = 0; 
//	uint16_t u16Parameter = 0; 
//	uint32_t u32Parameter = 0; 
//
//	bram = (uint8_t *)address; 
//
//    switch (type)
//    {
//        case WRITEOPERATION:
//            switch (length)
//            {
//                case 1:
//                    u8Parameter = *parameter; 
//                    u32Parameter = (uint32_t)u8Parameter; 
//                    break;
//                
//                case 2:
//                    tmPtr = (uint8_t *)&u16Parameter; 
//                    memcpy(tmPtr, parameter, 2); 
//                    u32Parameter = (uint32_t)u16Parameter; 
//                    break;
//                
//                case 4:
//                    tmPtr = (uint8_t *)&u32Parameter; 
//                    memcpy(tmPtr, parameter, 4); 
//                    break;
//            }
//
//            break;
//        
//        case READOPERATION:
//        default:
//            tmPtr = (uint8_t *)&u32Parameter; 
//            memcpy(tmPtr, bram, 4); 
//            switch (length)
//            {
//                case 1:
//                    *parameter = (uint8_t)u32Parameter; 
//                    break;
//                
//                case 2:
//                    tmPtr = (uint8_t *)&u16Parameter; 
//                    u16Parameter = (uint16_t)u32Parameter; 
//                    memcpy(parameter, tmPtr, 2); 
//                    break;
//                
//                case 4:
//                    tmPtr = (uint8_t *)&u32Parameter; 
//                    memcpy(parameter, tmPtr, 4); 
//                    break;
//            }
//            break;
//    }
//}


void displayZeroOperationErrorMessage(void)
{
    
}


void displayTareOperationErrorMessage(void)
{
    
}