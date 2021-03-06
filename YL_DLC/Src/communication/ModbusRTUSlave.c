#include "gpio.h"
#include "usart.h"

extern void SendCom(int Nport,uint8_t *sendstr,int lenth,int timeout);
//extern void StartTargetProcessExtend(void);
//extern double GetOvrUndTargetValueFromShareData(int type);
//extern void SetOvrUndTargetValueToShareData(int type, double wgt);
//extern int UartDrvMapNportToDevnum(int nport);

#define MODBUSRTU_CHANNEL_MAXNUM	1				//最大通道数
#define MODBUSRTU_COMPORT		1
#define MODBUSRTU_FRAME_MAXNUM		15				//最大支持数据帧长度/写数据帧长度，包括检验码

#define FUNC_CODE_READ	0x03
#define FUNC_CODE_SET   0x06
#define FUNC_CODE_ERR   0x80

#define QUERY_FRAME_LEN 0x08

#define SLAVEADDR_LEN	0x01
#define FUNC_LEN 	0x01
#define SIZE_LEN	0x01
#define CRC_LEN		0x02

#define PLC_MAX_WT_DIGITS 12


//0x03、0x06全支持；0x10支持最长4*word
//如果需要加长0x10命令处理区，可以直接加长该定义

typedef union{
	char byte[2];
	unsigned short word;
} CHAR2USHORT;//数据分解处理

typedef struct
{
	char ptr;																			//帧长度
	char saveptr;																		//数据在FrameBuf中的位置
	unsigned short crc_save;															//CRC数据存储
} FRAME_STRUCT;//帧处理结构

typedef struct
{
	unsigned char 	SeriesNO;															//资源号
	unsigned char 	ModbusAddress;														//接收地址
	int 			HeadPTR;															//接收头指针
	unsigned char	FrameBuf[MODBUSRTU_FRAME_MAXNUM+1];									//接收数据处理区
	FRAME_STRUCT	SubFrame[MODBUSRTU_FRAME_MAXNUM+1];									//命令处理结构
	unsigned char	CommandReadBuf[MODBUSRTU_FRAME_MAXNUM+1];		//命令buf + 读buf合一，减少数据转存消耗
} MODBUSRTU_FRAM_PARSE;//通道结构

typedef struct {
	CHAR2USHORT addr;
	CHAR2USHORT len;
	CHAR2USHORT crc;
	CHAR2USHORT data;	
}MODBUS_RTU_SLAVE_QUERY;

typedef struct {
	int len;
	char func_code;
	unsigned short iterator;
	CHAR2USHORT crc;	
	uint8_t data[80];
}MODBUS_RTU_SLAVE_RESPONSE;

typedef struct {
	//char			ID;	// ID = 1 for integer messages
	union {
		unsigned short 	word;	/*2 bytes*/
		unsigned char	byte[2];
	} flags;
	
	char	grossString[PLC_MAX_WT_DIGITS+1];
	char	netString[PLC_MAX_WT_DIGITS+1];
	char	tareString[PLC_MAX_WT_DIGITS+1];	
	char 	auxRateString[PLC_MAX_WT_DIGITS+1];
	double	decade;
	double  Increment;	
	double GrossWeight;
	double NetWeight;
	double DisplayWeight;
	double TareWeight;
	double	setpoint1Wt;
	double  auxrate;
	
}INTEGER_REMOTE_IO_MESSAGE;

extern int SetSetpointCommonFlags(unsigned short * flags);

//==================================================================================================
//  G L O B A L   V A R I A B L E S
//==================================================================================================
unsigned char g_modbus_address = 0;
unsigned char g_modbus_commport = 0;
int g_ModbusRTU_FunctionFlag = 0;
int g_ModbusRTU_CommandFlag = 0;
int g_ModbusRTU_CommandFrameBufPos = 0;
char g_modbus_increment = 0x00;


MODBUSRTU_FRAM_PARSE g_ModbusFramParse[MODBUSRTU_CHANNEL_MAXNUM];
INTEGER_REMOTE_IO_MESSAGE g_modbus_mesg;

unsigned short g_modbus_flags = 0;

static void ProcessCommand(uint8_t * query, int nport);

//---------------------------------------------------------------------------------------------------
//static unsigned short ModbusRTUCRCByte(unsigned short CrcValue, unsigned char onebyte)
//---------------------------------------------------------------------------------------------------
//! \brief		单字节增加CRC计算
//! \param[in]	unsigned short CrcValue:
//! \param[in]	unsigned char onebyte:
//! \return		unsigned short:
//---------------------------------------------------------------------------------------------------
static unsigned short ModbusRTUCRCByte(unsigned short CrcValue, unsigned char data);	//逐字CRC校验

static const unsigned short ModbusCRCTbl[256] =
{
	0x0000, 0xC1C0, 0x81C1, 0x4001, 0x01C3, 0xC003, 0x8002, 0x41C2,
	0x01C6, 0xC006, 0x8007, 0x41C7, 0x0005, 0xC1C5, 0x81C4, 0x4004,
	0x01CC, 0xC00C, 0x800D, 0x41CD, 0x000F, 0xC1CF, 0x81CE, 0x400E,
	0x000A, 0xC1CA, 0x81CB, 0x400B, 0x01C9, 0xC009, 0x8008, 0x41C8,
	0x01D8, 0xC018, 0x8019, 0x41D9, 0x001B, 0xC1DB, 0x81DA, 0x401A,
	0x001E, 0xC1DE, 0x81DF, 0x401F, 0x01DD, 0xC01D, 0x801C, 0x41DC,
	0x0014, 0xC1D4, 0x81D5, 0x4015, 0x01D7, 0xC017, 0x8016, 0x41D6,
	0x01D2, 0xC012, 0x8013, 0x41D3, 0x0011, 0xC1D1, 0x81D0, 0x4010,
	0x01F0, 0xC030, 0x8031, 0x41F1, 0x0033, 0xC1F3, 0x81F2, 0x4032,
	0x0036, 0xC1F6, 0x81F7, 0x4037, 0x01F5, 0xC035, 0x8034, 0x41F4,
	0x003C, 0xC1FC, 0x81FD, 0x403D, 0x01FF, 0xC03F, 0x803E, 0x41FE,
	0x01FA, 0xC03A, 0x803B, 0x41FB, 0x0039, 0xC1F9, 0x81F8, 0x4038,
	0x0028, 0xC1E8, 0x81E9, 0x4029, 0x01EB, 0xC02B, 0x802A, 0x41EA,
	0x01EE, 0xC02E, 0x802F, 0x41EF, 0x002D, 0xC1ED, 0x81EC, 0x402C,
	0x01E4, 0xC024, 0x8025, 0x41E5, 0x0027, 0xC1E7, 0x81E6, 0x4026,
	0x0022, 0xC1E2, 0x81E3, 0x4023, 0x01E1, 0xC021, 0x8020, 0x41E0,
	0x01A0, 0xC060, 0x8061, 0x41A1, 0x0063, 0xC1A3, 0x81A2, 0x4062,
	0x0066, 0xC1A6, 0x81A7, 0x4067, 0x01A5, 0xC065, 0x8064, 0x41A4,
	0x006C, 0xC1AC, 0x81AD, 0x406D, 0x01AF, 0xC06F, 0x806E, 0x41AE,
	0x01AA, 0xC06A, 0x806B, 0x41AB, 0x0069, 0xC1A9, 0x81A8, 0x4068,
	0x0078, 0xC1B8, 0x81B9, 0x4079, 0x01BB, 0xC07B, 0x807A, 0x41BA,
	0x01BE, 0xC07E, 0x807F, 0x41BF, 0x007D, 0xC1BD, 0x81BC, 0x407C,
	0x01B4, 0xC074, 0x8075, 0x41B5, 0x0077, 0xC1B7, 0x81B6, 0x4076,
	0x0072, 0xC1B2, 0x81B3, 0x4073, 0x01B1, 0xC071, 0x8070, 0x41B0,
	0x0050, 0xC190, 0x8191, 0x4051, 0x0193, 0xC053, 0x8052, 0x4192,
	0x0196, 0xC056, 0x8057, 0x4197, 0x0055, 0xC195, 0x8194, 0x4054,
	0x019C, 0xC05C, 0x805D, 0x419D, 0x005F, 0xC19F, 0x819E, 0x405E,
	0x005A, 0xC19A, 0x819B, 0x405B, 0x0199, 0xC059, 0x8058, 0x4198,
	0x0188, 0xC048, 0x8049, 0x4189, 0x004B, 0xC18B, 0x818A, 0x404A,
	0x004E, 0xC18E, 0x818F, 0x404F, 0x018D, 0xC04D, 0x804C, 0x418C,
	0x0044, 0xC184, 0x8185, 0x4045, 0x0187, 0xC047, 0x8046, 0x4186,
	0x0182, 0xC042, 0x8043, 0x4183, 0x0041, 0xC181, 0x8180, 0x4040
};

static unsigned short ModbusRTUCRCByte(unsigned short CrcValue, unsigned char onebyte)
{
	unsigned short temp;
	temp = ModbusCRCTbl[onebyte ^ (CrcValue >> 8)];
	CrcValue = ((temp & 0xFF00) ^ (CrcValue << 8)) | (temp & 0x00FF);
	return(CrcValue);
}

static unsigned short modbus_rtu_CRC(uint8_t *crc_str, char count)
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


static void ProcessCommand(uint8_t * query, int nport)
{
	MODBUS_RTU_SLAVE_QUERY query_parsed;
	MODBUS_RTU_SLAVE_RESPONSE response;
	int needStartTargetProcessFlag = 0;

	response.func_code = FUNC_CODE_ERR;

	if (query[0] == g_modbus_address)
	{
		if (query[1] == FUNC_CODE_READ)//PLC read terminal
		{
			query_parsed.crc.word = modbus_rtu_CRC(query, QUERY_FRAME_LEN - CRC_LEN);
			if ((query_parsed.crc.byte[0] == query[QUERY_FRAME_LEN - CRC_LEN]) && (query_parsed.crc.byte[1] == query[QUERY_FRAME_LEN - CRC_LEN + 1]))//CRC check OK?
			{
				unsigned short regaddr;

				query_parsed.addr.byte[0] = query[3];//read addr from PLC query
				query_parsed.addr.byte[1] = query[2];

				query_parsed.len.byte[0] = query[5];//read len from PLC query
				query_parsed.len.byte[1] = query[4];

				response.iterator = SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN;

				for (regaddr = query_parsed.addr.word; regaddr< query_parsed.addr.word + query_parsed.len.word; regaddr++)
				{
					switch (regaddr)
					{
					case 0://40001 read gross weight
					{
							   char * 	weight;
							   short	shortWeight = 0;
							   int i = 0;
							   unsigned char	minus = 0;

							   weight = g_modbus_mesg.grossString;
							   for (i = 0; i < PLC_MAX_WT_DIGITS; i++) {
								   if (weight[i] >= '0' && weight[i] <= '9')
									   shortWeight = weight[i] - '0' + shortWeight * 10;
								   else if (weight[i] == '-')
									   minus = 1;
							   }
							   if (minus == 1){
								   shortWeight = -shortWeight;
							   }
							   response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
							   response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
							   if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
							   {
								   response.func_code = FUNC_CODE_READ;
							   }
							   break;
					}


					case 1://40002 read net weight
					{
							   char * 	weight;
							   short	shortWeight = 0;
							   int i = 0;
							   unsigned char	minus = 0;

							   weight = g_modbus_mesg.netString;
							   for (i = 0; i < PLC_MAX_WT_DIGITS; i++) {
								   if (weight[i] >= '0' && weight[i] <= '9')
									   shortWeight = weight[i] - '0' + shortWeight * 10;
								   else if (weight[i] == '-')
									   minus = 1;
							   }
							   if (minus == 1){
								   shortWeight = -shortWeight;
							   }
							   response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
							   response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
							   if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
							   {
								   response.func_code = FUNC_CODE_READ;
							   }
							   break;
					}

					case 5://40006 read Terminal status flags
					{


							   response.data[response.iterator++] = g_modbus_mesg.flags.byte[1];
							   response.data[response.iterator++] = g_modbus_mesg.flags.byte[0];

							   if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
							   {
								   response.func_code = FUNC_CODE_READ;
							   }
							   break;
					}


					case 6://40007 read increment
					{
							   response.data[response.iterator++] = 0;
							   response.data[response.iterator++] = g_modbus_increment;
							   if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
							   {
								   response.func_code = FUNC_CODE_READ;
							   }
							   break;
					}

					case 7://40008 	read Capacity		
					{

                               
//							   response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
//							   response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
							   if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
							   {
								   response.func_code = FUNC_CODE_READ;
							   }
							   break;
					}

					case 8://40009 	read rate // zhou tao edit 20141029		
					{
							   short	shortRate = 0;
							   char * 	rate;
							   int i = 0;
							   unsigned char	minus = 0;

							   rate = g_modbus_mesg.auxRateString;
							   for (i = 0; i < PLC_MAX_WT_DIGITS; i++) {
								   if (rate[i] >= '0' && rate[i] <= '9')
									   shortRate = rate[i] - '0' + shortRate * 10;
								   else if (rate[i] == '-')
									   minus = 1;
							   }
							   if (minus == 1){
								   shortRate = -shortRate;
							   }
							   response.data[response.iterator++] = ((unsigned char *)(&shortRate))[1];
							   response.data[response.iterator++] = ((unsigned char *)(&shortRate))[0];
							   if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
							   {
								   response.func_code = FUNC_CODE_READ;
							   }
							   break;
					}

					case 9://40010 	read grand total transactionCount // zhou tao edit 20141029		
					{
//							   unsigned short	shortCount = 0;
//							   unsigned long 	count;
//
//							   sd_get(&count, DI_tz0102);
//
//							   shortCount = (unsigned short)count;
//							   response.data[response.iterator++] = ((unsigned char *)(&shortCount))[1];
//							   response.data[response.iterator++] = ((unsigned char *)(&shortCount))[0];
//							   if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
//							   {
//								   response.func_code = FUNC_CODE_READ;
//							   }
							   break;
					}

					case 10://40011 	read grand total weight // zhou tao edit 20141029		
					{
//								unsigned short	shortWeight = 0;
//								double 			weight;
//								char prim_unit;
//
//								sd_get(&prim_unit, DI_ce0103);
//								sd_get(&weight, DI_tz0101);
//								weight = ConvertToCurrentUnit(prim_unit, weight);
//								shortWeight = (unsigned short)(weight*g_modbus_mesg.decade + .5);
//                                
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
								{
									response.func_code = FUNC_CODE_READ;
								}
								break;
					}

					case 11://40012 	read subtotal transactionCount // zhou tao edit 20141029
					{
//								unsigned short	shortCount = 0;
//								unsigned long 	count;
//
//								sd_get(&count, DI_tz0104);
//
//								shortCount = (unsigned short)count;
//								response.data[response.iterator++] = ((unsigned char *)(&shortCount))[1];
//								response.data[response.iterator++] = ((unsigned char *)(&shortCount))[0];
//								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
//								{
//									response.func_code = FUNC_CODE_READ;
//								}
								break;
					}

					case 12://40013 	read subtotal weight // zhou tao edit 20141029
					{
//								unsigned short	shortWeight = 0;
//								double 			weight;
//								char prim_unit;
//
//								sd_get(&prim_unit, DI_ce0103);
//								sd_get(&weight, DI_tz0103);
//								weight = ConvertToCurrentUnit(prim_unit, weight);
//								shortWeight = (unsigned short)(weight*g_modbus_mesg.decade + .5);
//                                
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
//								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
//								{
//									response.func_code = FUNC_CODE_READ;
//								}
								break;
					}

					case 19://40020 read tare weight
					{
								char * 	weight;
								short	shortWeight = 0;
								int i = 0;
								unsigned char	minus = 0;

								weight = g_modbus_mesg.tareString;
								for (i = 0; i < PLC_MAX_WT_DIGITS; i++) {
									if (weight[i] >= '0' && weight[i] <= '9')
										shortWeight = weight[i] - '0' + shortWeight * 10;
									else if (weight[i] == '-')
										minus = 1;
								}
								if (minus == 1){
									shortWeight = -shortWeight;
								}
								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
								{
									response.func_code = FUNC_CODE_READ;
								}
								break;
					}

					case 22://40023 read target weight
					{
//								unsigned short	shortWeight = 0;
//								//sd_get(&g_modbus_mesg.setpoint1Wt, DI_sp0105);
//								g_modbus_mesg.setpoint1Wt = GetOvrUndTargetValueFromShareData(TARGET_SD_INDEX_VAL);
//								shortWeight = (unsigned short)(g_modbus_mesg.setpoint1Wt*g_modbus_mesg.decade + .5);
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
								{
									response.func_code = FUNC_CODE_READ;
								}
								break;
					}

					case 23://40024 read spill weight
					{
//								unsigned short	shortWeight = 0;
//								double 	spill;
//								char prim_unit;
//
//								sd_get(&prim_unit, DI_ce0103);
//								sd_get(&spill, DI_sp0109);
//                                spill = ConvertToCurrentUnit(prim_unit, spill);
//								shortWeight = (unsigned short)(spill*g_modbus_mesg.decade + .5);
//                                
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
								{
									response.func_code = FUNC_CODE_READ;
								}
								break;
					}

					case 24://40025 read fine feed weight
					{
//								unsigned short	shortWeight = 0;
//								double	finefeed;
//								char prim_unit;
//
//								sd_get(&prim_unit, DI_ce0103);                                
//								sd_get(&finefeed, DI_sp0110);
//                                finefeed = ConvertToCurrentUnit(prim_unit, finefeed);
//								shortWeight = (unsigned short)(finefeed*g_modbus_mesg.decade + .5);
//                                
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
								{
									response.func_code = FUNC_CODE_READ;
								}
								break;
					}

					case 26://40027 read + tolerance value
					{
//								short	shortWeight = 0;
//								double	ptol;
//								//sd_get(&ptol, DI_sp0111);
//								ptol = GetOvrUndTargetValueFromShareData(TARGET_SD_INDEX_UPLIMIT);
//								shortWeight = (short)(ptol*g_modbus_mesg.decade + .5);
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
								{
									response.func_code = FUNC_CODE_READ;
								}
								break;
					}

					case 27://40028 read - tolerance value
					{
//								short	shortWeight = 0;
//								double	ntol;
//								//sd_get(&ntol, DI_sp0112);
//								ntol = GetOvrUndTargetValueFromShareData(TARGET_SD_INDEX_LOWLIMIT);
//								shortWeight = (short)(ntol*g_modbus_mesg.decade + .5);
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[1];
//								response.data[response.iterator++] = ((unsigned char *)(&shortWeight))[0];
								if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
								{
									response.func_code = FUNC_CODE_READ;
								}
								break;
					}

					default:
					{
                               if(regaddr > 50)
                               {
                                    break;
                               }
                               
							   response.data[response.iterator++] = 0x00;
							   response.data[response.iterator++] = 0x00;
							   if (response.iterator >= SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2)
							   {
								   response.func_code = FUNC_CODE_READ;
								   break;
							   }
					}

					}
				}
			}
			else {//CRC check Error!
				response.func_code = FUNC_CODE_READ | FUNC_CODE_ERR;
			}

			//================send back response==========================
			if (response.func_code == FUNC_CODE_READ)
			{
				response.data[0] = g_modbus_address;
				response.data[1] = response.func_code;
				response.data[2] = query_parsed.len.word * 2;
				response.crc.word = modbus_rtu_CRC(response.data, SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2);
				response.data[SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2] = response.crc.byte[0];
				response.data[SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2 + 1] = response.crc.byte[1];
				response.len = SLAVEADDR_LEN + FUNC_LEN + SIZE_LEN + query_parsed.len.word * 2 + CRC_LEN;
				SendCom(nport, response.data, response.len, 0);
			}
			else//response.func_code == FUNC_CODE_READ|FUNC_CODE_ERR
			{
				response.data[0] = g_modbus_address;
				response.data[1] = response.func_code;
				response.data[2] = query_parsed.addr.byte[1];
				response.data[3] = query_parsed.addr.byte[0];
				response.data[4] = query_parsed.len.byte[1];
				response.data[5] = query_parsed.len.byte[0];
				response.crc.word = modbus_rtu_CRC(response.data, 6);
				response.data[6] = response.crc.byte[0];
				response.data[7] = response.crc.byte[1];
				response.len = 0x08;
                
				SendCom(nport, response.data, response.len, 0);

			}
			//===========================================================


		}

		else if (query[1] == FUNC_CODE_SET)//PLC write terminal
		{
			query_parsed.crc.word = modbus_rtu_CRC(query, QUERY_FRAME_LEN - CRC_LEN);
			if ((query_parsed.crc.byte[0] == query[QUERY_FRAME_LEN - CRC_LEN]) && (query_parsed.crc.byte[1] == query[QUERY_FRAME_LEN - CRC_LEN + 1]))//CRC check OK?
			{
				query_parsed.addr.byte[0] = query[3];//read addr from PLC query
				query_parsed.addr.byte[1] = query[2];

				query_parsed.data.byte[0] = query[5];//read data to be written to Terminal
				query_parsed.data.byte[1] = query[4];

				switch (query_parsed.addr.word)
				{
				case 19://40020 load preset tare
				{
//							double tare = 0;
//							unsigned short shortValue = (unsigned short)query_parsed.data.word;
//                            //char prim_unit;
//                            
//                            //sd_get(&prim_unit, DI_ce0103);
//							tare = shortValue / g_modbus_mesg.decade;
//							tare = RoundedWeight(tare, g_modbus_mesg.Increment);
//                            //tare = ConvertFromCurrentUnitToThisUnitIndex(prim_unit, tare);
//							sd_set(&tare, DI_wk0104);
							response.func_code = FUNC_CODE_SET;
							break;
				}

				case 22://40023 load target weight
				{
//							double target = 0;
//							unsigned short shortValue = (unsigned short)query_parsed.data.word;
//							target = shortValue / g_modbus_mesg.decade;
//							target = RoundedWeight(target, g_modbus_mesg.Increment);
//							//sd_set(&target, DI_sp0105);
//							SetOvrUndTargetValueToShareData(TARGET_SD_INDEX_VAL, target);
							response.func_code = FUNC_CODE_SET;
							needStartTargetProcessFlag = 1;
							break;
				}

				case 23://40024 load spill weight
				{
//							double spill = 0;
//							unsigned short shortValue = (unsigned short)query_parsed.data.word;
//                            char prim_unit;
//                            
//                            sd_get(&prim_unit, DI_ce0103);
//							spill = shortValue / g_modbus_mesg.decade;
//							spill = RoundedWeight(spill, g_modbus_mesg.Increment);
//                            spill = ConvertFromCurrentUnitToThisUnitIndex(prim_unit, spill);
//                            sd_set(&spill, DI_sp0109);
							response.func_code = FUNC_CODE_SET;
							needStartTargetProcessFlag = 1;
							break;
				}

				case 24://40025 load finefeed weight
				{
//							double finefeed = 0;
//							unsigned short shortValue = (unsigned short)query_parsed.data.word;
//                            char prim_unit;
//                            
//                            sd_get(&prim_unit, DI_ce0103);
//							finefeed = shortValue / g_modbus_mesg.decade;
//							finefeed = RoundedWeight(finefeed, g_modbus_mesg.Increment);
//                            finefeed = ConvertFromCurrentUnitToThisUnitIndex(prim_unit, finefeed);
//                            sd_set(&finefeed, DI_sp0110);
							response.func_code = FUNC_CODE_SET;
							needStartTargetProcessFlag = 1;
							break;
				}

				case 26://40027 load + tolerance value
				{
//							double ptol = 0;
//							short shortValue = query_parsed.data.word;
//							ptol = shortValue / g_modbus_mesg.decade;
//							ptol = RoundedWeight(ptol, g_modbus_mesg.Increment);
//							//sd_set(&ptol, DI_sp0111);
//							SetOvrUndTargetValueToShareData(TARGET_SD_INDEX_UPLIMIT, ptol);
							response.func_code = FUNC_CODE_SET;
							needStartTargetProcessFlag = 1;
							break;
				}

				case 27://40028 load - tolerance value
				{
//							double ntol = 0;
//							short shortValue = query_parsed.data.word;
//							ntol = shortValue / g_modbus_mesg.decade;
//							ntol = RoundedWeight(ntol, g_modbus_mesg.Increment);
//							//sd_set(&ntol, DI_sp0112);
//							SetOvrUndTargetValueToShareData(TARGET_SD_INDEX_LOWLIMIT, ntol);
							response.func_code = FUNC_CODE_SET;
							needStartTargetProcessFlag = 1;
							break;
				}

				case 49://40050// write T C Z P flags
				{
//							if (query_parsed.data.word & 0x01)//tare
//							{
//								unsigned char tflag = 0;
//								tflag = 0x01;
//								sd_set(&tflag, DI_wc0101);
//								response.func_code = FUNC_CODE_SET;
//							}
//							else if (query_parsed.data.word & 0x02)//clear
//							{
//								unsigned char cflag = 0;
//								cflag = 0x01;
//								sd_set(&cflag, DI_wc0102);
//								response.func_code = FUNC_CODE_SET;
//							}
//							else if (query_parsed.data.word & 0x04)//zero
//							{
//								unsigned char zflag = 0;
//								zflag = 0x01;
//								sd_set(&zflag, DI_wc0104);
//								response.func_code = FUNC_CODE_SET;
//							}
//							else if (query_parsed.data.word & 0x08)//print
//							{
//								unsigned char pflag = 0;
//								pflag = 0x01;
//								sd_set(&pflag, DI_wc0103);
//								response.func_code = FUNC_CODE_SET;
//							}
							break;
				}


				default:
				{
						   response.func_code = FUNC_CODE_SET | FUNC_CODE_ERR;
						   break;
				}

				}

			}
			else {//CRC check Error!
				response.func_code = FUNC_CODE_SET | FUNC_CODE_ERR;
			}

			//================send back response==========================
			if (response.func_code == FUNC_CODE_SET)
			{
				response.data[0] = g_modbus_address;
				response.data[1] = response.func_code;
				response.data[2] = query_parsed.addr.byte[1];
				response.data[3] = query_parsed.addr.byte[0];
				response.data[4] = query_parsed.data.byte[1];
				response.data[5] = query_parsed.data.byte[0];
				response.crc.word = modbus_rtu_CRC(response.data, 6);
				response.data[6] = response.crc.byte[0];
				response.data[7] = response.crc.byte[1];

				response.len = 8;
				SendCom(nport, response.data, response.len, 0);

				if (needStartTargetProcessFlag == 1)
				{
					StartTargetProcessExtend();
					needStartTargetProcessFlag = 0;
				}
			}
			else//response.func_code == FUNC_CODE_SET|FUNC_CODE_ERR
			{
				response.data[0] = g_modbus_address;
				response.data[1] = response.func_code;
				response.data[2] = query_parsed.addr.byte[1];
				response.data[3] = query_parsed.addr.byte[0];
				response.data[4] = query_parsed.data.byte[1];
				response.data[5] = query_parsed.data.byte[0];
				response.crc.word = modbus_rtu_CRC(response.data, 6);
				response.data[6] = response.crc.byte[0];
				response.data[7] = response.crc.byte[1];

				response.len = 8;
				SendCom(nport, response.data, response.len, 0);
			}
			//=============================================================

		}

	}
	return;
}

//static int Modbus_RTU_SetCommonFlags( unsigned short * flags)
//{
//	unsigned char over,motion,netmode,dataok;
// 	GetScaleStatus(&dataok,&over,&motion,&netmode);
// 	if(dataok==0 ||g_inscale_setupmode==1)
// 		return 0 ;		/*clear all flags*/ 		
// 		
// 	if(motion)
// 			*flags |=0x1000;
// 	if(netmode)
// 				*flags |=0x2000;		
// 	*flags |=0x8000;
//
//
//	SetSetpointCommonFlags(flags);
//
// 	return 1;
//}

 /*---------------------------------------------------------------------*
 * Name         :  void Modbus_RTU_IntegerOutputProcessing(void)
 * 
 * Description  : called in PLC_OutputProcessing,build message with INTEGER 
 			format
 		   		
 * Return value : None
 *---------------------------------------------------------------------*/
static double g_modbus_Increment_latch =0;
static double g_modbus_decade_latch =0;
void Modbus_RTU_IntegerOutputProcessing(void)
{
	int dataok;
	double dbtmp;
 	INTEGER_REMOTE_IO_MESSAGE *ptr;
 	int tmp;
 	
 	ptr = &g_modbus_mesg;
 	g_modbus_flags =0;
    // 设置模式返回
 	dataok = Modbus_RTU_SetCommonFlags(&g_modbus_flags);
 	if(g_inscale_setupmode ==1)	/*normal scale process state*/
 				dataok = 0;
 	if(dataok)
    { 				 		 		
		compy_GrossStr(ptr->grossString);
		compy_NetStr(ptr->netString);
		compy_TareStr(ptr->tareString);
		sd_get(ptr->auxRateString,DI_wt0108);
		ptr->GrossWeight =currentUnits_GetGrossWeightDouble();
		ptr->NetWeight=currentUnits_GetNetWeightDouble();
		ptr->TareWeight =currentUnits_GetTareWeightDouble();
		sd_get(&dbtmp,DI_wt0114);
		ptr->auxrate = dbtmp;	
		ptr->Increment = GetCurInc();
		if(ptr->Increment!=g_modbus_Increment_latch)
        {
			g_modbus_Increment_latch =ptr->Increment;
			tmp = (int)((1.0/g_modbus_Increment_latch) + 0.5);
			if(g_modbus_Increment_latch<0.000001)
            {//0.0000001 0.0000002 or 0.0000005
				g_modbus_decade_latch=10000000;
		  		switch(tmp)//can recognize so small a increment in modbus rtu
		  		{
		  			case 10000000:	g_modbus_increment = 0x00;	break;
		  			default:		g_modbus_increment = 0x00;	break;
		  		}				
			}
			else
            {			
			  	if( g_modbus_Increment_latch<0.00001)
                {//0.000001 0.000002 or 0.000005
			  		g_modbus_decade_latch=1000000;
			  		switch(tmp)//can not recognize so small a increment in modbus rtu
			  		{
			  			case 1000000:	g_modbus_increment = 0x00;	break;
			  			default:		g_modbus_increment = 0x00;	break;
			  		}
			  	}
			  	else
                {
			  		if(g_modbus_Increment_latch<0.0001)
                    {//0.00001 0.00002 or 0.00005
			  			g_modbus_decade_latch=100000;
				  		switch(tmp)//can not recognize so small a increment in modbus rtu
				  		{
				  			case 100000:	g_modbus_increment = 0x00;	break;
				  			default:		g_modbus_increment = 0x00;	break;
				  		}
			  		}
			  		else
                    {
			  			if(g_modbus_Increment_latch<0.001)
                        {//0.0001 0.0002 or 0.0005
			  				g_modbus_decade_latch=10000;
					  		switch(tmp)//can not recognize so small a increment in modbus rtu
					  		{
					  			case 10000:		g_modbus_increment = 0x00;	break;
					  			default:		g_modbus_increment = 0x00;	break;
					  		}
			  			}
			  			else{
			  				if(g_modbus_Increment_latch<0.01)
                            {//0.001 0.002 or 0.005
			  					g_modbus_decade_latch=1000;
						  		switch(tmp)//can recognize the increment in modbus rtu
						  		{
						  			case 1000:		g_modbus_increment = 0x00;	break;
						  			case 500:		g_modbus_increment = 0x10;	break;
						  			case 200:		g_modbus_increment = 0x20;	break;
						  			default:		g_modbus_increment = 0x00;	break;
						  		}
			  				}
			  				else
                            {
			  					if(g_modbus_Increment_latch<0.1) 
                                {//0.01 0.02 or 0.05
			  						g_modbus_decade_latch=100;
							  		switch(tmp)//can recognize the increment in modbus rtu
							  		{
							  			case 100:		g_modbus_increment = 0x30;	break;
							  			case 50:		g_modbus_increment = 0x40;	break;
							  			case 20:		g_modbus_increment = 0x50;	break;
							  			default:		g_modbus_increment = 0x00;	break;
							  		}
			  					}
			  					else
                                {
	  						 		if(g_modbus_Increment_latch<1)
                                    {//0.1 0.2 or 0.5
	  						 			g_modbus_decade_latch=10;
								  		switch(tmp)//can recognize the increment in modbus rtu
								  		{
								  			case 10:		g_modbus_increment = 0x60;	break;
								  			case 5:			g_modbus_increment = 0x70;	break;
								  			case 2:			g_modbus_increment = 0x80;	break;
								  			default:		g_modbus_increment = 0x00;	break;
								  		}
	  						 		}
	  						 		else
                                    {
	  						 			int g_modbus_Increment_latch_int = (int)g_modbus_Increment_latch;
	  						 			g_modbus_decade_latch=1;
								  		switch(g_modbus_Increment_latch_int)//can recognize the increment in modbus rtu
								  		{
								  			case 10:		g_modbus_increment = 0xC0;	break;
								  			case 5:			g_modbus_increment = 0xB0;	break;
								  			case 2:			g_modbus_increment = 0xA0;	break;
								  			case 1:			g_modbus_increment = 0x90;	break;
								  			default:		g_modbus_increment = 0x00;	break;
								  		}	  						 			
	  						 		}
			  					}		  					
			  				}
			  			}	
			  		}
			  	}
		  	}				 
		}
		ptr->decade = g_modbus_decade_latch;
		//ptr->setpoint1Wt =GetCurSetpoint1();	
		
	}
	else
    {		
			SysMemSet(ptr->grossString,0,sizeof(ptr->grossString));
			SysMemSet(ptr->netString,0,sizeof(ptr->netString));
			SysMemSet(ptr->tareString,0,sizeof(ptr->tareString));
			ptr->GrossWeight = 0;
 			ptr->NetWeight = 0;
 			ptr->TareWeight = 0;
			ptr->Increment = 0;		 	
			ptr->auxrate =0;	 	
	}
	ptr->flags.word = g_modbus_flags;
}

void DoModbusRTUSlaveProcess(int nport)
{
	char 	Recvcache[20];
	char 	cmdbuff[20];
	int		channel = 0;
	int     n = 0;

      
    memset(Recvcache, 0, sizeof(Recvcache));
    memcpy(Recvcache, &g_ModbusFramParse[channel].CommandReadBuf[0], g_ModbusFramParse[channel].SubFrame[g_ModbusRTU_CommandFrameBufPos].ptr);
    Recvcache[g_ModbusFramParse[channel].SubFrame[g_ModbusRTU_CommandFrameBufPos].ptr] = 0;
    n = g_ModbusFramParse[channel].SubFrame[g_ModbusRTU_CommandFrameBufPos].ptr;
	if(n <= 0)
	{
		return;	
	}

	memcpy(cmdbuff,Recvcache, QUERY_FRAME_LEN);	
	Modbus_RTU_IntegerOutputProcessing();
	ProcessCommand(cmdbuff, nport);
	
	g_ModbusFramParse[channel].SubFrame[g_ModbusRTU_CommandFrameBufPos].ptr = 0;
//    g_ModbusRTU_CommandFlag = 0;
}

//---------------------------------------------------------------------------------------------------
//void ModbusRTU_Init(int channel, unsigned char seriesno, unsigned char address)//初始化程序
//---------------------------------------------------------------------------------------------------
//! \brief		初始化	
//! \param[in]	int channel://默认提供两个通道资源，0==第一个；1==第二个
//! \param[in]	unsigned char seriesno://串口资源号，通过该号使用通讯口
//! \param[in]	unsigned char address://modbus用户设置通讯地址
//! \param[in]	unsigned char *buf://modbus数据缓存
//! \return		None
//---------------------------------------------------------------------------------------------------
void ModbusRTU_Init(int channel, unsigned char seriesno, unsigned char address)//初始化程序
{
	unsigned char i;

	//初始化包头解析结构
	g_ModbusFramParse[channel].SeriesNO = seriesno;
	g_ModbusFramParse[channel].HeadPTR = 0;
	g_ModbusFramParse[channel].ModbusAddress = address;

	//初始化帧数据处理区
	for (i = 0; i < MODBUSRTU_FRAME_MAXNUM; i++)
	{
		g_ModbusFramParse[channel].SubFrame[i].ptr = 0;
		g_ModbusFramParse[channel].SubFrame[i].saveptr = 0;
		g_ModbusFramParse[channel].SubFrame[i].crc_save = 0xffff;
	}
}
//---------------------------------------------------------------------------------------------------
//void ModbusRTU_Process(int channel, unsigned char data)//接收解析帧头程序
//---------------------------------------------------------------------------------------------------
//! \brief		执行
//! attention	在周期任务
//! \param[in]	int channel://通道号
//! \return		None
//---------------------------------------------------------------------------------------------------
void ModbusRTU_Process( int channel )//接收解析帧头程序
{
	int DoNumber = 0;
	char First = 0;
     channel = 0;
   int devnum_modbus = UartDrvMapNportToDevnum(g_modbus_commport);

    while(RB_UART_GetRxDataAvailable(devnum_modbus)!=0)
    {
        g_ModbusFramParse[channel].HeadPTR++;//指针++
        if (g_ModbusFramParse[channel].HeadPTR >= MODBUSRTU_FRAME_MAXNUM)
        {
            g_ModbusFramParse[channel].HeadPTR = 0;
        }

        g_ModbusFramParse[channel].FrameBuf[g_ModbusFramParse[channel].HeadPTR] =RB_UART_ReceiveChar(devnum_modbus);

        //是否存在新的帧可能，只判断1次
        if (g_ModbusFramParse[channel].FrameBuf[g_ModbusFramParse[channel].HeadPTR] == g_ModbusFramParse[channel].ModbusAddress)
        {
            First = 1;//存在新的线程
        }
        else
        {
            First = 0;//不存在新线程
        }
        //线程处理
        for (DoNumber = 0; DoNumber < MODBUSRTU_FRAME_MAXNUM; DoNumber++)//将各个线程处理一遍
        {
            //现有命令线程处理
            if (g_ModbusFramParse[channel].SubFrame[DoNumber].ptr != 0)//非空线程
            {
                g_ModbusFramParse[channel].SubFrame[DoNumber].ptr++;//指针++
                if (g_ModbusFramParse[channel].SubFrame[DoNumber].ptr > MODBUSRTU_FRAME_MAXNUM)
                {//帧线程超长，杀死该线程
                    g_ModbusFramParse[channel].SubFrame[DoNumber].ptr = 0;
                }
                else
                {//帧线程不超长
                    g_ModbusFramParse[channel].SubFrame[DoNumber].crc_save =//计算新CRC和
                        ModbusRTUCRCByte(g_ModbusFramParse[channel].SubFrame[DoNumber].crc_save, g_ModbusFramParse[channel].FrameBuf[g_ModbusFramParse[channel].HeadPTR]);
                    if (g_ModbusFramParse[channel].SubFrame[DoNumber].crc_save == 0)//是完整的数据帧
                    {
                        if (g_ModbusFramParse[channel].SubFrame[DoNumber].ptr >= 8)//异常判断，在这里比较好，理论上不需要
                        {//数据复制
                            int m;
                            for (m = 0; m < g_ModbusFramParse[channel].SubFrame[DoNumber].ptr; m++)
                            {
                                g_ModbusFramParse[channel].CommandReadBuf[m] = g_ModbusFramParse[channel].FrameBuf[g_ModbusFramParse[channel].SubFrame[DoNumber].saveptr];//数据转移
                                g_ModbusFramParse[channel].SubFrame[DoNumber].saveptr++;//指针++
                                if (g_ModbusFramParse[channel].SubFrame[DoNumber].saveptr >= MODBUSRTU_FRAME_MAXNUM)
                                {
                                    g_ModbusFramParse[channel].SubFrame[DoNumber].saveptr = 0;
                                }
                            }
                            //命令程序接口位置
    //						g_ModbusRTU_CommandFlag = 1;
                            DoModbusRTUSlaveProcess(g_modbus_commport);
                            
                            g_ModbusRTU_CommandFrameBufPos = DoNumber;
                        }
                    }
                }
            }
            //新线程增加，只增加1次
            if (First != 0)
            {
                if (g_ModbusFramParse[channel].SubFrame[DoNumber].ptr == 0)
                {
                    g_ModbusFramParse[channel].SubFrame[DoNumber].ptr = 1;//只有一个数据
                    g_ModbusFramParse[channel].SubFrame[DoNumber].saveptr = g_ModbusFramParse[channel].HeadPTR;//数据在缓冲区中的位置
                    g_ModbusFramParse[channel].SubFrame[DoNumber].crc_save = ModbusRTUCRCByte(0xffff, g_ModbusFramParse[channel].FrameBuf[g_ModbusFramParse[channel].HeadPTR]);//校验和
                    First = 0;
                }
            }
        }
    }
    
}

//void ModbusRTUSlave_Process(int comid)
//{
//    
//    char databuf[20];
//    int lenth= 0,i=0;
//    memset(databuf,0,sizeof(databuf));
//    lenth = RecvCom(comid, (char *)databuf, 20, 1);
//    while(i < lenth)
//    {
//      ModbusRTU_Process(comid, databuf[i++]);//接收解析帧头程序
//               
//    }
//        
//	if (g_ModbusRTU_FunctionFlag == 1)
//	{
//		//unsigned char scaleProcStatus;
//		//sd_get(&scaleProcStatus, DI_wt0115);
//		//if (scaleProcStatus == 1)
//		{
//			DoModbusRTUSlaveProcess(comid);
//		}
//	}
//}
