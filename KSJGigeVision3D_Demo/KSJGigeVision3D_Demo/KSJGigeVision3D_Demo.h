
// KSJGigeVision3D_Demo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CKSJGigeVision3D_DemoApp: 
// �йش����ʵ�֣������ KSJGigeVision3D_Demo.cpp
//

class CKSJGigeVision3D_DemoApp : public CWinApp
{
public:
	CKSJGigeVision3D_DemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CKSJGigeVision3D_DemoApp theApp;