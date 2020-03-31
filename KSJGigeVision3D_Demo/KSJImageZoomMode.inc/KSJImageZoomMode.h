#ifndef _KSJ_IMAGE_ZOOM_MODE_H__
#define _KSJ_IMAGE_ZOOM_MODE_H__

// #pragma message("Include KSJImageZoomMode.h") 

#if defined (_WIN32)
#	ifdef KSJIMAGEZOOMMODE_EXPORTS
#		define KSJIZM_API __declspec(dllexport) int __stdcall
#	elif defined KSJIMAGEZOOMMODE_STATIC
#		define KSJIZM_API
#	else
#	define KSJIZM_API __declspec(dllimport) int __stdcall
#endif

#elif defined (__GNUC__) && (__GNUC__ >= 4) && (defined (__linux__) || defined (__APPLE__))
#   if defined (__i386__)
#        define KSJIZM_API __attribute__((visibility("default"))) int __attribute__((stdcall))
#   else
#        define KSJIZM_API __attribute__((visibility("default"))) int
#  endif

#else
#  error Unknown platform, file needs adaption
#endif



#ifdef __cplusplus
extern "C"{
#endif

#ifndef HANDLE
	typedef void* HANDLE;
#endif

#define KSJIZM_RET_SUCCESS    0
#define KSJIZM_RET_FAIL       -1


	enum KSJ_ZOOM_MODE { ZM_FITIMG, ZM_FITWND, ZM_ACTUALSIZE, ZM_ZOOM, ZM_IN, ZM_OUT };

	KSJIZM_API     KSJIZM_GetVersion(int *pnVer1, int *pnVer2);

	KSJIZM_API     KSJIZM_Init( HANDLE *hZoom, int nImageW, int nImageH, int nClientW, int nClientH);
	KSJIZM_API     KSJIZM_UnInit(HANDLE *hZoom);

	KSJIZM_API     KSJIZM_MapPointClientToImage(HANDLE hZoom, int nClientX, int nClientY, int *pnImageX, int *pnImageY);
	KSJIZM_API     KSJIZM_MapPointImageToClient(HANDLE hZoom, int nImageX, int nImageY, int *pnClientX, int *pnClientY);

	KSJIZM_API     KSJIZM_MapPointClientToImageEx(HANDLE hZoom, double dClientX, double dClientY, double *pdImageX, double *pdImageY);
	KSJIZM_API     KSJIZM_MapPointImageToClientEx(HANDLE hZoom, double dImageX, double dImageY, double *pdClientX, double *pdClientY);

	// 当图像尺寸发生改变时，需要通知库
	KSJIZM_API     KSJIZM_SetImageInfo(HANDLE hZoom, int nImageW, int nImageH);

	// 当显示的窗口客户区发生改变时需要调用，以更新库内的信息
	KSJIZM_API     KSJIZM_SetClientInfo(HANDLE hZoom, int nClientW, int nClientH);

	KSJIZM_API     KSJIZM_SetZoomMode(HANDLE hZoom, KSJ_ZOOM_MODE ZoomMode);
	KSJIZM_API     KSJIZM_GetZoomMode(HANDLE hZoom, KSJ_ZOOM_MODE *pZoomMode);

	// ZoomIn,ZoomOut调用之前必须设置为ZM_ZOOM模式
	KSJIZM_API     KSJIZM_ZoomIn(HANDLE hZoom, int nX, int nY);   // 以原始图像上的(nX, nY)为中心放大
	KSJIZM_API     KSJIZM_ZoomOut(HANDLE hZoom, int nX, int nY);  // 以原始图像上的(nX, nY)为中心缩小

	KSJIZM_API     KSJIZM_Offset(HANDLE hZoom, int nX, int nY);   // 移动相对位置
	KSJIZM_API     KSJIZM_SetShowOffset(HANDLE hZoom, int nX, int nY);   // 相对位置

	// 获取当前的缩放比例
	KSJIZM_API     KSJIZM_GetZoomFactor(HANDLE hZoom, double *pdZoomH, double *pdZoomV);
	KSJIZM_API     KSJIZM_SetZoomFactor(HANDLE hZoom, double dZoomH, double dZoomV);

	// 根据图像、客户区的尺寸，以及用户的设置模式以及相对偏移，计算出的图像显示位置
	// 对于调用了如下函数之后，
	// KSJIZM_Init, 
	// KSJIZM_SetImageInfo
	// KSJIZM_SetClientInfo
	// KSJIZM_SetZoomMode
	// KSJIZM_ZoomIn
	// KSJIZM_ZoomOut
	// KSJIZM_Offset
	// 并在需要显示之前，必须通过调用此函数获取应该显示的位置，
	// 得到显示位置后，用户可以调用自己的SetPosition函数并进行显示重绘
	KSJIZM_API     KSJIZM_GetPosition(HANDLE hZoom, int *pnX, int *pnY, int *pnW, int *pnH);   // 移动相对位置



#ifdef __cplusplus
}
#endif


#endif