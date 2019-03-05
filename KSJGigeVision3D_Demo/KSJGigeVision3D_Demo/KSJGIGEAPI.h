


int KSJGIGE_Init();
int KSJGIGE_UnInit();
int KSJGIGE_GetCount();
int KSJGIGE_GetInformation(int nIndex, CAMERA_INFO *pCameraInfo);
int KSJGIGE_GetCaptureFieldOfView(int nIndex, unsigned int *pulColSize, unsigned int *pulRowSize);
int KSJGIGE_GetType(int nIndex, unsigned int *pulType);
int KSJGIGE_SetPacketSize(int nIndex, unsigned int nSize);
int KSJGIGE_GetPacketSize(int nIndex, unsigned int *pnSize);
int KSJGIGE_SetGvspSize(int nIndex, int nSize);
int KSJGIGE_SetGvspPort(int nIndex, int nPort);
int KSJGIGE_StartGvsp(int nIndex, bool bStart);
int KSJGIGE_GetGvspData(int nIndex, int nType, char* pGvspData, int *pnWidth, int *pnHeight);