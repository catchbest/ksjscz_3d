#ifndef __KSJ_GIGEVISION_API_H__
#define __KSJ_GIGEVISION_API_H__
#include "KSJ_GIGEVISION.h"
#include <string>
using namespace std;

#define KSJGV_RET_SUCCESS      0
#define KSJGV_RET_FAIL         -1
#define KSJGV_RET_IP           -2
#define KSJGV_RET_PARAM_ERRPR  -3


#define  VCDLL_EXPORTS
#ifdef VCDLL_EXPORTS
#define KSJGIGEVISIONAPI_API __declspec(dllexport)
#else
#define KSJGIGEVISIONAPI_API __declspec(dllimport)
#endif

struct ADAPTER_INFO
{
	TCHAR         szFriendlyName[64];
	TCHAR         szDescription[128];
	unsigned char ucDns[6];
	unsigned char ucIp[6];
	unsigned char ucMac[8];
};

#ifdef __cplusplus
extern "C"{
#endif

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_Init(int nType = 0, char* lpszServerIP = NULL);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_UnInit();

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_AdapterGetCount();

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_AdapterGetInformation(int nAdapterIndex, ADAPTER_INFO * pInfo);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraGetCount(int nAdapterIndex);

struct CAMERA_INFO
{
	unsigned short  SpecVersionMajor;
	unsigned short  SpecVersionMinor;
	unsigned int    DeviceMode;
	unsigned short  DeviceMacAddressH;
	unsigned int    DeviceMacAddressL;
	unsigned int    IpConfigOptions;
	unsigned int    IpConfigCurrent;
	unsigned int    CurrentIp;
	unsigned int    CurrentSubnetMask;
	unsigned int    DefaultGateway;
	char            ManufacturerName[32];
	char            ModelName[32];
	char            DeviceVersion[32];
	char            ManufacturerSpecificInformation[48];
	char            SerialNumber[16];
	char            UserDefinedName[17];
};

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraGetInformation(int nAdapterIndex, int nCameraIndex, CAMERA_INFO *pCameraInfo);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraForceIp(int nAdapterIndex, int nCameraIndex, char* szIP);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraRefresh(int nAdapterIndex, int nCameraIndex, GIGE_DISCOVERY_ACK* DiscoveryAck);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_ReadRegister(int nAdapterIndex, int nCameraIndex, unsigned int RegAddress, unsigned int* pRegData);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_WriteRegister(int nAdapterIndex, int nCameraIndex, unsigned int RegAddress, unsigned int RegData);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_ReadMem(int nAdapterIndex, int nCameraIndex, unsigned int MemAddress, unsigned short MemLength, unsigned char* pMemData);


KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraStartGvsp(int nAdapterIndex, int nCameraIndex);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraStopGvsp(int nAdapterIndex, int nCameraIndex);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraSetGvspPort(int nAdapterIndex, int nCameraIndex, int nGvspPort);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraSetGvspSize(int nAdapterIndex, int nCameraIndex, int nGvspSize);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraGetGvspData(int nAdapterIndex, int nCameraIndex, char* pGvspData);

KSJGIGEVISIONAPI_API int __stdcall KSJGIGEVISION_CameraGet3DGvspData(int nAdapterIndex, int nCameraIndex, int nType, char* pGvspData, int *pnWidth, int *pnHeight);
#ifdef __cplusplus
}
#endif

#endif
