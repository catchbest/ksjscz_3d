#ifndef KSJ_GIGEVISION_H__
#define KSJ_GIGEVISION_H__

#define SRC_DISCOVERY_PORT   61182				// 主机用于发送DISCOVERY广播命令的端口
#define SRC_GVCP_CMD_PORT    61183				// 主机用于GVCP命令传输的端口
#define SRC_GVSP_PORT        61184				// 主机用于GVSP数据流传输的端口

#define DST_GVCP_PORT	     3956				// GIGE设备用于应答GVCP命令的端口

//#define GVCP_PORT	          DST_GVCP_PORT
#define GVCP_HOST_PORT            59268
#define GVCP_HOST_PORT_BASLER     60614

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define STANDARD_ID         0
#define EXTENDED_ID         1

#define DATA_LEADER_FORMAT               1
#define DATA_TAILER_FORMAT               2
#define DATA_PAYLOAD_GENERIC_FORMAT      3
#define DATA_PAYLOAD_ALL_IN_FORMAT       4
#define DATA_PAYLOAD_H264_FORMAT         5
#define DATA_PAYLOAD_MULTIZONE_FORMAT    6

#pragma pack(1)

typedef union _tagGVSP_PACKET_HEADER_PACKETINFO
{
	unsigned long     Value;
    struct
    {
		unsigned long PacketId       : 24;
		unsigned long btPacketFormat : 4;			// 1 - DATA_LEADER_FORMAT    2 - DATA_TAILER_FORMAT    3 - DATA_PAYLOAD_GENERIC_FORMAT ...
		unsigned long btReserved     : 3;
		unsigned long btEI           : 1;			// 0 - STANDARD_ID  1 - EXTENDED_ID
	}Info;
} GVSP_PACKET_HEADER_PACKETINFO;

struct GVSP_PACKET_HEADER_STANDARD
{
	unsigned short                          wStatus;
	unsigned short                          wBlockId;
	GVSP_PACKET_HEADER_PACKETINFO dwPacketInfo;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Leader
struct GVSP_PACKET_LEADER_PAYLOAD
{
	unsigned long  ulTimestampH;
	unsigned long  ulTimestampL;
	unsigned long  ulImagePixelType;
	unsigned long  ulImageSizeX;
	unsigned long  ulImageSizeY;
	unsigned long  ulImageOffsetX;
	unsigned long  ulImageOffsetY;
	unsigned short usImagePaddingX;
	unsigned short usImagePaddingY;
};

struct GVSP_PACKET_LEADER_STANDARD
{
	GVSP_PACKET_HEADER_STANDARD    			 Header;
	unsigned short                           wPayloadTypeSpecific;
	unsigned short                           wPayloadType;
	GVSP_PACKET_LEADER_PAYLOAD     			 PayLoadData;
};

struct GVSP_PACKET_DATAPAYLOAD_STANDARD
{
	GVSP_PACKET_HEADER_STANDARD    Header;
	unsigned char    btPayLoad[1364];
};

struct GVSP_PACKET_TRAILER_STANDARD
{
	GVSP_PACKET_HEADER_STANDARD    Header;
	unsigned short usReserved;
	unsigned short usPayloadType;
	unsigned long  ulImageSizeY;
};

struct GVSP_PACKET_HEADER_EXTENDED
{
	unsigned short                          wStatus;
	unsigned short                          wBlockId;
	GVSP_PACKET_HEADER_PACKETINFO 			dwPacketInfo;
	unsigned long                         	dwBlockId64H;
	unsigned long                         	dwBlockId64L;
	unsigned long                         	dwPacketId32;
};

struct GVSP_PACKET_LEADER_EXTENDED
{
	GVSP_PACKET_HEADER_EXTENDED    			 Header;
	unsigned short                           wPayloadTypeSpecific;
	unsigned short                           wPayloadType;
	GVSP_PACKET_LEADER_PAYLOAD     			 PayLoadData;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct GVSP_PACKET_LEADER
{
	unsigned short usStatus;
	unsigned short usBlockID;
	unsigned long  ulPacketID;
	unsigned long  ulPayloadType;
	unsigned long  ulTimestampH;
	unsigned long  ulTimestampL;
	unsigned long  ulImagePixelType;
	unsigned long  ulImageSizeX;
	unsigned long  ulImageSizeY;
	unsigned long  ulImageOffsetX;
	unsigned long  ulImageOffsetY;
	unsigned short usImagePaddingX;
	unsigned short usImagePaddingY;
};

typedef enum
{
	ARV_GVCP_PACKET_TYPE_ACK    =	0x0000,
	ARV_GVCP_PACKET_TYPE_RESEND =	0x4200,
	ARV_GVCP_PACKET_TYPE_CMD    = 	0x4219,  // 0x4201
	ARV_GVCP_PACKET_TYPE_ERROR  =	0x8006,
}ArvGvcpPacketType;

typedef enum 
{
	ARV_GVCP_COMMAND_DISCOVERY_CMD      =	0x0002,
	ARV_GVCP_COMMAND_DISCOVERY_ACK      = 	0x0003,
	ARV_GVCP_COMMAND_BYE_CMD            =	0x0004,
	ARV_GVCP_COMMAND_BYE_ACK            =	0x0005,
	ARV_GVCP_COMMAND_PACKET_RESEND_CMD  =	0x0040,
	ARV_GVCP_COMMAND_PACKET_RESEND_ACK  =	0x0041,
	ARV_GVCP_COMMAND_READ_REGISTER_CMD  =	0x0080,
	ARV_GVCP_COMMAND_READ_REGISTER_ACK  =	0x0081,
	ARV_GVCP_COMMAND_WRITE_REGISTER_CMD =	0x0082,
	ARV_GVCP_COMMAND_WRITE_REGISTER_ACK =	0x0083,
	ARV_GVCP_COMMAND_READ_MEMORY_CMD    =	0x0084,
	ARV_GVCP_COMMAND_READ_MEMORY_ACK    =	0x0085,
	ARV_GVCP_COMMAND_WRITE_MEMORY_CMD   =	0x0086,
	ARV_GVCP_COMMAND_WRITE_MEMORY_ACK   =   0x0087,
} ArvGvcpCommand;

///////////////////////////////////////////////////////////////////////////////////////////////////////GVCPPORT


///////////////////////////////////////////////////////////////////////////////////////////////////////GVCPCMDACK
#define GVCP_DISCOVERY_CMD        0x0002
#define GVCP_DISCOVERY_ACK        0x0003
#define GVCP_BYE_CMD              0x0004
#define GVCP_BYE_ACK              0x0005
#define GVCP_PACKET_RESEND_CMD    0x0040
#define GVCP_PACKET_RESEND_ACK    0x0041
#define GVCP_READREG_CMD          0x0080
#define GVCP_READREG_ACK          0x0081
#define GVCP_WRITEREG_CMD         0x0082
#define GVCP_WRITEREG_ACK         0x0083
#define GVCP_READMEM_CMD          0x0084
#define GVCP_READMEM_ACK          0x0085
#define GVCP_WRITEMEM_CMD         0x0086
#define GVCP_WRITEMEM_ACK         0x0087

#define KSJ_XML_OFFSET0			  0    // 0x00000200
#define KSJ_XML_OFFSET1			  0    // 0x00000400
#define KSJ_XML_SIZE			  512

#define KSJ_READREG_ADDRESS_CCP       0x00000934
#define KSJ_READREG_CMD_CCP_REQID     0x0002			// 对于Balser相机，当发送GVCP_READREG_CMD命令时，ReqId必须等于2 

///////////////////////////////////////////////////////////////////////////////////////////////////////ACKSTATUS
#define GEV_STATUS_SUCCESS    0x0000

///////////////////////////////////////////////////////////////////////////////////////////////////////HEADER
#define GVCP_CMD_HARD_CODE_KEY    0x42

///////////////////////////////////////////////////////////////////////////////////////////////////////DISCOVERY
typedef union
{
    struct
    {
		unsigned char bmReserved : 3;
		unsigned char bmAllowsBroadcast : 1; // when this bit is cleared  the device cannot broadcast its acknowledge message
		unsigned char bmUseGVCPHeader : 4;
    }bmValue;
    unsigned char btValue;
}GVCP_CMD_FLAG;

typedef struct
{
	unsigned char  ucHardCodedKey;
	GVCP_CMD_FLAG  uFlag;
	unsigned short Command;
	unsigned short Length;			// GVCP命令结构的GVCP_CMD_HEADER结构后面的数据字节数
	unsigned short ReqId;           // 相应的GVCP_ACK_HEADER中的AckId应该与此相同
} GVCP_CMD_HEADER;

typedef struct
{
	unsigned short Status;          // GEV_STATUS_SUCCESS
	unsigned short Answer; 	        // GVCP_xxxx_ACK
	unsigned short Length;		    // GVCP响应结构的GVCP_ACK_HEADER结构后面的数据字节数
	unsigned short AckId;
}GVCP_ACK_HEADER;


typedef struct
{
	GVCP_ACK_HEADER AckHeader;
	unsigned short  SpecVersionMajor;
	unsigned short  SpecVersionMinor;
	unsigned int    DeviceMode;
	unsigned short  Reserved;
	unsigned short  DeviceMacAddressH;
	unsigned int    DeviceMacAddressL;
	unsigned int    IpConfigOptions;
	unsigned int    IpConfigCurrent;
	unsigned int    Reserved0;
	unsigned int    Reserved1;
	unsigned int    Reserved2;
	unsigned int    CurrentIp;
	unsigned int    Reserved3;
	unsigned int    Reserved4;
	unsigned int    Reserved5;
	unsigned int    CurrentSubnetMask;
	unsigned int    Reserved6;
	unsigned int    Reserved7;
	unsigned int    Reserved8;
	unsigned int    DefaultGateway;
	char            ManufacturerName[32];
	char            ModelName[32];
	char            DeviceVersion[32];
	char            ManufacturerSpecificInformation[48];
	char            SerialNumber[16];
	char            UserDefinedName[16];
}GIGE_DISCOVERY_ACK;

///////////////////////////////////////////////////////////////////////////////////////////////////////FORCEIP

typedef struct
{
	GVCP_CMD_HEADER CmdHeader;
	unsigned short  Reserved0;
	unsigned short  DeviceMacAddressH;
	unsigned int    DeviceMacAddressL;
	unsigned int    Reserved1;
	unsigned int    Reserved2;
	unsigned int    Reserved3;
	unsigned int    ForceIp;
	unsigned int    Reserved4;
	unsigned int    Reserved5;
	unsigned int    Reserved6;
	unsigned int    CurrentSubnetMask;
	unsigned int    Reserved7;
	unsigned int    Reserved8;
	unsigned int    Reserved9;
	unsigned int    DefaultGateway;
}GVCP_FORCEIP_CMD;

///////////////////////////////////////////////////////////////////////////////////////////////////////READREG
typedef struct
{
	GVCP_CMD_HEADER CmdHeader;
	unsigned int   Address;
}GIGE_READREG_CMD;

typedef struct
{
	GVCP_ACK_HEADER AckHeader;
	unsigned int   Value;				
}GIGE_READREG_ACK;

///////////////////////////////////////////////////////////////////////////////////////////////////////WRITEREG
typedef struct
{
	unsigned int RegAddress;
	unsigned int RegData;
}WRITEREGDATA;

typedef struct
{
	GVCP_CMD_HEADER CmdHeader;
	WRITEREGDATA    WriteRegData;
}GIGE_WRITEREG_CMD;

typedef struct
{
	GVCP_ACK_HEADER AckHeader;
	unsigned short  Reserved;
	unsigned short  Index;
}GIGE_WRITEREG_ACK;

///////////////////////////////////////////////////////////////////////////////////////////////////////READMEM
typedef struct
{
	GVCP_CMD_HEADER CmdHeader;
	unsigned int   Address;
	unsigned short Reserved;
	unsigned short Count;
}GIGE_READMEM_CMD;

typedef struct
{
	GVCP_ACK_HEADER AckHeader;
	unsigned int    Address;
	unsigned char   Data[4];				        // 固定读取4字节的数据
}GIGE_READMEM_ACK;

typedef struct
{
	GVCP_ACK_HEADER AckHeader;
	unsigned int    Address;
	unsigned char   Data[KSJ_XML_SIZE];				// 固定读取KSJ_XML_SIZE字节的数据
}GIGE_READMEM_XML_ACK;

///////////////////////////////////////////////////////////////////////////////////////////////////////WRITEMEM
typedef struct
{
	GVCP_CMD_HEADER CmdHeader;
	unsigned int   Address;
	unsigned char  Data[4];                  // Value of the 8-bit data to write. The number of data to write must be a multiple of 4 bytes.
}GIGE_WRITEMEM_CMD;

typedef struct
{
	GVCP_ACK_HEADER AckHeader;
	unsigned short  Reserved;
	unsigned short  Index;
}GIGE_WRITEMEM_ACK;

#pragma pack()

#endif
