
// KSJGigeVision3D_DemoDlg.h : 头文件
//

#pragma once
#include "SnapStatic.h"
#include "..\opencv.Inc\opencv\cv.hpp"
#include "..\opencv.Inc\opencv\highgui.h"
#include "..\opencv.Inc\opencv\cxcore.hpp"
#pragma comment(lib, "..\\opencv.Lib\\opencv_world300.lib")
using namespace cv;

// CKSJGigeVision3D_DemoDlg 对话框
class CKSJGigeVision3D_DemoDlg : public CDialogEx
{
// 构造
public:
	CKSJGigeVision3D_DemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CKSJGigeVision3D_DemoDlg();
// 对话框数据
	enum { IDD = IDD_KSJGIGEVISION3D_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	int m_nDeviceCurSel;
	static UINT WINAPI GvspThread(LPVOID pParam);
	void GvspThreadX();
	HANDLE m_hGvspExitEvent;
	HANDLE m_hGvspThread;
// 实现
protected:
	HICON m_hIcon;
	CSnapStatic m_SnapStatic;//显示图像控件
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
public:
	void UpdateInterface();
	afx_msg void OnCbnSelchangeComboDeviceList();
	afx_msg void OnBnClickedCheckPreviewstart();
};
