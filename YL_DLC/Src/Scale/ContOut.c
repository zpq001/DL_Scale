//==================================================================================================
//                                          Rainbow
//==================================================================================================
//
//! \file			IND245/scale/ContOut.c
//! \ingroup	IND245_scale
//! \brief		process Continuous Output
//!
//! (c) Copyright 2004-2006 Mettler-Toledo Laboratory & Weighing Technologies. All Rights Reserved.
//! \author Liang XiuWen
//
// $Date: 		2010/04/09
// $State: 
// $Revision: 0.1
//
//==================================================================================================
//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================
#include <string.h>
#include "RB_ASCII.h"
#include "RB_IncludeAll.h"
#include "ScaleConfig.h"
#include "menu.h"
#include "Scale.h"
#include "ApplMain.h"
#include "ApplConfig.h"
#include "ContOut.h"
#include "UserParam.h"
#include "RB_UART.h"
#include "DiscreteIO.h"
#include "GUIDisplayProcess.h"
#include "dynamic.h"
#include "Ethernet.h"
#include "CheckTime.h"


extern char BTconnectflag;        //Bluetooth connection flag
extern unsigned char WgtDataOK;  // weight data ok flag
extern bool conoutfalg;

//void continuousOutput_Process(void);
void ContinusOutPutProcess(void);



static void FormatOutputWeightString(char *pSrcString, char *pDestString, int maxLen, bool sign,bool decimal);
static void FormatOutputWeightString8142(char *pSrcString, char *pDestString, int maxLen, bool sign,bool decimal);
static void FormatOutputWeightStringKingbird(char *pSrcString, char *pDestString, int maxLen, bool16 sign,bool16 decimal);

static void buildAnimalTotalAndTare(SCALE *pScale, char *pOutString);
static void buildNormalNetAndTare(SCALE *pScale, char *pString);
static void buildNormalNetAndTare8142(SCALE *pScale, char *pString);
static void buildNormalNetAndTareKingbird(SCALE *pScale, char *pString);

static void buildMultiContinous1OutputString(SCALE *pScale, char *pOutString, CON1_DATA_TYPE type); 
static void buildMultiContinous2OutputString(SCALE *pScale, char *pOutString, CON1_DATA_TYPE type); 

static int caculatevalidlenth(void);

static void CONT_BuildContinousOutputString(SCALE *this, char *pOutString);
static void CONT8142_BuildContinousOutputString(SCALE *this, char *pOutString);
static void CONTKingbird_BuildContinousOutputString(SCALE *pScale, char *pOutString);
static void CONT_BuildContinuousExtendedOutputString(SCALE *pScale, char *pOutString, char addr);


static void COM1ContinusOutPutProcess(void);
static void COM2ContinusOutPutProcess(void);
static void EthernetCountinusOutPutProcess(void);
static void EthernetPrintClintCoutProcess(void);
static void USBContinusOutPutProcess(void);
static void BluetoothContinusOutPutProcess(void);


//--------------------------------------------------------------------------------------------
// Continus Out Put Process
//--------------------------------------------------------------------------------------------
void ContinusOutPutProcess(void)
{
  //condition check, then processs g_setupMode||(WgtDataOK != 0)||loadtypematch == false || 
  if(conoutfalg == false||g_setupMode)
    return ;
  COM1ContinusOutPutProcess();
  COM2ContinusOutPutProcess();
  USBContinusOutPutProcess();
  EthernetCountinusOutPutProcess();  
  EthernetPrintClintCoutProcess();
  BluetoothContinusOutPutProcess();
  
  g_bPrintRequest = 0;    // reset the print flag
}


/**---------------------------------------------------------------------
 * Name         : continuousOutput_Process
 * Description  : Continuous output process
 * Prototype in : 
 * \param       : none
 * \return      : none
 *---------------------------------------------------------------------*/

/**---------------------------------------------------------------------
 * Name         : buildAnimalTotalAndTare
 * Description  : Build MT standard continuous output string, total size is 18 bytes, including checksum bytes
 * Prototype in : 
 * \param       : pScale  stand scale struct;    pOutString  the out str
 * \return      : none
 *---------------------------------------------------------------------*/
void buildAnimalTotalAndTare(SCALE *pScale, char *pOutString)
{
    DYNAMICAPPLET_STATUS currentStatus;
    char totalString[12] = {0};
    char tareString[12]  = {0}; 
    char string[10] = {0}; 
    int32_t dp = SCALE_GetDp(scale.currInc); 
    uint8_t i = 0; 
    char animaltring[15] = {0}; 
    
    if (dp < 1)
        dp = 0; 
    if (m_DynamicApplet_Status == DYNAMICAPPLET_ENTERDATA || m_DynamicApplet_Status == DYNAMICAPPLET_ENTERID)
        currentStatus = m_PreviousDynamicApplet_Status;
    else
        currentStatus = m_DynamicApplet_Status;

    switch (currentStatus)
    {   
        case DYNAMICAPPLET_NONE:
        case DYNAMICAPPLET_ENTERANIMALNUMBER:
        case DYNAMICAPPLET_START:
        case DYNAMICAPPLET_WAITTOEND:
        {
            buildNormalNetAndTare(pScale, pOutString);
            break;
        }
        
        case DYNAMICAPPLET_WORKING:
        {
            buildNormalNetAndTare(pScale, (char *)animaltring); 
            for (i = 0; i < 6; i++)
            {
                if (animaltring[i] != ' ')
                    animaltring[i] = '*'; 
            }
            for (i = 0; i < 6; i++)
            {
                if (animaltring[i + 6] != ' ')
                    animaltring[i + 6] = '0'; 
            }
            RB_STRING_strncpymax(pOutString, (char *)animaltring, 12 + 1);
//            string[0] = 0x2a;
//            string[1] = '\0';
//            RB_STRING_AlignRight(string, 6);
//            RB_STRING_strncpymax(pOutString, string, 6 + 1);
//            pOutString += 6;
//            string[0] = '0';
//            string[1] = '\0';
//            RB_STRING_AlignRight(string, 6);
//            RB_STRING_strncpymax(pOutString, string, 6 + 1);
            break;
        }
        
        case DYNAMICAPPLET_SHOWAVEWEIGHT:
        {
            RB_FORMAT_Double(string, dp, (float64)(g_DynamicWeighing.totalWeight), 10);
            FormatOutputWeightString(string, totalString, 6, false, false);
            FormatOutputWeightString(pScale->tareString, tareString, 6, false,false);
            RB_STRING_strncpymax(pOutString, totalString, 7);
            pOutString += 6;
            RB_STRING_strncpymax(pOutString, tareString, 7);
            break;
        }
        
        default:
            break;
    }
}
/**---------------------------------------------------------------------
* Name         :  buildStatusWord
* Description  :  build WORDA WORDB WORDC
* Prototype in : 
* \param       : pScale  stand scale struct;    pOutString  the out str
* \return      : none
*---------------------------------------------------------------------*/
static void buildStatusWord(SCALE *pScale, char* pString)
{
    uint8_t      netMode;
    uint8_t      wordA, wordB, wordC;
    bool       bUnderZero, bMotion, bPowerUpZero;
    UNIT_tSymbol unit;
    
    wordA       =  buildContinuousOutputWordA(pScale); 
    *pString++  =  wordA;

    // Build status word B
    wordB = 0x20;       // B.5 always 1
    // B.0 Gross = 0, Net = 1
    netMode = TARE_GetTareMode(&(pScale->tare));
    if (netMode == 'N')
        wordB |= 0x01;
    // B.1 Sign: Positive = 0, Negative = 1
    if (((netMode == 'G') && (pScale->roundedGrossWeight < 0)) || ((netMode == 'N') && (pScale->roundedNetWeight < 0)))
        wordB |= 0x02;
    // B.2 Out of range = 1 (Over or under)
    bUnderZero = ZERO_GetUnderZero(&(pScale->zero));
    if (pScale->bOverCapacity || bUnderZero)
        wordB |= 0x04;
    // B.3 Motion = 1
    bMotion = MOTION_GetMotion(&(pScale->motion));
    if (bMotion)
        wordB |= 0x08;
    // B.4 Unit: lb = 0, kg = 1 (see SWC, 0-2)
    unit = UNIT_GetUnit(&(pScale->unit), pScale->unit.currUnitType);
    if (unit == UNIT_kg)
        wordB |= 0x10;
    // B.6 In Power up = 1 (Zero Not Captured)
    bPowerUpZero = ZERO_GetPowerUpZeroCaptured(&(pScale->zero));
    if (!bPowerUpZero)
        wordB |= 0x40;
    *pString++ = wordB;
    
    // Build status word C
    wordC = 0x20;      // C.5 always 1
    // C.0 C.1 C.2 current unit besides kg and lb (see SWB.4)
    switch (unit)
    {
        case UNIT_g:    
            wordC |= 0x01; 
            break;
            
        case UNIT_t:    
            wordC |= 0x02;
            break;
            
        case UNIT_kg:
        case UNIT_lb:   
            wordC &= ~0x03; 
            break;
        
        case UNIT_oz:
            wordC |= 0x03;
            break; 
                
        case UNIT_ton:  
            wordC |= 0x06; 
            break;
        case UNIT_newton:
			wordC |= 0x07;
			break;
        default: 
            break;
    }
    // Print request = 1
    if (g_bPrintRequest)
        wordC |= 0x08;
    // Expand X10 = 1
    if (pScale->bExpandDisplay)
        wordC |= 0x10;
    *pString++ = wordC;
}
/**---------------------------------------------------------------------
* Name         :  buildChecksum
* Description  :  work the checksum
* Prototype in : 
* \param       :  pOutString  the out str
* \return      :  none
*---------------------------------------------------------------------*/
static void buildChecksum(char *pOutString)
{
    uint8_t i, checksum;
    char *  pString;
    checksum = 0;
    pString  = pOutString;
    for (i = 0; i < 17; i++)
    {
        checksum -= *pString;
        pString++;
    }
    *pString++ = (checksum & 0x7F); 
    // add terminating zero
    *pString = '\0';
}

/**---------------------------------------------------------------------
* Name         :  buildNormalNetAndTare
* Description  :  build the field of Net and tare in normal mode
* Prototype in : 
* \param       :  pScale  stand scale struct;    pOutString  the out str
* \return      :  none
*---------------------------------------------------------------------*/
static void buildNormalNetAndTare(SCALE *pScale, char *pString)
{
    char netString[12] = {0}, tareString[12] = {0};
    FormatOutputWeightString(pScale->netString,  netString,  6, false,false);
    FormatOutputWeightString(pScale->tareString, tareString, 6, false,false);
    RB_STRING_strncpymax(pString, netString, 7);
    pString += 6;
    RB_STRING_strncpymax(pString, tareString, 7);
}

static void buildNormalNetAndTare8142(SCALE *pScale, char *pString)
{
    char netString[12] = {0}, tareString[12] = {0};
    FormatOutputWeightString8142(pScale->netString,  netString,  6, false,false);
    FormatOutputWeightString8142(pScale->tareString, tareString, 6, false,false);
    RB_STRING_strncpymax(pString, netString, 7);
    pString += 6;
    RB_STRING_strncpymax(pString, tareString, 7);
}

static void buildNormalNetAndTareKingbird(SCALE *pScale, char *pString)
{
    char netString[12] = {0}, tareString[12] = {0};
    FormatOutputWeightStringKingbird(pScale->netString,  netString,  6, false,false);
    FormatOutputWeightStringKingbird(pScale->tareString, tareString, 6, false,false);
    RB_STRING_strncpymax(pString, netString, 7);
    pString += 6;
    RB_STRING_strncpymax(pString, tareString, 7);
}
/**---------------------------------------------------------------------
 * Name         : CONT_BuildContinousOutputString
 * Description  :  Build MT standard continuous output string, total size is 18 bytes, including checksum bytes
 * Prototype in : 
 * \param         : pScale  stand scale struct;    pOutString  the out str
 * \return      : none
 *---------------------------------------------------------------------*/
static void CONT_BuildContinousOutputString(SCALE *pScale, char *pOutString)
{
    char *pString; 
    
    pString = pOutString; 
    *pString++ = RB_ASCII_STX; 
    buildStatusWord(pScale, pString); 
    pString += 3; 
    // Format output weight string
    switch (selectedApplet)
    {
        case APPLCONFIG_APPL_DYNAMICWEIGHING:
        {
            buildAnimalTotalAndTare(pScale, pString);
            break;
        }
        
        default:
        {
            buildNormalNetAndTare(pScale, pString);
            break;
        }
    }
    pString += 12;
    // add CR
    *pString++ = RB_ASCII_CR;
    buildChecksum(pOutString);
    
}

static void CONT8142_BuildContinousOutputString(SCALE *pScale, char *pOutString)
{
    char *pString; 
    
    pString = pOutString; 
    *pString++ = RB_ASCII_STX; 
    buildStatusWord(pScale, pString); 
    pString += 3; 
    // Format output weight string
    switch (selectedApplet)
    {
        case APPLCONFIG_APPL_DYNAMICWEIGHING:
        {
            buildAnimalTotalAndTare(pScale, pString);
            break;
        }
        
        default:
        {
            buildNormalNetAndTare8142(pScale, pString);
            break;
        }
    }
    pString += 12;
    // add CR
    *pString++ = RB_ASCII_CR;
    buildChecksum(pOutString);
    
}
//kingbird continuous output,补有效零
static void CONTKingbird_BuildContinousOutputString(SCALE *pScale, char *pOutString)
{
    char *pString; 
    
    pString = pOutString; 
    *pString++ = RB_ASCII_STX; 
    buildStatusWord(pScale, pString); 
    pString += 3; 
    // Format output weight string
    switch (selectedApplet)
    {
        case APPLCONFIG_APPL_DYNAMICWEIGHING:
        {
            buildAnimalTotalAndTare(pScale, pString);
            break;
        }
        
        default:
        {
            buildNormalNetAndTareKingbird(pScale, pString);
            break;
        }
    }
    pString += 12;
    // add CR
    *pString++ = RB_ASCII_CR;
    buildChecksum(pOutString);
    
}

/**---------------------------------------------------------------------
 * Name         : CONT_BuildContinuousExtendedOutputString
 * Description  : process MT extend continous output format
 * Prototype in : 
 * \param         : pScale   pOutString     addr:end flag
 * \return      : none
 *---------------------------------------------------------------------*/

static void CONT_BuildContinuousExtendedOutputString(SCALE *pScale, char *pOutString, char addr)
{
    char netString[10], tareString[9];
    unsigned char i, checksum;
    unsigned char byte1, byte2, byte3, byte4;
    UNIT_tSymbol unit;
    bool bCOZ, bMotion, bUnderZero, bPowerUpZero;
    unsigned char netMode;
    char *pString;
    unsigned char *pTareSource;
    
    pString = pOutString;
    *pString++ = RB_ASCII_SOH;
    *pString++ = addr;
    
    // build status byte1
    byte1 = 0x20;
    
    unit = UNIT_GetUnit(&(pScale->unit), pScale->unit.currUnitType);
    switch (unit)
    {
        case UNIT_lb:
            byte1 |= 0x01;
            break;
            
        case UNIT_kg:
            byte1 |= 0x02;
            break;
            
        case UNIT_g:
            byte1 |= 0x03;
            break;
            
        case UNIT_t:
            byte1 |= 0x04;
            break;
            
        case UNIT_ton:
            byte1 |= 0x05;
            break;
            
        case UNIT_oz:
            byte1 |= 0x08;
            break;
        case UNIT_newton:
			byte1 |= 0x09;
			break;
   //     case UNIT_ozt:
    //        byte1 |= 0x06;
     //       break;
     //   case UNIT_dwt:
     //       byte1 |= 0x07;
     //       break;
     //   case UNIT_oz:
     //       byte1 |= 0x08;
     //       break;
//        case UNIT_cus:
//            byte1 |= 0x09;
//            break;
        default:
            break;
    }
    
    bCOZ = ZERO_GetCenterOfZero(&(pScale->zero));
    if (bCOZ == 1)
        byte1 |= 0x10;
    
    bMotion = MOTION_GetMotion(&(pScale->motion));
    if (bMotion)
        byte1 |= 0x40;

    *pString++ = byte1;
    
    // build status byte2
    byte2 = 0x20;

    netMode = TARE_GetTareMode(&(pScale->tare));
    if (netMode == 'N')
    {
        byte2 |= 0x01;
    
        // to do, get tare source
        pTareSource = TARE_GetTareSource(&(pScale->tare));
        switch (*pTareSource)
        {
            case PUSHBUTTON_TARE:
            case AUTO_TARE:
                byte2 |= 0x02;
                break;
                
            case KEYBOARD_TARE:
                byte2 |= 0x04;
                break;
                
            case TARE_MEMORY:
                byte2 |= 0x06;
                break;
                
            default:
                break;
        }
    }
    
    // weight range
    if (pScale->numberRanges == TWO_RANGES) //|| pScale->numberRanges == TWO_INTERVALS
    {
        if (pScale->currentRange == 1)
            byte2 |= 0x10; 
        else
            byte2 |= 0x08; 
    }
    
    
    // Expand X10 = 1
    if (pScale->bExpandDisplay)
        byte2 |= 0x40;

    *pString++ = byte2;

    // build status byte3
    byte3 = 0x20;
    
    if (g_scaleProcessState != 1)
        byte3 |= 0x01;
        
    bUnderZero = ZERO_GetUnderZero(&(pScale->zero));
    if (bUnderZero)
        byte3 |= 0x02;

    if (pScale->bOverCapacity)
        byte3 |= 0x04;
    
    bPowerUpZero = ZERO_GetPowerUpZeroCaptured(&(pScale->zero));
    if (!bPowerUpZero)
        byte3 |= 0x08;
    
    if (g_bPrintRequest)
        byte3 |= 0x10;

    // to do, function of bit6(Below MinWeigh threshold) is not used

    *pString++ = byte3;
    
    // build status byte4, to do, application bits is not used here
    byte4 = 0x20;

    *pString++ = byte4;
    
    // build display weight
    FormatOutputWeightString(pScale->netString, netString, 9, true,true);
    FormatOutputWeightString(pScale->tareString, tareString, 8, true,true);
    RB_STRING_strncpymax(pString, netString, 10);
    pString += 9;
    RB_STRING_strncpymax(pString, tareString, 9);
    pString += 8;

    // add CR
    *pString++ = RB_ASCII_CR;
    
    // add checksum byte
    checksum = 0;
    pString = pOutString;
    for (i = 0; i < 24; i++)
    {
        checksum -= *pString;
        pString++;
    }
    *pString++ = (checksum & 0x7F); 
    // add terminating zero
    *pString = '\0';
}

/**---------------------------------------------------------------------
 * Name         : buildContinuousOutputWordA
 * Description  : build continuous output status word A
 * Prototype in : 
 * \param       : pScale---scale struct
 * \return      : status word
 *---------------------------------------------------------------------*/
uint8_t buildContinuousOutputWordA(SCALE *pScale)
{
    uint8_t word_A = 0x20;
    int32_t incrIndex; 
    float tmpIncr;

    if (pScale->bExpandDisplay)
    {
        tmpIncr = pScale->currInc * 10;
        incrIndex = SCALE_GetIncrIndex(tmpIncr);
        word_A |= ((incrIndex % 3 + 1) & 0x03) << 3;
        incrIndex = SCALE_GetIncrIndex(pScale->currInc);
        if (incrIndex >= 0)
        {
            word_A |= ((CONFIG_MAX_DP + 2) - incrIndex / 3) & 0x07; 
        }
        else
        {
            
            if (pScale->roundedNetWeight < 9.999999)
            {
                //output format:x.xxxxx
                word_A |= 0x07;
            }
            else
            {
                //output format:xx.xxxx,the last digit after decimal lost
                word_A |= 0x06;
            }
        }
    }
    else
    {
        incrIndex = SCALE_GetIncrIndex(pScale->currInc);
        if (incrIndex >= 0)
        {
            word_A |= (((incrIndex % 3 + 1) & 0x03) << 3)                    // add build code 2 bits
                     | (((CONFIG_MAX_DP + 2) - incrIndex / 3) & 0x07);      // add dp location 3 bits
        }
    }
    return word_A;
}

/**---------------------------------------------------------------------
 * Name         : FormatOutputWeightString
 * Description  : // Remove decimal point and negtive sign (if disable) in weight string, then align right
 * Prototype in : 
 * \param         : pSrcString   pDestString  maxLen  sign  decimal
 * \return      : none
 *---------------------------------------------------------------------*/

static void FormatOutputWeightString(char *pSrcString, char *pDestString, int maxLen, bool sign,bool decimal)
{
    int32_t i;
    char ch;
    char *pString;
    
    pString = pDestString;
	for (i = 0; i < 12; i++)
	{
	    ch = *pSrcString;
		if (ch >= '0' && ch <= '9')
		{
			*pString = ch;
			pSrcString++;
			pString++;
		}
		else if (ch == '.')
		{if (decimal==false)//Feb_09_09  lxw to add the decimal in contimuous_extanded output 
			{pSrcString++;
		       }
		else
			{
			*pString = ch;
			pSrcString++;
			pString++;
			}
		}
		else if (ch == '-')
		{
		    if (sign)
		    {
		        *pString = ch;
		        pString++;
		    }
		    pSrcString++;
		}
		else if (ch == '\0')
			break;
		else
			pSrcString++;
	}
    *pString = '\0';
    
    RB_STRING_AlignRight(pDestString, maxLen);
}

static void FormatOutputWeightString8142(char *pSrcString, char *pDestString, int maxLen, bool sign,bool decimal)
{
    int32_t i;
    char ch;
    char *pString;
    
    pString = pDestString;
	for (i = 0; i < 12; i++)
	{
	    ch = *pSrcString;
		if (ch >= '0' && ch <= '9')
		{
			*pString = ch;
			pSrcString++;
			pString++;
		}
		else if (ch == '.')
		{
            if (decimal==false)//Feb_09_09  lxw to add the decimal in contimuous_extanded output 
            {
                pSrcString++;
            }
             else
            {
              *pString = ch;
              pSrcString++;
              pString++;
            }
		}
		else if (ch == '-')
		{
		    if (sign)
		    {
		        *pString = ch;
		        pString++;
		    }
		    pSrcString++;
		}
		else if (ch == '\0')
			break;
		else
			pSrcString++;
	}
    *pString = '\0';
    
    RB_STRING_AlignRight(pDestString, maxLen);
	
		pString = pDestString;
		while((*pString) == ' ')
		{
			*pString = '0'; 
			pString++; 
		}

}
//计算有效长度，
static int caculatevalidlenth(void)
{
  int  dp;
  char tmp[12]={0};  
  unsigned char len;
  dp = SCALE_GetDp(scale.currInc);
    if (dp < 1)
        dp = 0;  //无小数    
    sprintf(tmp,"%d",(unsigned int )scale.scaleCapacity);
    len = strlen(tmp) + dp;
    return len;    
  
}

static void FormatOutputWeightStringKingbird(char *pSrcString, char *pDestString, int maxLen, bool16 sign,bool16 decimal)
{
    int32_t i;
    char ch;
    char temp[7]={0};
    char *pString;
    unsigned int validlen = 0;
    unsigned int templen = 0;
    pString = pDestString;
	for (i = 0; i < 12; i++)
	{
	    ch = *pSrcString;
		if (ch >= '0' && ch <= '9')
		{
			*pString = ch;
			pSrcString++;
			pString++;
		}
		else if (ch == '.')
		{
            if (decimal==false)//Feb_09_09  lxw to add the decimal in contimuous_extanded output 
            {
                pSrcString++;
            }
             else
            {
              *pString = ch;
              pSrcString++;
              pString++;
            }
		}
		else if (ch == '-')
		{
		    if (sign)
		    {
		        *pString = ch;
		        pString++;
		    }
		    pSrcString++;
		}
		else if (ch == '\0')
			break;
		else
			pSrcString++;
	}
    *pString = '\0';
    validlen = caculatevalidlenth();
    templen = strlen(pDestString);
    if((validlen > templen)&&((validlen - templen)<6))
    {
      for(i=0; i<(validlen - templen);i++)
        temp[i] = '0';
    }
    strcat(temp,pDestString);
    RB_STRING_strncpymax(pDestString,temp,7);
    RB_STRING_AlignRight(pDestString, maxLen);
//	
//		pString = pDestString;
//		while((*pString) == ' ')
//		{
//			*pString = '0'; 
//			pString++; 
//		}

}

/**---------------------------------------------------------------------
 * Name         : buildMultiContinous1OutputString
 * Description  : Build MT muti continuous 1 output string, 
 *                total size is 18 bytes, including checksum bytes
 * Prototype in : 
 * \param       : pScale  stand scale struct;    pOutString  the out str
 * \return      : none
 *---------------------------------------------------------------------*/
static void buildMultiContinous1OutputString(SCALE *pScale, char *pOutString, CON1_DATA_TYPE type)
{
    char string[10] = {0}; 
    char fieldValueString[10] = {0}; 
    char tareString[10] = {0}; 
    char *pString;
    int32_t dp = SCALE_GetDp(scale.currInc); 
    
    if (dp < 1)
        dp = 0; 
    pString = pOutString;
    *pString++ = (char)type;
    buildStatusWord(pScale, pString);
    pString += 3; 
    
    switch (type)
    {
        case ASCII_QUANTITY_OF_ANIMAL:
        {
            //to number of animal field, the increment should be x1 and unit should be custom unit
            pString -= 3; 
            *pString = 0x2A; 
            pString += 2; 
            *pString |= 0x07; 
            pString += 1; 
            RB_FORMAT_ULong(string, g_DynamicWeighing.animalNumber, 6); 
            RB_STRING_AlignRight(string, 6); 
            
            RB_STRING_strncpymax(pString, string, 7); 
            break;
        }
        
        case ASCII_AVERAGE_WEIGHT_PER_ANIMAL:
        {
            if (m_DynamicApplet_Status == DYNAMICAPPLET_SHOWAVEWEIGHT || m_DynamicApplet_Status == DYNAMICAPPLET_WAITTOEND)
            {
                RB_FORMAT_Double(string, dp, (float64)(g_DynamicWeighing.averageWeight), 10);
                FormatOutputWeightString(string, fieldValueString, 6, false, false);
                RB_STRING_strncpymax(pString, fieldValueString, 7);
            }
            else
            {
                RB_STRING_strncpymax(pString, "      ", 7);
            }
            break;
        }
        
        case ASCII_AVERAGE_TOTAL_WEIGHT:
        {
            if (m_DynamicApplet_Status == DYNAMICAPPLET_SHOWAVEWEIGHT || m_DynamicApplet_Status == DYNAMICAPPLET_WAITTOEND)
            {
                RB_FORMAT_Double(string, dp, (float64)(g_DynamicWeighing.totalWeight), 10);
                FormatOutputWeightString(string, fieldValueString, 6, false, false);
                RB_STRING_strncpymax(pString, fieldValueString, 7);
            }
            else
            {
                RB_STRING_strncpymax(pString, "      ", 7);
            }
            break;
        }
    }
    pString += 6;
    FormatOutputWeightString(pScale->tareString, tareString, 6, false, false);
    RB_STRING_strncpymax(pString, tareString, 7);
    pString += 6;
    *pString++ = RB_ASCII_CR;
    buildChecksum(pOutString);
}

/**---------------------------------------------------------------------
 * Name         : buildMultiContinous2OutputString
 * Description  : Build MT muti continuous 1 output string, 
 *                total size is 18 bytes, including checksum bytes
 * Prototype in : 
 * \param       : pScale  stand scale struct;    pOutString  the out str
 * \return      : none
 *---------------------------------------------------------------------*/
static void buildMultiContinous2OutputString(SCALE *pScale, char *pOutString, CON1_DATA_TYPE type)
{
    char * pString = pOutString; 
    char string[10] = {0}; 
    char fieldValueString[10] = {0}; 
    char tareString[10] = {0}; 
    uint8_t wordC = 0x20; 
    int32_t dp = SCALE_GetDp(scale.currInc); 
    
    if (dp < 1)
        dp = 0; 
    CONT_BuildContinousOutputString(&scale, pString); 
    if (type == ASCII_QUANTITY_OF_ANIMAL)
        *(pString + 1) = 0x2A; 
    pString += 3; 
    memset(pString, 0, 15); 
    
    if (SCALE_CheckScaleStatus(&scale) == SCALE_IN_EXPAND)
        wordC |= 0x10; 
    if (g_bPrintRequest)
        wordC |= 0x04; 
    switch (type)
    {
        case ASCII_QUANTITY_OF_ANIMAL:
        {
            wordC |= 0x01; 
            *pString++ = wordC; 
            RB_FORMAT_ULong(string, g_DynamicWeighing.animalNumber, 6); 
            RB_STRING_AlignRight(string, 6); 
            RB_STRING_strncpymax(pString, string, 7); 
            break;
        }
        
        case ASCII_AVERAGE_WEIGHT_PER_ANIMAL:
        {
            wordC |= 0x02; 
            *pString++ = wordC; 
            if (m_DynamicApplet_Status == DYNAMICAPPLET_SHOWAVEWEIGHT 
                    || (m_PreviousDynamicApplet_Status == DYNAMICAPPLET_SHOWAVEWEIGHT && (m_DynamicApplet_Status == DYNAMICAPPLET_ENTERDATA || m_DynamicApplet_Status == DYNAMICAPPLET_ENTERID)))
            {
                RB_FORMAT_Double(string, dp, (float64)(g_DynamicWeighing.averageWeight), 10);
                FormatOutputWeightString(string, fieldValueString, 6, false, false);
                RB_STRING_strncpymax(pString, fieldValueString, 7);
            }
            else
            {
                RB_STRING_strncpymax(pString, "      ", 7);
            }
            break;
        }
        
        case ASCII_AVERAGE_TOTAL_WEIGHT:
        {
            wordC |= 0x03; 
            *pString++ = wordC; 
            if (m_DynamicApplet_Status == DYNAMICAPPLET_SHOWAVEWEIGHT 
                    || (m_PreviousDynamicApplet_Status == DYNAMICAPPLET_SHOWAVEWEIGHT && (m_DynamicApplet_Status == DYNAMICAPPLET_ENTERDATA || m_DynamicApplet_Status == DYNAMICAPPLET_ENTERID)))
            {
                RB_FORMAT_Double(string, dp, (float64)(g_DynamicWeighing.totalWeight), 10);
                FormatOutputWeightString(string, fieldValueString, 6, false, false);
                RB_STRING_strncpymax(pString, fieldValueString, 7);
            }
            else
            {
                RB_STRING_strncpymax(pString, "      ", 7);
            }
            break;
        }
    }
    
    pString += 6; 
    FormatOutputWeightString(pScale->tareString, tareString, 6, false,false);
    RB_STRING_strncpymax(pString, tareString, 7);
    pString += 6;
    *pString++ = RB_ASCII_CR;
    buildChecksum(pOutString);
}

/****************************************************************************
New continus out put process

*****************************************************************************/
//--------------------------------------------------------------------------------------------
// Com1 Continus out put
//--------------------------------------------------------------------------------------------
static void COM1ContinusOutPutProcess(void)
{  
  
   char outBuf[30] = {0};
   int  sendlenth = 0;
   if ( scale.setupCOM1Assignment == COMASSIGNMENT_CONTINUOUSOUTPUT ) 
   {
       CONT_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17; 
   }
   if ( scale.setupCOM1Assignment == COMASSIGNMENT_8142CONTINUOUSOUTPUT )
   {
       CONT8142_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17;
   }
   if ( scale.setupCOM1Assignment == COMASSIGNMENT_KINGBIRDCONTINUOUSOUTPUT )
   {
       CONTKingbird_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17;
   }
   if ( scale.setupCOM1Assignment == COMASSIGNMENT_EXTENDEDCONTINUOUSOUTPUT )
   {
       CONT_BuildContinuousExtendedOutputString(&scale, outBuf, 0x31);
       sendlenth = 24;
   }   

  //confirm the send string lenth
   if( scale.setupCOM1AssignmentChecksum &&(sendlenth != 0))
       sendlenth += 1; // have checksum, string lenth +1;
 ///////////////////////////////////////////////////////////////////////////////
 // animal output process ,it is different compare with the above 
   if (scale.setupCOM1Assignment == COMASSIGNMENT_MULTI1)
   {
     static CON1_DATA_TYPE multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous1OutputString(&scale, outBuf, multiCon1type);
     sendlenth = 18;
     multiCon1type++; 
     if (multiCon1type > ASCII_AVERAGE_TOTAL_WEIGHT)
         multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     
   }
   
   if(scale.setupCOM1Assignment == COMASSIGNMENT_MULTI2)
   {
     static CON1_DATA_TYPE multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous2OutputString(&scale, outBuf, multiCon2type); 
     sendlenth = 18;
     multiCon2type++; 
     if (multiCon2type > ASCII_AVERAGE_TOTAL_WEIGHT)
     multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
   }
   
   if(sendlenth == 0)   // no continus out put return
     return ;
   
  // then config the IO status ,via COM1 slect rs422 rs232 rs485
  //if RS485 , set IO pin output
   if( scale.setupCOM1InterFace == 2) //485
   {
       RB_GPIO_SetPin(RB_CONFIG_GPIO_422_RX_EN,1);  // RS485 receiving disable
       RB_GPIO_SetPin(RB_CONFIG_GPIO_422_TX_EN,1);  //
   }
   else
   {
       RB_GPIO_SetPin(RB_CONFIG_GPIO_422_RX_EN,0);  // RS485 receiving enable
	   RB_GPIO_SetPin(RB_CONFIG_GPIO_422_TX_EN,1);  //RS422  send enable
   }
   if ( RB_DEVICE_GetUsedSendBufferSpace(hostChannels[0]) == 0 )     // no more characters in send buffer
       RB_DEVICE_Send(hostChannels[0], outBuf, sendlenth);
   
   
}
//--------------------------------------------------------------------------------------------
//COM2 Continus out put
//--------------------------------------------------------------------------------------------
static void COM2ContinusOutPutProcess(void)
{
  
   char outBuf[30] = {0};
   int  sendlenth = 0;
   if ( scale.setupCOM2Assignment == COMASSIGNMENT_CONTINUOUSOUTPUT ) 
   {
       CONT_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17; 
   }
   if ( scale.setupCOM2Assignment == COMASSIGNMENT_8142CONTINUOUSOUTPUT )
   {
       CONT8142_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17;
   }
   if ( scale.setupCOM2Assignment == COMASSIGNMENT_KINGBIRDCONTINUOUSOUTPUT )
   {
       CONTKingbird_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17;
   }
   if ( scale.setupCOM2Assignment == COMASSIGNMENT_EXTENDEDCONTINUOUSOUTPUT )
   {
       CONT_BuildContinuousExtendedOutputString(&scale, outBuf, 0x31);
       sendlenth = 24;
   }
  //confirm the send string lenth
   if( scale.setupCOM2AssignmentChecksum &&(sendlenth != 0))
       sendlenth += 1; // have checksum, string lenth +1;
   ///////////////////////////////////////////////////////////////////////////////
 // animal output process ,it is different compare with the above 
   if (scale.setupCOM2Assignment == COMASSIGNMENT_MULTI1)
   {
     static CON1_DATA_TYPE multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous1OutputString(&scale, outBuf, multiCon1type);
     sendlenth = 18;
     multiCon1type++; 
     if (multiCon1type > ASCII_AVERAGE_TOTAL_WEIGHT)
         multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     
   }
   if(scale.setupCOM2Assignment == COMASSIGNMENT_MULTI2)
   {
     static CON1_DATA_TYPE multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous2OutputString(&scale, outBuf, multiCon2type); 
     sendlenth = 18;
     multiCon2type++; 
     if (multiCon2type > ASCII_AVERAGE_TOTAL_WEIGHT)
     multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
   }
   
    if(sendlenth == 0)   // no continus out put return
     return ;
   
  // then config the IO status ,via COM1 slect  rs232 rs485
  //if RS485 , set IO pin output
   if( scale.setupCOM2InterFace == 1) //485
   {
       RB_GPIO_SetPin(RB_CONFIG_GPIO_RS485_EN,1);
   }
   if ( RB_DEVICE_GetUsedSendBufferSpace(hostChannels[1]) == 0 )     // no more characters in send buffer
       RB_DEVICE_Send(hostChannels[1], outBuf, sendlenth);
   
   
}
//--------------------------------------------------------------------------------------------
//Ethernet Continus out put,include WIFI
//--------------------------------------------------------------------------------------------
static void EthernetCountinusOutPutProcess(void)
{
  
  char outBuf[30] = {0};
  int  sendlenth = 0;
  if( (OPTION2 != APPL_CONFIG_OPRIONBOARD_ETHERNET)&&(OPTION2 != APPL_CONFIG_OPRIONBOARD_WIFI) )  //no ethernet option board
      return ;
  if ( scale.setupEthernetAssignment == COMASSIGNMENT_CONTINUOUSOUTPUT )
  {
     CONT_BuildContinousOutputString(&scale, outBuf);
     sendlenth = 17;
  }
  if ( scale.setupEthernetAssignment == COMASSIGNMENT_8142CONTINUOUSOUTPUT )
  {
    CONT8142_BuildContinousOutputString(&scale, outBuf);
    sendlenth = 17;
  }
   if ( scale.setupEthernetAssignment == COMASSIGNMENT_KINGBIRDCONTINUOUSOUTPUT )
   {
       CONTKingbird_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17;
   }
  if ( scale.setupEthernetAssignment == COMASSIGNMENT_EXTENDEDCONTINUOUSOUTPUT )
  {
     CONT_BuildContinuousExtendedOutputString(&scale, outBuf, 0x31);  
     sendlenth = 24;
  }
  if (scale.setupEthernetChecksum &&(sendlenth != 0)) 
    sendlenth++; // have checksum, string lenth +1;
   ///////////////////////////////////////////////////////////////////////////////
 // animal output process ,it is different compare with the above 
   if (scale.setupEthernetAssignment == COMASSIGNMENT_MULTI1)
   {
     static CON1_DATA_TYPE multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous1OutputString(&scale, outBuf, multiCon1type);
     sendlenth = 18;
     multiCon1type++; 
     if (multiCon1type > ASCII_AVERAGE_TOTAL_WEIGHT)
         multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     
   }
   if(scale.setupEthernetAssignment == COMASSIGNMENT_MULTI2)
   {
     static CON1_DATA_TYPE multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous2OutputString(&scale, outBuf, multiCon2type); 
     sendlenth = 18;
     multiCon2type++; 
     if (multiCon2type > ASCII_AVERAGE_TOTAL_WEIGHT)
     multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
   }
   //
   if(sendlenth == 0)   // no continus out put return
     return ;
   if (ethernetCheckSocketConnection())
   {
        if (RB_DEVICE_GetUsedSendBufferSpace(hostChannels[3]) == 0)
        {
            if ( resetSocket )
            {
                ResetSocketFunCancel();
            }            
            RB_DEVICE_Send(hostChannels[3], outBuf, sendlenth);      
        }
   }
    else
        showErrorMessage((char *)getTextString(TID_NO_SOCKET_CONNECTION), NORMAL_INTERVAL); 

}

//--------------------------------------------------------------------------------------------
//EternetPrintClint continus output process
//-------------------------------------------------------------------------------------------
static void EthernetPrintClintCoutProcess(void)
{
  
    char outBuf[30] = {0};
  int  sendlenth = 0;
  if( (OPTION2 != APPL_CONFIG_OPRIONBOARD_ETHERNET)&&(OPTION2 != APPL_CONFIG_OPRIONBOARD_WIFI) )  //no ethernet option board
      return ;
  if ( scale.setupEthernetPrintClientAssignment == COMASSIGNMENT_CONTINUOUSOUTPUT )
  {
     CONT_BuildContinousOutputString(&scale, outBuf);
     sendlenth = 17;
  }
  if ( scale.setupEthernetPrintClientAssignment == COMASSIGNMENT_8142CONTINUOUSOUTPUT )
  {
    CONT8142_BuildContinousOutputString(&scale, outBuf);
    sendlenth = 17;
  }
   if ( scale.setupEthernetPrintClientAssignment == COMASSIGNMENT_KINGBIRDCONTINUOUSOUTPUT )
   {
       CONTKingbird_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17;
   }
  if ( scale.setupEthernetPrintClientAssignment == COMASSIGNMENT_EXTENDEDCONTINUOUSOUTPUT )
  {
     CONT_BuildContinuousExtendedOutputString(&scale, outBuf, 0x31);  
     sendlenth = 24;
  }
  if (scale.setupEthernetPrintClientChecksum &&(sendlenth != 0)) 
    sendlenth++; // have checksum, string lenth +1;
   ///////////////////////////////////////////////////////////////////////////////
 // animal output process ,it is different compare with the above 
   if (scale.setupEthernetPrintClientAssignment == COMASSIGNMENT_MULTI1)
   {
     static CON1_DATA_TYPE multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous1OutputString(&scale, outBuf, multiCon1type);
     sendlenth = 18;
     multiCon1type++; 
     if (multiCon1type > ASCII_AVERAGE_TOTAL_WEIGHT)
         multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     
   }
   if(scale.setupEthernetPrintClientAssignment == COMASSIGNMENT_MULTI2)
   {
     static CON1_DATA_TYPE multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous2OutputString(&scale, outBuf, multiCon2type); 
     sendlenth = 18;
     multiCon2type++; 
     if (multiCon2type > ASCII_AVERAGE_TOTAL_WEIGHT)
     multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
   }
   //
   if(sendlenth == 0)   // no continus out put return
     return ;
   if (ethernetCheckSocketConnection())
   {
        if (RB_DEVICE_GetUsedSendBufferSpace(hostChannels[3]) == 0)
        {
            if ( resetSocket )
            {
                ResetSocketFunCancel();
            }            
            RB_DEVICE_Send(hostChannels[3], outBuf, sendlenth);      
        }
   }
    else
        showErrorMessage((char *)getTextString(TID_NO_SOCKET_CONNECTION), NORMAL_INTERVAL); 
   
}
//--------------------------------------------------------------------------------------------
//USB COM Continus Out put
//--------------------------------------------------------------------------------------------
static void USBContinusOutPutProcess(void)
{
  
  char outBuf[30] = {0};
  int  sendlenth = 0;
  if((OPTION1 != APPL_CONFIG_OPRIONBOARD_USB)&&(OPTION1 != APPL_CONFIG_OPRIONBOARD_USB_DIO)) //no USB option board
    return ;
  if ( scale.setupUSBAssignment == COMASSIGNMENT_CONTINUOUSOUTPUT )
  {
      CONT_BuildContinousOutputString(&scale, outBuf);
      sendlenth = 17;
  }
  if(scale.setupUSBAssignment == COMASSIGNMENT_8142CONTINUOUSOUTPUT)
  {
      CONT8142_BuildContinousOutputString(&scale, outBuf);
      sendlenth = 17;
  }
   if ( scale.setupUSBAssignment == COMASSIGNMENT_KINGBIRDCONTINUOUSOUTPUT )
   {
       CONTKingbird_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17;
   }
  if(scale.setupUSBAssignment == COMASSIGNMENT_EXTENDEDCONTINUOUSOUTPUT)
  {
     CONT_BuildContinuousExtendedOutputString(&scale, outBuf, 0x31);
     sendlenth = 24;
  }
   if (scale.setupUSBAssignmentChecksum &&(sendlenth != 0))
     sendlenth++;
      
     ///////////////////////////////////////////////////////////////////////////////
 // animal output process ,it is different compare with the above 
   if (scale.setupUSBAssignment == COMASSIGNMENT_MULTI1)
   {
     static CON1_DATA_TYPE multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous1OutputString(&scale, outBuf, multiCon1type);
     sendlenth = 18;
     multiCon1type++; 
     if (multiCon1type > ASCII_AVERAGE_TOTAL_WEIGHT)
         multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     
   }
   if(scale.setupUSBAssignment == COMASSIGNMENT_MULTI2)
   {
     static CON1_DATA_TYPE multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous2OutputString(&scale, outBuf, multiCon2type); 
     sendlenth = 18;
     multiCon2type++; 
     if (multiCon2type > ASCII_AVERAGE_TOTAL_WEIGHT)
     multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
   } 
    if(sendlenth == 0)   // no continus out put return
      return ;
  if (RB_DEVICE_GetUsedSendBufferSpace(hostChannels[1]) == 0)     // no more characters in send buffer
     RB_DEVICE_Send(hostChannels[1], outBuf, sendlenth);
  
}
//-------------------------------------------------------------------------------------------- 
//bluetooth continus out put
//--------------------------------------------------------------------------------------------
static void BluetoothContinusOutPutProcess()
{
  
  char outBuf[30] = {0};
  int  sendlenth = 0;
  if(OPTION2 != APPL_CONFIG_OPRIONBOARD_BLUETOOTH) //no Blutooth board
    return ;
  
  if ( scale.setupBTAssignment == COMASSIGNMENT_CONTINUOUSOUTPUT )
  {
      CONT_BuildContinousOutputString(&scale, outBuf);
      sendlenth = 17;
  }
  if ( scale.setupBTAssignment == COMASSIGNMENT_8142CONTINUOUSOUTPUT )
  {
    CONT8142_BuildContinousOutputString(&scale, outBuf);
    sendlenth = 17;
  }
   if ( scale.setupBTAssignment == COMASSIGNMENT_KINGBIRDCONTINUOUSOUTPUT )
   {
       CONTKingbird_BuildContinousOutputString(&scale, outBuf);
       sendlenth = 17;
   }
  if ( scale.setupBTAssignment == COMASSIGNMENT_EXTENDEDCONTINUOUSOUTPUT)
  {
    CONT_BuildContinuousExtendedOutputString(&scale, outBuf, 0x31);
    sendlenth = 24;
  }
  if (scale.setupBTAssignmentChecksum &&(sendlenth != 0))
    sendlenth++;
       ///////////////////////////////////////////////////////////////////////////////
 // animal output process ,it is different compare with the above 
   if (scale.setupBTAssignment == COMASSIGNMENT_MULTI1)
   {
     static CON1_DATA_TYPE multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous1OutputString(&scale, outBuf, multiCon1type);
     sendlenth = 18;
     multiCon1type++; 
     if (multiCon1type > ASCII_AVERAGE_TOTAL_WEIGHT)
         multiCon1type = ASCII_QUANTITY_OF_ANIMAL; 
     
   }
   if(scale.setupBTAssignment == COMASSIGNMENT_MULTI2)
   {
     static CON1_DATA_TYPE multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
     buildMultiContinous2OutputString(&scale, outBuf, multiCon2type); 
     sendlenth = 18;
     multiCon2type++; 
     if (multiCon2type > ASCII_AVERAGE_TOTAL_WEIGHT)
     multiCon2type = ASCII_QUANTITY_OF_ANIMAL; 
   } 
   if(sendlenth == 0)   // no continus out put, return
     return ;
  if ((RB_DEVICE_GetUsedSendBufferSpace(hostChannels[2]) == 0) && BTconnectflag)     // no more characters in send buffer
     RB_DEVICE_Send(hostChannels[2], outBuf, sendlenth);
  
}
