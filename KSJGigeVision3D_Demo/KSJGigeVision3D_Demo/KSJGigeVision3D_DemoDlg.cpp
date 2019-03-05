
// KSJGigeVision3D_DemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KSJGigeVision3D_Demo.h"
#include "KSJGigeVision3D_DemoDlg.h"
#include "afxdialogex.h"
#include "KSJGIGEAPI.h"
#include <fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKSJGigeVision3D_DemoDlg 对话框



CKSJGigeVision3D_DemoDlg::CKSJGigeVision3D_DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKSJGigeVision3D_DemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	KSJGIGE_Init();//初始化相机

	m_hGvspExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_nDeviceCurSel = -1;
}

CKSJGigeVision3D_DemoDlg::~CKSJGigeVision3D_DemoDlg()
{
	KSJGIGE_UnInit();//释放相机资源
}

void CKSJGigeVision3D_DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PREVIEWWND, m_SnapStatic);
}

BEGIN_MESSAGE_MAP(CKSJGigeVision3D_DemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_LIST, &CKSJGigeVision3D_DemoDlg::OnCbnSelchangeComboDeviceList)
	ON_BN_CLICKED(IDC_CHECK_PREVIEWSTART, &CKSJGigeVision3D_DemoDlg::OnBnClickedCheckPreviewstart)
END_MESSAGE_MAP()


// CKSJGigeVision3D_DemoDlg 消息处理程序

BOOL CKSJGigeVision3D_DemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	CComboBox    *pComboBox = NULL;
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICE_LIST);
	pComboBox->ResetContent();
	int nCameraCount = KSJGIGE_GetCount();//获取相机数量
	TCHAR  szDeviceNum[64] = { '\0' };
	sprintf_s(szDeviceNum, _T("%d Device Found:"), nCameraCount);
	GetDlgItem(IDC_STATIC_DEVICENUM)->SetWindowText(szDeviceNum);
	CAMERA_INFO DiscoveryAck;
	char szDev[32] = { 0 };
	for (int i = 0; i < nCameraCount; i++)//获取每个相机的参数插入到顶部下拉框中
	{
		KSJGIGE_GetInformation(i, &DiscoveryAck);
		sprintf_s(szDev, "%s-%s", DiscoveryAck.ModelName, DiscoveryAck.SerialNumber);
		pComboBox->AddString(szDev);
	}

	GetDlgItem(IDC_STATIC_PREVIEWWND)->MoveWindow(20, 80, 1280, 256);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKSJGigeVision3D_DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CKSJGigeVision3D_DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CKSJGigeVision3D_DemoDlg::OnCbnSelchangeComboDeviceList()
{
	CComboBox    *pComboBox = NULL;
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICE_LIST);
	m_nDeviceCurSel = pComboBox->GetCurSel();//下拉框重新选择相机
	UpdateInterface();
}

void CKSJGigeVision3D_DemoDlg::UpdateInterface()
{
	if (m_nDeviceCurSel == -1)   return;

	CString strType[3] = { "Grya", "Profile", "3D" };//相机模式：0 灰度，1 Profile，2 3D
	unsigned int nColSize;
	unsigned int nRowSize;
	KSJGIGE_GetCaptureFieldOfView(m_nDeviceCurSel, &nColSize, &nRowSize);
	unsigned int nType;
	KSJGIGE_GetType(m_nDeviceCurSel, &nType);
	SetDlgItemInt(IDC_EDIT_COL_SIZE, nColSize);
	SetDlgItemInt(IDC_EDIT_ROW_SIZE, nRowSize);
	GetDlgItem(IDC_EDIT_ROW_TYPE)->SetWindowText(strType[nType]);
}

void CKSJGigeVision3D_DemoDlg::OnBnClickedCheckPreviewstart()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_PREVIEWSTART))->GetCheck();
	if (bCheck)
	{
		DWORD dwThreadID;
		ResetEvent(m_hGvspExitEvent);//启动线程进行采集
		m_hGvspThread = (HANDLE)_beginthreadex(NULL, 0, (PBEGINTHREADEX_FUNC)CKSJGigeVision3D_DemoDlg::GvspThread, this, 0, (PBEGINTHREADEX_ID)&dwThreadID);
	}
	else
	{
		if (m_hGvspExitEvent != NULL)//关闭线程停止采集
		{
			SetEvent(m_hGvspExitEvent);
		}
	}
}


UINT WINAPI CKSJGigeVision3D_DemoDlg::GvspThread(LPVOID pParam)
{
	CKSJGigeVision3D_DemoDlg *pKSJDlg = (CKSJGigeVision3D_DemoDlg*)pParam;
	pKSJDlg->GvspThreadX();

	return 0;
}

void CKSJGigeVision3D_DemoDlg::GvspThreadX()
{
	int nRet = 0;
	unsigned int uiGvspport = 0xcbbb;
	unsigned int uiGvspsize = 0;
	unsigned short usGvspsize = 0;
	KSJGIGE_SetGvspPort(m_nDeviceCurSel, uiGvspport);//设置gvsp端口
	KSJGIGE_GetPacketSize(m_nDeviceCurSel, &uiGvspsize);//获取包大小
	usGvspsize = uiGvspsize;
	KSJGIGE_SetGvspSize(m_nDeviceCurSel, usGvspsize);//设置包大小
	char* imagedata = NULL;

	unsigned int nColSize;
	unsigned int nRowSize;
	KSJGIGE_GetCaptureFieldOfView(m_nDeviceCurSel, &nColSize, &nRowSize);//获取相机宽高
	unsigned int nType;
	KSJGIGE_GetType(m_nDeviceCurSel, &nType);//获取相机采集类型
	int nCount = 0;
	int nMod = 0;
	int i = 0;
	imagedata = new char[nColSize * nRowSize * 8];//为采集数据分配空间
	KSJGIGE_StartGvsp(m_nDeviceCurSel, true);//发送采集命令
	int nWidth = nColSize;
	int nHeight = nRowSize;
	Mat  ImageOutput;
	float *profile;
	float *profilex;
	char szText[16] = { 0 };
	char szMsg[64] = { 0 };
	ofstream pcdfile;//有默认构造函数
	float fProfiley;
	GetDlgItem(IDC_EDIT_Y)->GetWindowText(szText, 16);
	fProfiley = atof(szText);
	nCount = nWidth * nHeight;
	float fZmapHigh, fZmapLow;
	GetDlgItem(IDC_EDIT_LOW)->GetWindowText(szText, 16);
	fZmapLow = atof(szText);
	GetDlgItem(IDC_EDIT_HIGH)->GetWindowText(szText, 16);
	fZmapHigh = atof(szText);
	float unit = (fZmapHigh - fZmapLow) / 255;
	int Index = 0;
	char* idata = NULL;
	idata = new char[nCount];//3d类型显示图片
	while (true)
	{
		if (WaitForSingleObject(m_hGvspExitEvent, 0) == 0)//停止采集，离开线程函数
		{
			KSJGIGE_StartGvsp(m_nDeviceCurSel, false);//发送停止采集命令
			if (imagedata != NULL) delete[] imagedata;//是否采集空间
			if (idata != NULL) delete[] idata;//是否采集空间
			imagedata = NULL;
			idata = NULL;
			break;
		}

		nRet = KSJGIGE_GetGvspData(m_nDeviceCurSel, nType, imagedata, &nWidth, &nHeight);//采集相机一帧
		if (nRet == 1)//采集成功
		{
			if (nType == 0)//gray
			{
				m_SnapStatic.LoadImage((BYTE*)imagedata, nWidth, nHeight, 8);//在控件上显示
			}
			else if (nType == 1)//profile
			{
				profile = (float*)imagedata;//从数据中获取z坐标
				profilex = (float*)(imagedata)+nWidth;// *4);//从数据中获取x坐标
				Mat matDisplay = Mat::zeros(Size(nWidth, 256), CV_8UC1);//新建图片
				for (i = 1; i < nWidth; ++i)//绘制图片
				{
					if (profile[i - 1] != -1000 && profile[i] != -1000 && profilex[i - 1] > 0 && profilex[i] > 0)
					{
						line(matDisplay,
							Point(profilex[i - 1] * 1280 / 50, 256 - profile[i - 1] * 256 / 10),
							Point(profilex[i] * 1280 / 50, 256 - profile[i] * 256 / 10),
							Scalar(255, 0, 0),
							1);
					}
				}
				ImageOutput = matDisplay;
				m_SnapStatic.LoadImage(ImageOutput.data, nWidth, 256, 8);//在控件上显示
			}
			else if (nType == 2)//3d
			{
				profile = (float*)imagedata;//从数据中获取z坐标
				profilex = (float*)(imagedata)+ nCount;// *4);//从数据中获取x坐标

				Index = 0;
				for (int i = 0; i < nHeight; ++i)
				{
					for (int j = 0; j < nWidth; ++j)
					{
						if (profile[Index] <= fZmapHigh && profile[Index] >= fZmapLow)
						{
							idata[Index] = (profile[Index] - fZmapLow) / unit + 0.5;
						}
						else
						{
							idata[Index] = 0;
						}

						++Index;
					}
				}

				m_SnapStatic.LoadImage((BYTE*)idata, nWidth, nHeight, 8);//在控件上显示
				BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_SAVE))->GetCheck();
				if (bCheck)
				{
					pcdfile.open("test.pcd", ios::binary);//保存一帧3d数据到pcd文件内
					char temp[64] = "# .PCD v0.7 - Point Cloud Data file format";
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					strcpy(temp, "VERSION 0.7");
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					strcpy(temp, "FIELDS x y z");
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					strcpy(temp, "SIZE 4 4 4");
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					strcpy(temp, "TYPE F F F");
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					strcpy(temp, "COUNT 1 1 1");
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					sprintf(temp, "WIDTH %d", nColSize);
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					sprintf(temp, "HEIGHT %d", nRowSize);
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					strcpy(temp, "VIEWPOINT 0 0 0 1 0 0 0");
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					sprintf(temp, "POINTS %d", nCount);
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);
					strcpy(temp, "DATA ascii");
					pcdfile.write(temp, strlen(temp));
					pcdfile.write("\n", 1);

					for (i = 0; i < nCount; i++)
					{
						if (profilex[i] == -1000) strcpy(szMsg, "nan");
						else sprintf_s(szMsg, "%.5f %.5f %.5f", profilex[i], fProfiley * (i / 1280), profile[i]);
						pcdfile.write(szMsg, strlen(szMsg));
						if (i != nCount - 1) pcdfile.write("\n", 1);
					}

					pcdfile.close();
				}
			}
		}
	}
}