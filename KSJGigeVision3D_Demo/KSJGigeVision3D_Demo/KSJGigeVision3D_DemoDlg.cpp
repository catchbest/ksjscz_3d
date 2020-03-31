
// KSJGigeVision3D_DemoDlg.cpp : ʵ���ļ�
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


// CKSJGigeVision3D_DemoDlg �Ի���



CKSJGigeVision3D_DemoDlg::CKSJGigeVision3D_DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKSJGigeVision3D_DemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	KSJGIGE_Init();//��ʼ�����

	m_hGvspExitEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
	m_nDeviceCurSel = -1;
}

CKSJGigeVision3D_DemoDlg::~CKSJGigeVision3D_DemoDlg()
{
	KSJGIGE_UnInit();//�ͷ������Դ
	m_pWndImage->DestroyWindow();
	delete m_pWndImage;
	m_pWndImage = NULL;
}

void CKSJGigeVision3D_DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_PREVIEWWND, m_SnapStatic);
}

BEGIN_MESSAGE_MAP(CKSJGigeVision3D_DemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_LIST, &CKSJGigeVision3D_DemoDlg::OnCbnSelchangeComboDeviceList)
	ON_BN_CLICKED(IDC_CHECK_PREVIEWSTART, &CKSJGigeVision3D_DemoDlg::OnBnClickedCheckPreviewstart)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CKSJGigeVision3D_DemoDlg ��Ϣ�������

BOOL CKSJGigeVision3D_DemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	CComboBox    *pComboBox = NULL;
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICE_LIST);
	pComboBox->ResetContent();
	int nCameraCount = KSJGIGE_GetCount();//��ȡ�������
	TCHAR  szDeviceNum[64] = { '\0' };
	sprintf_s(szDeviceNum, _T("%d Device Found:"), nCameraCount);
	GetDlgItem(IDC_STATIC_DEVICENUM)->SetWindowText(szDeviceNum);
	CAMERA_INFO DiscoveryAck;
	char szDev[32] = { 0 };
	for (int i = 0; i < nCameraCount; i++)//��ȡÿ������Ĳ������뵽������������
	{
		KSJGIGE_GetInformation(i, &DiscoveryAck);
		sprintf_s(szDev, "%s-%s", DiscoveryAck.ModelName, DiscoveryAck.SerialNumber);
		pComboBox->AddString(szDev);
	}

	CRect         m_rcSrcImageWnd;
	m_rcSrcImageWnd.left = 220;
	m_rcSrcImageWnd.top = 80;
	m_rcSrcImageWnd.right = 920;
	m_rcSrcImageWnd.bottom = 420;
	m_pWndImage = new CKSJImageWnd(this, -1);
	m_pWndImage->Create(NULL, TEXT(""), WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE, m_rcSrcImageWnd, this, 0, NULL);   // 
	m_pWndImage->ShowWindow(SW_SHOW);
	m_pWndImage->SetFocus();
	m_pWndImage->SetSelected(TRUE);
	GetDlgItem(IDC_STATIC_PREVIEWWND)->MoveWindow(20, 80, 1280, 256);
	if (nCameraCount >= 1)
	{
		m_nDeviceCurSel = 0;
		pComboBox->SetCurSel(m_nDeviceCurSel);
		OnCbnSelchangeComboDeviceList();
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CKSJGigeVision3D_DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CKSJGigeVision3D_DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CKSJGigeVision3D_DemoDlg::OnCbnSelchangeComboDeviceList()
{
	CComboBox    *pComboBox = NULL;
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DEVICE_LIST);
	m_nDeviceCurSel = pComboBox->GetCurSel();//����������ѡ�����
	UpdateInterface();
}

void CKSJGigeVision3D_DemoDlg::UpdateInterface()
{
	if (m_nDeviceCurSel == -1)   return;

	CString strType[3] = { "Grya", "Profile", "3D" };//���ģʽ��0 �Ҷȣ�1 Profile��2 3D
	unsigned int nColSize;
	unsigned int nRowSize;
	KSJGIGE_GetCaptureFieldOfView(m_nDeviceCurSel, &nColSize, &nRowSize);
	unsigned int nType;
	KSJGIGE_GetType(m_nDeviceCurSel, &nType);
	SetDlgItemInt(IDC_EDIT_COL_SIZE, nColSize);
	SetDlgItemInt(IDC_EDIT_ROW_SIZE, nRowSize);
	GetDlgItem(IDC_EDIT_Y)->SetWindowText("0.1");
	SetDlgItemInt(IDC_EDIT_LOW, 0);
	SetDlgItemInt(IDC_EDIT_HIGH, 20);
	GetDlgItem(IDC_EDIT_ROW_TYPE)->SetWindowText(strType[nType]);
}

void CKSJGigeVision3D_DemoDlg::OnBnClickedCheckPreviewstart()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_PREVIEWSTART))->GetCheck();
	if (bCheck)
	{
		DWORD dwThreadID;
		ResetEvent(m_hGvspExitEvent);//�����߳̽��вɼ�
		m_hGvspThread = (HANDLE)_beginthreadex(NULL, 0, (PBEGINTHREADEX_FUNC)CKSJGigeVision3D_DemoDlg::GvspThread, this, 0, (PBEGINTHREADEX_ID)&dwThreadID);
	}
	else
	{
		if (m_hGvspExitEvent != NULL)//�ر��߳�ֹͣ�ɼ�
		{
			SetEvent(m_hGvspExitEvent);
		}

	
		while (TRUE)
		{
			DWORD result;
			MSG msg;

			result = MsgWaitForMultipleObjects(1, &m_hGvspThread,
				FALSE, INFINITE, QS_ALLINPUT);

			if (result == (WAIT_OBJECT_0))
			{
				break;
			}
			else
			{
				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
				DispatchMessage(&msg);
			}
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
	KSJGIGE_SetGvspPort(m_nDeviceCurSel, uiGvspport);//����gvsp�˿�
	KSJGIGE_GetPacketSize(m_nDeviceCurSel, &uiGvspsize);//��ȡ����С
	usGvspsize = uiGvspsize;
	KSJGIGE_SetGvspSize(m_nDeviceCurSel, usGvspsize);//���ð���С
	char* imagedata = NULL;

	unsigned int nColSize;
	unsigned int nRowSize;
	KSJGIGE_GetCaptureFieldOfView(m_nDeviceCurSel, &nColSize, &nRowSize);//��ȡ������
	unsigned int nType;
	KSJGIGE_GetType(m_nDeviceCurSel, &nType);//��ȡ����ɼ�����
	int nCount = 0;
	int nMod = 0;
	int i = 0, j = 0;
	imagedata = new char[nColSize * nRowSize * 8];//Ϊ�ɼ����ݷ���ռ�
	KSJGIGE_StartGvsp(m_nDeviceCurSel, true);//���Ͳɼ�����
	int nWidth = nColSize;
	int nHeight = nRowSize;
	Mat  ImageOutput;
	float *profile;
	float *profilex;
	float *profiley = NULL;
	profiley = new float[nColSize * nRowSize];
	char szText[16] = { 0 };
	char szMsg[64] = { 0 };
	ofstream pcdfile;//��Ĭ�Ϲ��캯��
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
	idata = new char[nCount];//3d������ʾͼƬ
	while (true)
	{
		if (WaitForSingleObject(m_hGvspExitEvent, 0) == 0)//ֹͣ�ɼ����뿪�̺߳���
		{
			KSJGIGE_StartGvsp(m_nDeviceCurSel, false);//����ֹͣ�ɼ�����
			if (imagedata != NULL) delete[] imagedata;//�ͷŲɼ��ռ�
			if (idata != NULL) delete[] idata;//�ͷŲɼ��ռ�
			imagedata = NULL;
			idata = NULL;
			if (profiley != NULL) delete[] profiley;
			profiley = NULL;
			break;
		}

		nRet = KSJGIGE_GetGvspData(m_nDeviceCurSel, nType, imagedata, &nWidth, &nHeight);//�ɼ����һ֡
		if (nRet == 1)//�ɼ��ɹ�
		{
			if (nType == 0)//gray
			{
				m_pWndImage->SetImageInfo(nWidth, nHeight, 8);
				m_pWndImage->LoadImage((BYTE*)imagedata, nWidth, nHeight, 8);
				//m_SnapStatic.LoadImage((BYTE*)imagedata, nWidth, nHeight, 8);//�ڿؼ�����ʾ
			}
			else if (nType == 1)//profile
			{
				m_pWndImage->SetImageInfo(nWidth, 256, 8);
				profile = (float*)imagedata;//�������л�ȡz����
				profilex = (float*)(imagedata)+nWidth;// *4);//�������л�ȡx����
				Mat matDisplay = Mat::zeros(Size(nWidth, 256), CV_8UC1);//�½�ͼƬ
				for (i = 1; i < nWidth; ++i)//����ͼƬ
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
				m_pWndImage->LoadImage(ImageOutput.data, nWidth, 256, 8);
				//m_SnapStatic.LoadImage(ImageOutput.data, nWidth, 256, 8);//�ڿؼ�����ʾ
			}
			else if (nType == 2)//3d
			{
				m_pWndImage->SetImageInfo(nWidth, nHeight, 8);
				profile = (float*)imagedata;//�������л�ȡz����
				profilex = (float*)(imagedata)+ nCount;// *4);//�������л�ȡx����

				Index = 0;
				for (i = 0; i < nHeight; ++i)
				{
					for (j = 0; j < nWidth; ++j)
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

				m_pWndImage->LoadImage((BYTE*)idata, nWidth, nHeight, 8);
				//m_SnapStatic.LoadImage((BYTE*)idata, nWidth, nHeight, 8);//�ڿؼ�����ʾ
				BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_SAVE))->GetCheck();
				if (bCheck)
				{
					SYSTEMTIME LocalTime;
					char szFileName[128] = { 0 };
					GetLocalTime(&LocalTime);
#ifdef halcon12
					for (i = 0; i < nCount; i++) profiley[i] = fProfiley * (i / 1280);

					HObject Hobjx, Hobjy, Hobjz;
					gen_image1(&Hobjx, "real", nWidth, nHeight, (long)profilex);//���
					gen_image1(&Hobjy, "real", nWidth, nHeight, (long)profiley);
					gen_image1(&Hobjz, "real", nWidth, nHeight, (long)profile);
					HTuple hv_ObjectModel3D;
					XyzToObjectModel3d(Hobjx, Hobjy, Hobjz, &hv_ObjectModel3D);
					sprintf_s(szFileName, "capture-%04d-%02d-%02d-%02d-%02d-%02d.om3", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
					WriteObjectModel3d(hv_ObjectModel3D, "om3", szFileName, "invert_normals", "false");
#else
					sprintf_s(szFileName, "capture-%04d-%02d-%02d-%02d-%02d-%02d.pcd", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
					pcdfile.open(szFileName, ios::binary);//����һ֡3d���ݵ�pcd�ļ���
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
						if (profilex[i] == -1000) strcpy(szMsg, "nan nan nan");
						else sprintf_s(szMsg, "%.5f %.5f %.5f", profilex[i], fProfiley * (i / 1280), profile[i]);
						pcdfile.write(szMsg, strlen(szMsg));
						if (i != nCount - 1) pcdfile.write("\n", 1);
					}

					pcdfile.close();
#endif
					
				}
			}
		}
	}
}

void CKSJGigeVision3D_DemoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_PREVIEWSTART))->GetCheck();
	if (bCheck)
	{
		((CButton*)GetDlgItem(IDC_CHECK_PREVIEWSTART))->SetCheck(FALSE);
		OnBnClickedCheckPreviewstart();
	}
}
