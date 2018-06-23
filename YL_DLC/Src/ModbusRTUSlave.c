
//#include "sd_index.h"
//#include "task_ad.h"
//#include "ServiceManager.h"
//#include "AppMemTarget.h"

//extern void StartTargetProcessExtend(void);
//extern double GetOvrUndTargetValueFromShareData(int type);
//extern void SetOvrUndTargetValueToShareData(int type, double wgt);
//extern int UartDrvMapNportToDevnum(int nport);

#include <string.h>
#include "Scale.h"
#include "usart.h"

#define MODBUSRTU_CHANNEL_MAXNUM	1				//最大通道数
#define MODBUSRTU_COMPORT		1
#define MODBUSRTU_FRAME_MAXNUM		15				//最大支持数据帧长度/写数据帧长度，包括检验码

#define FUNC_CODE_READ	0x03
#define FUNC_CODE_SET   0x06
#define FUNC_CODE_ERR   0x80

#define FRAME_LEN 0x08

#define SLAVEADDR_LEN	0x01
#define FUNC_LEN 	0x01
#define SIZE_LEN	0x01
#define CRC_LEN		0x02

#define PLC_MAX_WT_DIGITS 12

extern unsigned char ReadAddr();

//0x03、0x06全支持；0x10支持最长4*word
//如果需要加长0x10命令处理区，可以直接加长该定义

typedef union
{
	unsigned char byte[2];
	unsigned short word;
} CHAR2USHORT;// short char  union 



typedef union
{
	CHAR2USHORT shortvalue[2];
//    CHAR2USHORT shortvalue2;
	int         intvalue;
} Short2int;// double char  union 


typedef union
{
	unsigned char  byte[8];
	double         doublevalue;
} CHAR2double;// double char  union 


typedef union
{
	unsigned char  byte[4];
	int         intvalue;
} CHAR2int;// double char  union 




typedef struct
{
  char          DevicAddr;   // 设备地址
  char          FunCode;    //function code 03 06 
  CHAR2USHORT   QueryAddr;  // read or write reg address
  CHAR2USHORT   QueryDataLenth;  // read or write data length
  CHAR2USHORT   FramCrc;        // CRC 
}ModbusRTU_SlavequeryFramType;






//==================================================================================================
//  G L O B A L   V A R I A B L E S
//==================================================================================================

unsigned char g_modbus_address = 0;

unsigned char g_ReadModbusRegMapArry[100];
//unsigned char g_WtieModbusRegMapArry[100];

unsigned char g_respbuf[110];


//#ifdef RB_ENV_BSP_WIN32
//MODBUSRTU_FRAM_PARSE g_ModbusFramParse[MODBUSRTU_CHANNEL_MAXNUM];
//INTEGER_REMOTE_IO_MESSAGE g_modbus_mesg;
//#else
//MODBUSRTU_FRAM_PARSE g_ModbusFramParse[MODBUSRTU_CHANNEL_MAXNUM]@"LRAM1_MEMORY";
//INTEGER_REMOTE_IO_MESSAGE g_modbus_mesg@"LRAM1_MEMORY";
//#endif
unsigned short g_modbus_flags = 0;

int ProcessCommand(char * query, int receivelenth, int nport);

//
//static int modebus_readreg(int com,unsigned short regaddr,unsigned short lenth);
//static int modebus_writereg(int com,unsigned short regaddr,unsigned short lenth);

void FreshRegArry();

static unsigned short modbus_rtu_CRC(unsigned char *crc_str, char count)
{
     char i,j;
     unsigned short CrcValue, temp;  
     CrcValue = 0xffff;  
     for(i=0; i<count ;i++,crc_str++)
     { 
		temp = (unsigned short)*crc_str & 0x0FF;
        CrcValue ^= temp ;
        for (j=0; j<8; j++)
        {
        	if (CrcValue & 0x0001)
                CrcValue =(CrcValue >> 1) ^ 0x0A001;
            else
                CrcValue = CrcValue >> 1;
         }
	  }
      return CrcValue;
}

// 初始化 regmap
void FreshRegArry()
{ 
    Short2int tempintvalue;  
    tempintvalue.intvalue = (int)g_ScaleData.roundedNetWeight;
    g_ReadModbusRegMapArry[0] = tempintvalue.shortvalue[1].byte[1];
    g_ReadModbusRegMapArry[1] = tempintvalue.shortvalue[1].byte[0];
    g_ReadModbusRegMapArry[2] = tempintvalue.shortvalue[0].byte[1];
    g_ReadModbusRegMapArry[3] = tempintvalue.shortvalue[0].byte[0];

    tempintvalue.intvalue = (int)g_ScaleData.roundedGrossWeight;
    g_ReadModbusRegMapArry[4] = tempintvalue.shortvalue[1].byte[1];
    g_ReadModbusRegMapArry[5] = tempintvalue.shortvalue[1].byte[0];
    g_ReadModbusRegMapArry[6] = tempintvalue.shortvalue[0].byte[1];
    g_ReadModbusRegMapArry[7] = tempintvalue.shortvalue[0].byte[0];
    

    tempintvalue.intvalue = (int)g_ScaleData.roundedTareWeight;
    g_ReadModbusRegMapArry[8] = tempintvalue.shortvalue[1].byte[1];
    g_ReadModbusRegMapArry[9] = tempintvalue.shortvalue[1].byte[0];
    g_ReadModbusRegMapArry[10] = tempintvalue.shortvalue[0].byte[1];
    g_ReadModbusRegMapArry[11] = tempintvalue.shortvalue[0].byte[0];
  
  
}


static int modebus_readreg(int com,unsigned short regaddr,unsigned short lenth)
{
  
   CHAR2USHORT tmpshort;
   memset(g_respbuf,0,sizeof(g_respbuf));
   g_respbuf[0] = g_modbus_address;
   g_respbuf[1] = 0x03;
   
    switch (regaddr)    
    {
      
    case 0:      // 
    case 4:      
    case 8:
      if(lenth>(100-regaddr-1))
      {
        tmpshort.word = regaddr;
        
        g_respbuf[1]|=0x80;
        g_respbuf[2]=tmpshort.byte[1];
        g_respbuf[3]=tmpshort.byte[0];
        tmpshort.word = lenth;
        g_respbuf[4]=tmpshort.byte[1];
        g_respbuf[5]=tmpshort.byte[0];
        tmpshort.word = modbus_rtu_CRC(g_respbuf,6);
        g_respbuf[6]=tmpshort.byte[0];
        g_respbuf[7]=tmpshort.byte[1];     
        //
        SendCom(com,g_respbuf,8,0x50);
          
      }
      else
      {
        g_respbuf[2] = (unsigned char)lenth;
        memcpy(g_respbuf+3,g_ReadModbusRegMapArry,lenth);
        tmpshort.word = modbus_rtu_CRC(g_respbuf,lenth+3);
        g_respbuf[lenth+3] = tmpshort.byte[0];
        g_respbuf[lenth+3+1] = tmpshort.byte[1];
        SendCom(com,g_respbuf,lenth+3+1+1,0x50);
      }
      break;
    default:
        tmpshort.word = regaddr;        
        g_respbuf[1]|=0x80;
        g_respbuf[2]=tmpshort.byte[1];
        g_respbuf[3]=tmpshort.byte[0];
        tmpshort.word = lenth;
        g_respbuf[4]=tmpshort.byte[1];
        g_respbuf[5]=tmpshort.byte[0];
        tmpshort.word = modbus_rtu_CRC(g_respbuf,6);
        g_respbuf[6]=tmpshort.byte[0];
        g_respbuf[7]=tmpshort.byte[1];     
        //
        SendCom(com,g_respbuf,8,0x50);
        break;
    }
    return 0;
}

static int modebus_writereg(int com,unsigned short regaddr,unsigned short writedata)
{
   CHAR2USHORT tmpshort;
   memset(g_respbuf,0,sizeof(g_respbuf));
   g_respbuf[0] = g_modbus_address;
   g_respbuf[1] = 0x06;
   switch(regaddr)
   {
   case 0x0100:
     if(writedata == 1)
     {
        
        g_ScaleData.bTareCommand = 1;
        // run tare
     }
     else
     {
       g_respbuf[1] |= 0x80;
     }
     break;
   case 0x0102:
      if(writedata == 1)
     {
       g_ScaleData.bClearCommand = 1;
     }
     else
     {
       g_respbuf[1] |= 0x80;
     }
     
     break;
   default:
      g_respbuf[1] |= 0x80;
     break;
   }
   
    tmpshort.word = regaddr;
    g_respbuf[2] = tmpshort.byte[1];
    g_respbuf[3] = tmpshort.byte[0];
    tmpshort.word = writedata;
    g_respbuf[4] = tmpshort.byte[1];
    g_respbuf[5] = tmpshort.byte[0];
    tmpshort.word = modbus_rtu_CRC(g_respbuf,6);
    g_respbuf[6]=tmpshort.byte[0];
    g_respbuf[7]=tmpshort.byte[1];     
        //
     SendCom(com,g_respbuf,8,0x50);
        
  return 0;
}

int ProcessCommand(char * query, int receivelenth, int nport)
{

  return 0;
}

//---------------------------------------------------------------------------------------------------
//void ModbusRTU_Process(int channel, unsigned char data)//接收解析帧头程序
//---------------------------------------------------------------------------------------------------
//! \brief		执行
//! attention	在周期任务
//! \param[in]	int channel://通道号
//! \return		None
//---------------------------------------------------------------------------------------------------
int  ModbusRTU_Process( int com, unsigned char * rebuf,int receivelenth )//接收解析帧头程序
{

  unsigned short crc;
  CHAR2USHORT rec_crc;
  CHAR2USHORT regaddr;
  CHAR2USHORT readlenth;
  CHAR2USHORT writedate;
  unsigned char funcode;
 g_modbus_address =  ReadAddr();
     // 收完一帧数据，判断释放大于最大长度，大于扔掉
     if(receivelenth!=FRAME_LEN)
       return -1;  // 非命令帧
     if(rebuf[0] != g_modbus_address)   //
       return -2;
     
     crc = modbus_rtu_CRC(rebuf,receivelenth-2);
     rec_crc.byte[0] = rebuf[6];
     rec_crc.byte[1] = rebuf[7];
     if(crc!= rec_crc.word) //crc check error
       return -3;
     funcode = rebuf[1];  //function code
     regaddr.byte[0] = rebuf[3];  //read reg addr
     regaddr.byte[1] = rebuf[2];

     switch(funcode)
     {
     case FUNC_CODE_READ:	
       readlenth.byte[0] = rebuf[5];
       readlenth.byte[1] = rebuf[4];
       modebus_readreg(com,regaddr.word,readlenth.word*2);
       break;
     case FUNC_CODE_SET:
       writedate.byte[0] = rebuf[5];
       writedate.byte[1] = rebuf[4];
       modebus_writereg(com,regaddr.word,writedate.word*2);
       break;
     default:
       break;
     }
     return 0;
     

    
}


