#include "CmdProcess.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "stm32f1xx_hal.h"

#include "FreeRTOS.h"
#include "cmsis_os.h"

#include "scale.h"
#include "UserParam.h"

///
//extern osMutexId myIICMutexHandle;
extern int32_t adcvalue1;
extern int32_t adcvalue2;
extern int32_t sumvalue;
extern double dFilerAdcValue;
extern double sFilerAdcValue;

double adc1_adjustcounts1,adc1_adjustcounts2;
double adc2_adjustcounts1,adc2_adjustcounts2;
double adjustk2 = 1.0;

extern void SendCom(int Nport,uint8_t *sendstr,int lenth,int timeout);

void SendOK();
void SendErr();
static void SetCapacitiyAndIncr(char *cmdstr,unsigned char cmdlenth);

static void ReadCapacityAndIncr(char *cmdstr,unsigned char cmdlenth);

static void SetTestPoint(char *cmdstr,unsigned char cmdlenth);
static void ReadTestPoint(char *cmdstr,unsigned char cmdlenth);

static void SetGeoCode(char *cmdstr,unsigned char cmdlenth);
static void ReadGeoCode(char *cmdstr,unsigned char cmdlenth);

static void SetCalibration(char *cmdstr,unsigned char cmdlenth);
static void ReadCalibration(char *cmdstr,unsigned char cmdlenth);
// power up zero or cmd set zero
static void SetZeroRang(char *cmdstr,unsigned char cmdlenth);
static void ReadZeroRang(char *cmdstr,unsigned char cmdlenth);

// SET ADC parameters,gain and speed
static void SetADC(char *cmdstr,unsigned char cmdlenth);
static void ReadADC(char *cmdstr,unsigned char cmdlenth);

static void GetWeigt(char *cmdstr,unsigned char cmdlenth);
static void GetWeigtG(char *cmdstr,unsigned char cmdlenth);
static void GetWeigtN(char *cmdstr,unsigned char cmdlenth);
static void GetWeigtT(char *cmdstr,unsigned char cmdlenth);

static void GetADCvalue(char *cmdstr,unsigned char cmdlenth);

static void SetTare(char *cmdstr,unsigned char cmdlenth);

static void ClearTare(char *cmdstr,unsigned char cmdlenth);

static void SetZero(char *cmdstr,unsigned char cmdlenth);

static void ScaleAdjust(char *cmdstr,unsigned char cmdlenth);

static void SetFilterHz(char *cmdstr,unsigned char cmdlenth);
static void SetFilterPos(char *cmdstr,unsigned char cmdlenth);
static void ResetSys(char *cmdstr,unsigned char cmdlenth);
static void ResetParamters(char *cmdstr,unsigned char cmdlenth);

uint8_t machine_addr;

//CmdFramStruct cmdfram,respfram;

char respsendbuf[200]={0};

CmdStruct SetCmdArry[25] =
{
    
    { "SETCI",		5,	SetCapacitiyAndIncr },
	{ "READCI",		6,	ReadCapacityAndIncr },
	{ "SETTP",		5,	SetTestPoint },
	{ "READTP",		6,	ReadTestPoint },
	{ "SETGEO",		6,	SetGeoCode },
	{ "READGEO",	7,	ReadGeoCode },
    { "SETCAL_D",   8,  SetCalibration},
    { "READCAL_D",  9,  ReadCalibration},
    { "SETZRANG",   8,  SetZeroRang},
    { "READZRANG",  9,  ReadZeroRang},
    {"SETADC",      6,  SetADC},
    {"READADC",     7,  ReadADC},
    {"GETWA",        5,  GetWeigt},
    {"GETWG",        5,  GetWeigtG},
    {"GETWN",        5,  GetWeigtN},
    {"GETWT",        5,  GetWeigtT},
    {"GADCV",       5,  GetADCvalue},
    {"SETTA",       5,  SetTare},
	{"CLEARTA",     7,  ClearTare},
    {"ZEROZ",       5,  SetZero},
    {"AJUST",       5,  ScaleAdjust},
    {"SETFPOLS",    8,  SetFilterPos},
    {"SETFHZ",       6,  SetFilterHz},
    {"RESET",       5,  ResetSys},
    {"PRESET",      6,  ResetParamters},    //25
};


void SendOK(int Nport)
{
    SendCom(Nport,"OK\r\n",strlen("OK\r\n"),0);
}

void SendErr(int Nport)
{
    SendCom(Nport,"CMD error\r\n",strlen("CMD error\r\n"),0);
}

static void SetCapacitiyAndIncr(char *cmdstr,unsigned char cmdlenth)
{
    double tmpcap = 20.0;  //default
    double tmpinr = 0.1;   //default
    char  *ptrtmp = NULL;
    int retvalue = 0;
    ptrtmp = (cmdstr + cmdlenth);
    
    retvalue = sscanf(ptrtmp,"%lf %lf",&tmpcap,&tmpinr);
    
    if(retvalue == 2)
    {
       if(0== CheckIncrIsValid(tmpinr)&&((tmpcap>100*tmpinr)&&(tmpcap<tmpinr*300000)))
       {
           // save these paramters
          USER_PARAM_Set(BLK0_setupRangeOneCapacity, (uint8_t *)(&tmpcap));
          USER_PARAM_Set(BLK0_setupRangeOneIncrement, (uint8_t *)(&tmpinr));
          InitScaleParamters(&g_ScaleData);
          SendOK(1);
       }
        else
        {
            SendCom(1,"data error\r\n",strlen("data error\r\n"),0);
        }
    }
    else
        SendErr(1);
    
}


static void SetFilterHz(char *cmdstr,unsigned char cmdlenth)
{
  double tmphz = 2.0;  //default
  
    char  *ptrtmp = NULL;
    int retvalue = 0;
    ptrtmp = (cmdstr + cmdlenth);    
    retvalue = sscanf(ptrtmp,"%lf",&tmphz);    
    if(retvalue == 1)
    {
       if((tmphz>0.1)&&(tmphz<9.9))
       {
           // save these paramters
          USER_PARAM_Set(BLK0_setupLowPassFilter,   (uint8_t*)&(tmphz)); 

          initialize_filter();
//          InitScaleParamters(&g_ScaleData);
          SendOK(1);
       }
        else
        {
            SendCom(1,"data error\r\n",strlen("data error\r\n"),0);
        }
    }
    else
        SendErr(1);
    
  
  
}


static void SetFilterPos(char *cmdstr,unsigned char cmdlenth)
{
  
  unsigned char tmppol = 0;  //default
   int tmpint;
    char  *ptrtmp = NULL;
    int retvalue = 0;
    ptrtmp = (cmdstr + cmdlenth);    
    retvalue = sscanf(ptrtmp,"%d",&tmpint);    
    
    if(retvalue == 1)
    {
      tmppol = (unsigned char)tmpint;
       if((tmppol>=2)&&(tmppol<=8))
       {
           // save these paramters
          USER_PARAM_Set(BLK0_setupFilterPols,   (uint8_t*)&(tmppol)); 

          initialize_filter();
//          InitScaleParamters(&g_ScaleData);
          SendOK(1);
       }
        else
        {
            SendCom(1,"data error\r\n",strlen("data error\r\n"),0);
        }
    }
    else
        SendErr(1);
  

   
   
}

/*

extern int32_t adcvalue1;
extern int32_t adcvalue2;

double adc1_adjustcounts1,adc1_adjustcounts2;
double adc2_adjustcounts1,adc2_adjustcounts2;
double adjustk1 = 1.0, adjustk2 = 1.0;

*/
static void ScaleAdjust(char *cmdstr,unsigned char cmdlenth)
{
  
    char  *ptrtmp = NULL;
    int tmpint = 0;  //default
    double precounts1,precounts2 ;
    ptrtmp = (cmdstr + cmdlenth);    
    int retvalue = sscanf(ptrtmp,"%d",&tmpint);   
    int times = 0;
    if(retvalue == 1)
    {
      
      switch(tmpint)
      {
      case 1:
        while(times <30)
        {
            if(fabs(adcvalue1 -precounts1) >20 ||fabs(adcvalue2 -precounts2) >20)
            {
                // 数据相差太大不算次数
                precounts1 = adcvalue1;
                precounts2 = adcvalue2;
            }
            else
            {
                precounts1 = adcvalue1; 
                precounts2 = adcvalue2;
                times++;
                adc1_adjustcounts1+=adcvalue1 ;
                adc2_adjustcounts1+=adcvalue2 ;
                SendCom(1,  "#", 1,0);
                   
            }
            osDelay(100); 
        }
        times = 0;
        precounts1 = 0;
        precounts2 = 0;
        SendOK(1);
        break;
      case 2:
        while(times <30)
        {
            if(fabs(adcvalue1 -precounts1) >20 ||fabs(adcvalue2 -precounts2) >20)
            {
                // 数据相差太大不算次数
                precounts1 = adcvalue1;
                precounts2 = adcvalue2;
            }
            else
            {
                precounts1 = adcvalue1; 
                precounts2 = adcvalue2;
                times++;
                adc1_adjustcounts2+=adcvalue1 ;
                adc2_adjustcounts2+=adcvalue2 ;
                SendCom(1,  "#", 1,0);                   
            }
            osDelay(100); 
        }
        times = 0;
        precounts1 = 0;
        precounts2 = 0; 
        adjustk2 = (adc1_adjustcounts1 - adc1_adjustcounts2)/(adc2_adjustcounts2 - adc2_adjustcounts1);
        
        g_ScaleData.adjutk2 = adjustk2;
        USER_PARAM_Set(BLK0_ADJUST_K2,                    (uint8_t *)&adjustk2); 
        SendOK(1);
        break;
      default:
        SendErr(1);
        break;
      }
      

    }
    else
        SendErr(1);
    
}

static void ResetSys(char *cmdstr,unsigned char cmdlenth)
{
  SendCom(1,  "System will reset!", strlen("System will reset !"),0); 
  osDelay(100);
  HAL_NVIC_SystemReset();
  
}


static void ResetParamters(char *cmdstr,unsigned char cmdlenth)
{
  //
  char tmpchar[21]={0};
  memcpy(tmpchar,"Scale 2",strlen("Scale 2"));
  USER_PARAM_Set(BLK0_setupScaleName, (uint8_t *)tmpchar);
  SendCom(1,  "System will reset!", strlen("System will reset !"),0); 
  osDelay(100);
  HAL_NVIC_SystemReset();
}

// 
static void ReadCapacityAndIncr(char *cmdstr,unsigned char cmdlenth)
{
  
    double tmpcap = 0.0;  //default
    double tmpinr = 0.0;   //default
    USER_PARAM_Get(BLK0_setupRangeOneCapacity, (uint8_t *)(&tmpcap)); 
    USER_PARAM_Get(BLK0_setupRangeOneIncrement, (uint8_t *)(&tmpinr)); 
    memset(respsendbuf,0,sizeof(respsendbuf));
    sprintf(respsendbuf,"%lf,%lf\r\n",tmpcap,tmpinr);
    SendCom(1,  (uint8_t*)respsendbuf, strlen(respsendbuf),0);
}

static void SetTestPoint(char *cmdstr,unsigned char cmdlenth)
{
    
}
static void ReadTestPoint(char *cmdstr,unsigned char cmdlenth)
{
    
}

static void SetGeoCode(char *cmdstr,unsigned char cmdlenth)
{
    int tempint;
    uint8_t tempgeo;
    if(1==sscanf((cmdstr+cmdlenth),"%d ",&tempint))
    {
       if(tempint <0||tempint>31)
       {
         SendCom(1,"GEO value must 0-31\r\n",strlen("GEO value must 0-31\r\n"),0);
       }
       else
       {
            tempgeo = (uint8_t)tempint;
            USER_PARAM_Set(BLK0_usrGeo, (uint8_t *)(&tempgeo)); 
            SendOK(1);
            InitScaleParamters(&g_ScaleData);
       }
    }
}

static void ReadGeoCode(char *cmdstr,unsigned char cmdlenth)
{
    uint8_t calgeo,usergeo;
    USER_PARAM_Get(BLK0_calGeo       , (uint8_t *)(&calgeo));    
    USER_PARAM_Get(BLK0_usrGeo       , (uint8_t *)(&usergeo)); 
    memset(respsendbuf,0,sizeof(respsendbuf));
    sprintf(respsendbuf,"CalGEO = %d,UserGEO = %d\r\n",calgeo ,usergeo);
    SendCom(1,  (uint8_t*)respsendbuf, strlen(respsendbuf),0);
   
    
    
}
// cmd SETCAL_D 0 0:  校准点1 0kg采集counts
// CMD SETCAL_D 1 20
static void SetCalibration(char *cmdstr,unsigned char cmdlenth)
{
    int testpoint;    // 校准点    
    double calcounts = 0;     // 获取的counts值
    double calweight; // 校准重量
    int times = 0;
    double precounts ;
    if(2==sscanf((cmdstr+cmdlenth),"%d %lf",&testpoint,&calweight))
    {
        //
        while(times <30)
        {
            if(fabs(dFilerAdcValue -precounts) >20)
            {
                // 数据相差太大不算次数
                precounts = dFilerAdcValue;
            }
            else
            {
                precounts = dFilerAdcValue ;
                times++;
                calcounts+=dFilerAdcValue ;
                SendCom(1,  "#", 1,0);
                   
            }
            osDelay(100); 
        }
        calcounts/=30;    
        CAL_AdjustCalParams(&g_ScaleData, testpoint, calweight, (int32_t)calcounts);

        switch(testpoint)
        {
        case 0:
            reInitializeScaleParameters(&g_ScaleData,CAL_INITZERO);
            break;
        case 1:
            reInitializeScaleParameters(&g_ScaleData,CAL_INITSPAN);

            break;
        case 2:
            break;
        default:
            break;
        }
//        SCALE_Init(&g_ScaleData); 

    }
    else
        SendErr(1);
    
   
       
}

static void ReadCalibration(char *cmdstr,unsigned char cmdlenth)
{
    
}
// power up zero or cmd set zero
static void SetZeroRang(char *cmdstr,unsigned char cmdlenth)
{
    
}

static void ReadZeroRang(char *cmdstr,unsigned char cmdlenth)
{
    
}

// SET ADC parameters,gain and speed
static void SetADC(char *cmdstr,unsigned char cmdlenth)
{
    
}

static void ReadADC(char *cmdstr,unsigned char cmdlenth)
{
    
}


static void GetWeigt(char *cmdstr,unsigned char cmdlenth)
{
     SendCom(1,  (uint8_t*)g_ScaleData.grossString, strlen(g_ScaleData.grossString),0);
     osDelay(5);
     SendCom(1,  "\r\n", 2,0);
     SendCom(1,  (uint8_t*)g_ScaleData.netString, strlen(g_ScaleData.netString),0);
     osDelay(5);
     SendCom(1,  "\r\n", 2,0);
     SendCom(1,  (uint8_t*)g_ScaleData.netString, strlen(g_ScaleData.netString),0);
     osDelay(5);
     SendCom(1,  "\r\n", 2,0);
}

static void GetWeigtG(char *cmdstr,unsigned char cmdlenth)
{
     SendCom(1,  (uint8_t*)g_ScaleData.grossString, strlen(g_ScaleData.grossString),0);
     osDelay(5);
     SendCom(1,  "\r\n", 2,0);
  
}

static void GetWeigtN(char *cmdstr,unsigned char cmdlenth)
{
     SendCom(1,  (uint8_t*)g_ScaleData.netString, strlen(g_ScaleData.netString),0);
     osDelay(5);
     SendCom(1,  "\r\n", 2,0);
}

static void GetWeigtT(char *cmdstr,unsigned char cmdlenth)
{
     SendCom(1,  (uint8_t*)g_ScaleData.tareString, strlen(g_ScaleData.tareString),0);
     osDelay(5);
     SendCom(1,  "\r\n", 2,0);
}


static void GetADCvalue(char *cmdstr,unsigned char cmdlenth)
{
    
    sprintf(respsendbuf,"%d,%d,%d,%lf,%lf\r\n",adcvalue1 ,adcvalue2,sumvalue,dFilerAdcValue,sFilerAdcValue);
    SendCom(1,  (uint8_t*)respsendbuf, strlen(respsendbuf),0);
    
}

static void SetTare(char *cmdstr,unsigned char cmdlenth)
{
    if(*cmdstr == *cmdstr);
  
    g_ScaleData.bTareCommand = 1;
    SendOK(1);
}

static void ClearTare(char *cmdstr,unsigned char cmdlenth)
{
    if(*cmdstr == *cmdstr);
  
    g_ScaleData.bClearCommand = 1;
    SendOK(1);
}

static void SetZero(char *cmdstr,unsigned char cmdlenth)
{
    if(*cmdstr == *cmdstr);
  
    g_ScaleData.bZeroCommand = 1;
    SendOK(1);  
}



void SetCmdProcess(char *recstr,CmdStruct *Ptrcmdstruct)
{
     
     uint8_t i;
     for (i = 0; i < 25; i++)
     {
          if (0 != strncmp(Ptrcmdstruct->cmdstr, recstr,Ptrcmdstruct->cmdstrlenth))
          {
              Ptrcmdstruct++;
              continue;
          }
          else
          {
              Ptrcmdstruct->cmdproc(recstr,Ptrcmdstruct->cmdstrlenth);
              break;

          }
      }
    
}