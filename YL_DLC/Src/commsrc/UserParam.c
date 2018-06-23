#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "stm32F1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"
#include "i2c.h"

#include "UserParam.h"


// Block storage type
const uint16_t blockStorageType[MAX_BLOCK_NUM] = {
    BLOCK_STORAGE_TYPE_DEFINITIONS
};

// Parameter table, i.e. definition list of all parameters
const USER_PARAM_tTableEntry USER_PARAM_Table[] = {
    USER_PARAM_DEFINITIONS
};

//! Size of parameter table
const int USER_PARAM_TableSize = sizeof(USER_PARAM_Table) / sizeof(USER_PARAM_tTableEntry);

static USER_PARAM_tBlockInfo blockInfo[MAX_BLOCK_NUM];
static uint8_t eeAppMap[MAX_APP_PARAM_SIZE];
static uint8_t eeMfgMap[MAX_MFG_PARAM_SIZE];


//static USER_PARAM_tStatus RestoreBackupBlock(int32_t blockIndex, uint8_t *param);
static uint16_t USER_PARAM_CalcBlockChecksum(int32_t blockIndex, uint8_t *pBlock);

/*---------------------------------------------------------------------*
 * Name         : USER_PARAM_Initialize
 * Prototype in : UserParam.h
 * Description  : Initialize user application parameters
 * Return value : blockError = 0 OK
                  blockError != 0 some blocks' parameters error 
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
int USER_PARAM_Initialize(void)
{
    int i, offsetApp, offsetMfg, size, blockIndex, blockError;
    USER_PARAM_tStatus readStatus;
    
    for (i = 0; i < MAX_BLOCK_NUM; i++)
    {
        blockInfo[i].startIndex = 0;
        blockInfo[i].endIndex = 0;
        blockInfo[i].blockOffset = 0;
        blockInfo[i].blockSize = 0;
    }

    blockIndex = 0;
    offsetApp  = 0;
    offsetMfg  = 0;
    size = 0;
    for (i = 0; i < USER_PARAM_TableSize; i++)
    {   
        if ((USER_PARAM_Table[i].paramId/100) > blockIndex)       // next block start
        {
            blockInfo[blockIndex].endIndex  = i;
            blockInfo[blockIndex].blockSize = size;
            size = 0;
            blockIndex++;
            if (blockIndex < MAX_BLOCK_NUM)
            {
                blockInfo[blockIndex].startIndex = i;
                if (blockStorageType[blockIndex] == STORAGE_APP)
                    blockInfo[blockIndex].blockOffset = offsetApp;
                else if (blockStorageType[blockIndex] == STORAGE_MFG)
                    blockInfo[blockIndex].blockOffset = offsetMfg;
            }
            else
                return USER_PARAM_TOO_MUCH_BLOCK;
        }
        if (blockStorageType[blockIndex] == STORAGE_APP)
            offsetApp += USER_PARAM_Table[i].dataSize;
        else if (blockStorageType[blockIndex] == STORAGE_MFG)
            offsetMfg += USER_PARAM_Table[i].dataSize;
        
        size += USER_PARAM_Table[i].dataSize;
    }
    blockInfo[blockIndex].endIndex  = i;    // the last block info
    blockInfo[blockIndex].blockSize = size; // the last block info
    
    blockError = 0;    
    // for (i = 0; i < MAX_BLOCK_NUM; i++)
    for (i = 0; i < EEPROM_USEDBLOCKS; i++)
    {
        if (blockStorageType[i] == STORAGE_APP)
            readStatus = USER_PARAM_GetBlock(i, &eeAppMap[blockInfo[i].blockOffset]);
        else
            readStatus = USER_PARAM_GetBlock(i, &eeMfgMap[blockInfo[i].blockOffset]);
        
        if (readStatus != USER_PARAM_OK)
        {
            blockError |= 1 << i;
        }
    }

    return blockError;
}

USER_PARAM_tStatus USER_PARAM_Get(USER_PARAM_tIdent ident, uint8_t* param)
{
    int32_t i, offset, blockIndex;
    HAL_StatusTypeDef readStatus;
    USER_PARAM_tStorageType storageType;
    
    // Get the block index that the parameter belongs to
    blockIndex = ident / 100;
    if (blockIndex >= MAX_BLOCK_NUM)
        return USER_PARAM_TOO_MUCH_BLOCK;
    
    offset = blockInfo[blockIndex].blockOffset;
    // Get the parameter offset
    for (i = blockInfo[blockIndex].startIndex; i < blockInfo[blockIndex].endIndex; i++)
    {
        if (USER_PARAM_Table[i].paramId == ident)
            break;
        
        offset += USER_PARAM_Table[i].dataSize;
    }
    
    if (i < blockInfo[blockIndex].endIndex)     // found parameter ID
    {
        storageType = (USER_PARAM_tStorageType)blockStorageType[blockIndex];
        
        if (storageType == STORAGE_APP)       // get single parameter from eeprom immage
            memcpy(param, &eeAppMap[offset], USER_PARAM_Table[i].dataSize);
        else if (storageType == STORAGE_MFG)
        {

           readStatus =  EEPROM_Read(EE_MFG_BASE+offset, param,USER_PARAM_Table[i].dataSize, 4000);
            if (readStatus != HAL_OK)
                return USER_PARAM_MFG_READ_ERROR;
        }
    }
    else
        return USER_PARAM_ID_NOT_FOUND;
        
    return USER_PARAM_OK;
}

/*---------------------------------------------------------------------*
 * Name         : USER_PARAM_Set
 * Prototype in : 
 * Paremeter    : ident:the parameter identifier
 *                param:the new vaule buffer
 * Description  : Reset scale parameters to default value
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
USER_PARAM_tStatus USER_PARAM_Set(USER_PARAM_tIdent ident, uint8_t* param)
{
    int32_t i, j, offset, blockIndex, paramOffset, checksumOffset;
    HAL_StatusTypeDef status1;
    USER_PARAM_tStorageType storageType;
    uint16_t checksum;
    uint8_t *pChecksum;
  
    // Get the block index that the parameter belongs to
    blockIndex = ident / 100;
    if (blockIndex >= MAX_BLOCK_NUM)
        return USER_PARAM_TOO_MUCH_BLOCK;
  
    offset = blockInfo[blockIndex].blockOffset;
    // Get the parameter offset
    for (i = blockInfo[blockIndex].startIndex; i < blockInfo[blockIndex].endIndex; i++)
    {
        if (USER_PARAM_Table[i].paramId == ident)
        {
            paramOffset = offset;
            j = i;
        }
        offset += USER_PARAM_Table[i].dataSize;
    }

    checksumOffset = blockInfo[blockIndex].blockOffset + blockInfo[blockIndex].blockSize - 2;
    storageType = (USER_PARAM_tStorageType)blockStorageType[blockIndex];
    if (j < blockInfo[blockIndex].endIndex)
    {
        if (storageType == STORAGE_APP)
        {
          /*to check if the parameter changed*/
          for (i = 0; i < USER_PARAM_Table[j].dataSize; i++)
          {
            if (param[i] != eeAppMap[paramOffset+i])
                break;
          }
          if (i == USER_PARAM_Table[j].dataSize)
          {
            checksum = USER_PARAM_CalcBlockChecksum(blockIndex, &eeAppMap[blockInfo[blockIndex].blockOffset]);
            pChecksum = (uint8_t *)&checksum;
            /* There is no change for parameter, no need to write*/
            if ((pChecksum[0] == eeAppMap[checksumOffset]) && (pChecksum[1] == eeAppMap[checksumOffset+1]))
                return USER_PARAM_OK;       
          }
          

          status1 = EEPROM_Write(EE_APP_BASE+paramOffset, param,USER_PARAM_Table[j].dataSize, 4000);
             if (status1 == HAL_OK)
          {
//             HAL_Delay(20); 
              EEPROM_Read(EE_APP_BASE+paramOffset, param, USER_PARAM_Table[j].dataSize, 4000);
              memcpy(&eeAppMap[paramOffset], param, USER_PARAM_Table[j].dataSize);
              /*Recalculate the block checksum*/
              checksum = USER_PARAM_CalcBlockChecksum(blockIndex, &eeAppMap[blockInfo[blockIndex].blockOffset]);
             
             status1 = EEPROM_Write(EE_APP_BASE+checksumOffset, (uint8_t *)&checksum, 2, 4000);
//             HAL_Delay(20); 
          }
   
          
          if (status1 != HAL_OK)
              return USER_PARAM_APP_WRITE_ERROR;
        }
        else if (storageType == STORAGE_MFG)
        {
            for (i = 0; i < USER_PARAM_Table[j].dataSize; i++)
            {
                if (param[i] != eeMfgMap[paramOffset+i])
                    break;
            }
            if (i == USER_PARAM_Table[j].dataSize)
            {
                checksum = USER_PARAM_CalcBlockChecksum(blockIndex, &eeMfgMap[blockInfo[blockIndex].blockOffset]);
                pChecksum = (uint8_t *)&checksum;
                /*There is no change for parameter, no need to write*/
                if ((pChecksum[0] == eeMfgMap[checksumOffset]) && (pChecksum[1] == eeMfgMap[checksumOffset+1]))
                    return USER_PARAM_OK;       
            }

            status1 = EEPROM_Write(EE_MFG_BASE+paramOffset, param, USER_PARAM_Table[j].dataSize, 0xFFFF);
//            HAL_Delay(20); 
            if (status1 == HAL_OK)
            {
                memcpy(&eeMfgMap[paramOffset], param, USER_PARAM_Table[j].dataSize);
                /*Recalculate the block checksum*/
                checksum = USER_PARAM_CalcBlockChecksum(blockIndex, &eeMfgMap[blockInfo[blockIndex].blockOffset]);
                status1 = EEPROM_Write(EE_MFG_BASE+checksumOffset, (uint8_t *)&checksum,2, 0xFFFF);
//                HAL_Delay(20); 
            }
                  
            if (status1 != HAL_OK)
                return USER_PARAM_MFG_WRITE_ERROR;           
        }
    }
    else
        return USER_PARAM_ID_NOT_FOUND;
      
    return USER_PARAM_OK;
}

USER_PARAM_tStatus USER_PARAM_GetBlock(int32_t blockIndex, uint8_t *param)
{
    uint16_t checksumCalc = 0;
    int32_t  checksumOffset;
    // uint16_t *pChecksumGet;
    uint8_t *pChecksum;
    USER_PARAM_tStorageType storageType;
    HAL_StatusTypeDef readStatus;
    
    /*Get the start address of block checksum stored in EEPROM*/
    checksumOffset = blockInfo[blockIndex].blockSize-2;
    
    storageType = (USER_PARAM_tStorageType)blockStorageType[blockIndex];
    if (storageType == STORAGE_APP)
    {
        /*Read all block parameters back,including the 2 checksum bytes*/

        readStatus = EEPROM_Read( EE_APP_BASE+blockInfo[blockIndex].blockOffset, param, blockInfo[blockIndex].blockSize, 4000);
        if (readStatus == HAL_OK)
        {
            /* calculate block checksum*/
            checksumCalc = USER_PARAM_CalcBlockChecksum(blockIndex, param);
            pChecksum = (uint8_t *)&checksumCalc;
            if ((pChecksum[0] != (uint8_t)param[checksumOffset]) || (pChecksum[1] != (uint8_t)param[checksumOffset+1]))
                return USER_PARAM_APP_BLOCK_CHECKSUM_ERROR; 
        }
        else
            return USER_PARAM_APP_READ_ERROR;            
      
    }
    else if (storageType == STORAGE_MFG)
    {
//        readStatus = RB_EEPROM_Read(RB_CONFIG_I2C_DEV_EEPROM_MAIN, EE_MFG_BASE+blockInfo[blockIndex].blockOffset, param, blockInfo[blockIndex].blockSize);
//        readStatus = HAL_I2C_Master_Receive(&hi2c1, EE_MFG_BASE+blockInfo[blockIndex].blockOffset, param, blockInfo[blockIndex].blockSize, 200);
        readStatus = EEPROM_Read(EE_MFG_BASE+blockInfo[blockIndex].blockOffset, param, blockInfo[blockIndex].blockSize, 4000);
        if (readStatus == HAL_OK)
        {
            /* calculate block checksum*/
            checksumCalc = USER_PARAM_CalcBlockChecksum(blockIndex, param);
            pChecksum = (uint8_t *)&checksumCalc;
            if ((pChecksum[0] != (uint8_t)param[checksumOffset]) || (pChecksum[1] != (uint8_t)param[checksumOffset+1]))
                return USER_PARAM_MFG_BLOCK_CHECKSUM_ERROR;
        }
        else
            return USER_PARAM_MFG_READ_ERROR;          
    }
  
    return USER_PARAM_OK; 
}

USER_PARAM_tStatus USER_PARAM_SetBlock(int32_t blockIndex, uint8_t* param)
{
    uint16_t checksumCalc = 0;
    uint16_t *pChecksumGet;
    USER_PARAM_tStorageType storageType;
    HAL_StatusTypeDef status1;

    // Recalculate the block checksum
    checksumCalc = USER_PARAM_CalcBlockChecksum(blockIndex, param);
    pChecksumGet = (uint16_t *)&param[blockInfo[blockIndex].blockSize-2];
    *pChecksumGet = checksumCalc;
            
    storageType = (USER_PARAM_tStorageType)blockStorageType[blockIndex];
    if (storageType == STORAGE_APP)
    {
        status1 = EEPROM_Write(EE_APP_BASE+blockInfo[blockIndex].blockOffset, param, blockInfo[blockIndex].blockSize, 0XFFFF);
        if (status1 == HAL_OK)
            memcpy(&eeAppMap[blockInfo[blockIndex].blockOffset], param, blockInfo[blockIndex].blockSize);

        
        if (status1 != HAL_OK)
            return USER_PARAM_APP_WRITE_ERROR;

    }
    else if (storageType == STORAGE_MFG)
    {      
          status1 = EEPROM_Write(EE_MFG_BASE+blockInfo[blockIndex].blockOffset, param, blockInfo[blockIndex].blockSize, 0XFFFF);

        if (status1 != HAL_OK)
            return USER_PARAM_MFG_WRITE_ERROR;            
    }
    
    return USER_PARAM_OK;
}




static uint16_t USER_PARAM_CalcBlockChecksum(int32_t blockIndex, uint8_t *pBlock)
{
    int32_t i;
    uint16_t checksum = 0;
    
    for (i = 0; i < (blockInfo[blockIndex].blockSize-2); i++)
        checksum += *pBlock++;
        
    return checksum;
}
