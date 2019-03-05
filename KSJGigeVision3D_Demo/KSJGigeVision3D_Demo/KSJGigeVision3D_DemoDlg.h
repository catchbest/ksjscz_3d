
// KSJGigeVision3D_DemoDlg.h : ͷ�ļ�
//

#pragma once
#include "SnapStatic.h"
#include "..\opencv.Inc\opencv\cv.hpp"
#include "..\opencv.Inc\opencv\highgui.h"
#include "..\opencv.Inc\opencv\cxcore.hpp"
#pragma comment(lib, "..\\opencv.Lib\\opencv_world300.lib")
using namespace cv;

// CKSJGigeVision3D_DemoDlg �Ի���
class CKSJGigeVision3D_DemoDlg : public CDialogEx
{
// ����
public:
	CKSJGigeVision3D_DemoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CKSJGigeVision3D_DemoDlg();
// �Ի�������
	enum { IDD = IDD_KSJGIGEVISION3D_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	int m_nDeviceCurSel;
	static UINT WINAPI GvspThread(LPVOID pParam);
	void GvspThreadX();
	HANDLE m_hGvspExitEvent;
	HANDLE m_hGvspThread;
// ʵ��
protected:
	HICON m_hIcon;
	CSnapStatic m_SnapStatic;//��ʾͼ��ؼ�
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
public:
	void UpdateInterface();
	afx_msg void OnCbnSelchangeComboDeviceList();
	afx_msg void OnBnClickedCheckPreviewstart();
};
