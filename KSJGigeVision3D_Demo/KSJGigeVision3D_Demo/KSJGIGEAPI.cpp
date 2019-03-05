#include "stdafx.h"
#include "KSJGIGEAPI.h"

int CamareIndex[64][2];
int KSJGIGE_Init()
{
	return KSJGIGEVISION_Init();
}

int KSJGIGE_UnInit()
{
	return KSJGIGEVISION_UnInit();
}

int KSJGIGE_GetCount()
{
	int nCount = 0;
	int nAdapterIndex = KSJGIGEVISION_AdapterGetCount();
	int nCameraIndex = 0;
	for (int i = 0; i < nAdapterIndex; i++)
	{
		nCameraIndex = KSJGIGEVISION_CameraGetCount(i);
		for (int j = 0; j < nCameraIndex; j++)
		{
			CamareIndex[nCount][0] = i;
			CamareIndex[nCount][1] = j;
			nCount++;
		}
	}

	return nCount;
}

int KSJGIGE_GetInformation(int nIndex, CAMERA_INFO *pCameraInfo)
{
	return KSJGIGEVISION_CameraGetInformation(CamareIndex[nIndex][0], CamareIndex[nIndex][1], pCameraInfo);
}

int KSJGIGE_GetCaptureFieldOfView(int nIndex, unsigned int *pulColSize, unsigned int *pulRowSize)
{
	KSJGIGEVISION_ReadRegister(CamareIndex[nIndex][0], CamareIndex[nIndex][1], 0x100, pulColSize);
	KSJGIGEVISION_ReadRegister(CamareIndex[nIndex][0], CamareIndex[nIndex][1], 0x104, pulRowSize);
	return 0;
}

int KSJGIGE_GetType(int nIndex, unsigned int *pulType)
{
	return KSJGIGEVISION_ReadRegister(CamareIndex[nIndex][0], CamareIndex[nIndex][1], 0xb000, pulType);
}

int KSJGIGE_SetPacketSize(int nIndex, unsigned int nSize)
{
	return KSJGIGEVISION_WriteRegister(CamareIndex[nIndex][0], CamareIndex[nIndex][1], 0xd04, nSize);
}

int KSJGIGE_GetPacketSize(int nIndex, unsigned int *pnSize)
{
	return KSJGIGEVISION_ReadRegister(CamareIndex[nIndex][0], CamareIndex[nIndex][1], 0xd04, pnSize);
}

int KSJGIGE_SetGvspSize(int nIndex, int nSize)
{
	return KSJGIGEVISION_CameraSetGvspSize(CamareIndex[nIndex][0], CamareIndex[nIndex][1], nSize);
}

int KSJGIGE_SetGvspPort(int nIndex, int nPort)
{
	KSJGIGEVISION_WriteRegister(CamareIndex[nIndex][0], CamareIndex[nIndex][1], 0xd00, nPort);
	KSJGIGEVISION_CameraSetGvspPort(CamareIndex[nIndex][0], CamareIndex[nIndex][1], nPort);
	return 0;
}

int KSJGIGE_StartGvsp(int nIndex, bool bStart)
{
	if (bStart)
	{
		return KSJGIGEVISION_WriteRegister(CamareIndex[nIndex][0], CamareIndex[nIndex][1], 0x124, 1);
	}
	else
	{
		return 	KSJGIGEVISION_WriteRegister(CamareIndex[nIndex][0], CamareIndex[nIndex][1], 0x124, 0);

	}
}

int KSJGIGE_GetGvspData(int nIndex, int nType, char* pGvspData, int *pnWidth, int *pnHeight)
{
	return KSJGIGEVISION_CameraGet3DGvspData(CamareIndex[nIndex][0], CamareIndex[nIndex][1], nType, pGvspData, pnWidth, pnHeight);
}