#include "stdafx.h"
#include "KSJImageWnd.h"
#include "KSJGigeVision3D_DemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WIDTHBYTES(i)    ( ( i+31 ) / 32 * 4) 

#define  ZOOM_INDEX_MIN         0
#define  ZOOM_INDEX_MAX			22
#define  ZOOM_INDEX_DEFAULT     12			// 实际尺寸显示时1:1比例时，缩放比例因子的索引

static double   g_dZoomFactor[] =
{
	0.0, 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0
};

static LPCTSTR  g_szZoomFactor[] =
{
	_T("0%"), _T("1%"), _T("5%"), _T("10%"), _T("20%"), _T("30%"), _T("40%"), _T("50%"), _T("60%"), _T("70%"), _T("80%"),
	_T("90%"), _T("100%"), _T("150%"), _T("200%"), _T("300%"), _T("400%"), _T("500%"), _T("600%"), _T("700%"), _T("800%"),
	_T("900%"), _T("1000%"),
};


IMPLEMENT_DYNCREATE(CKSJImageWnd, CWnd)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CKSJImageWnd::CKSJImageWnd( CWnd *pParent, int nIndex ) : 
m_pParent( pParent ), 
m_nIndex( nIndex ),
m_pImageBmpInfo( NULL ),
m_pImageBmpData( NULL ),
m_bSelected( FALSE ),
m_hZoom(NULL),
m_dZoomFactor( 1.0 ),
m_dShowZoomH( 1.0 ),
m_dShowZoomV( 1.0 ),
m_bOpenFromFile( FALSE )
{
	m_ptOffset.x = m_ptOffset.y = 0;

	m_pImageBmpInfo    = (LPBITMAPINFO)new char[ sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) ];
	memset( (VOID*)m_pImageBmpInfo, 0, sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) );

	if( m_pImageBmpInfo == NULL )
	{
		return ;
	}

	m_pImageBmpInfo->bmiHeader.biSize			= sizeof(tagBITMAPINFOHEADER);
	m_pImageBmpInfo->bmiHeader.biPlanes			= 1;
	m_pImageBmpInfo->bmiHeader.biCompression	= BI_RGB;
	m_pImageBmpInfo->bmiHeader.biSizeImage		= 0;
	m_pImageBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	m_pImageBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	m_pImageBmpInfo->bmiHeader.biClrUsed		= 0;
	m_pImageBmpInfo->bmiHeader.biClrImportant	= 0;

	m_pImageBmpInfo->bmiHeader.biWidth			= 0;
	m_pImageBmpInfo->bmiHeader.biHeight			= 0;
	m_pImageBmpInfo->bmiHeader.biBitCount		= 0;
	m_pImageBmpInfo->bmiHeader.biSizeImage      = 0;  // m_nWidth * m_nHeight * ( m_nBitCount >> 3 );

	for ( int i=0; i<256; i++ )
	{
		m_pImageBmpInfo->bmiColors[i].rgbBlue	   = (BYTE) i;
		m_pImageBmpInfo->bmiColors[i].rgbGreen	   = (BYTE) i;
		m_pImageBmpInfo->bmiColors[i].rgbRed	   = (BYTE) i;
		m_pImageBmpInfo->bmiColors[i].rgbReserved  = (BYTE) i;
	}

	KSJIZM_Init(&m_hZoom, 0, 0, 0, 0 );

	KSJIZM_SetZoomMode(m_hZoom, ZM_FITIMG);

	memset( m_szOpenFileFullName, 0, MAX_PATH * sizeof( TCHAR ) );
}

void CKSJImageWnd::ConnectCamera(int nIndex)
{
	m_nIndex = nIndex;
}


CKSJImageWnd::~CKSJImageWnd()
{
	if (m_pImageBmpData != NULL)
	{
		delete[] m_pImageBmpData;
		m_pImageBmpData = NULL;
	}

	if( m_pImageBmpInfo )
	{
		delete [] m_pImageBmpInfo;
		m_pImageBmpInfo = NULL;
	}

	if (m_hZoom)
	{
		KSJIZM_UnInit(&m_hZoom);
		m_hZoom = NULL;
	}

}


BEGIN_MESSAGE_MAP(CKSJImageWnd, CWnd)
	//{{AFX_MSG_MAP(CKSJImageWnd)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

int CKSJImageWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifndef SHOW_HVSCROLL
	EnableScrollBarCtrl(SB_HORZ, FALSE);
	EnableScrollBarCtrl(SB_VERT, FALSE);
#else
	EnableScrollBarCtrl(SB_HORZ, TRUE);
	EnableScrollBarCtrl(SB_VERT, TRUE);
#endif

	return 0;
}

void CKSJImageWnd::SetSelected(BOOL bSelected)
{
	m_bSelected = bSelected;

	SetFocus();

	Invalidate();
}

// 当显示的图像大小发生改变时，必须调用
int CKSJImageWnd::SetImageInfo(int nWidth, int nHeight, int nBitCount)
{
	if (m_nIndex != -1)
	{
		if (m_pImageBmpData != NULL)
		{
			delete[] m_pImageBmpData;
			m_pImageBmpData = NULL;
		}

		//if (m_pImageBmpInfo)
		//{
		//	delete[] m_pImageBmpInfo;
		//	m_pImageBmpInfo = NULL;
		//}

		KSJIZM_SetImageInfo(m_hZoom, nWidth, nHeight);

		return 1;
	}


	if (m_pImageBmpData != NULL &&
		m_pImageBmpInfo->bmiHeader.biWidth == nWidth &&
		m_pImageBmpInfo->bmiHeader.biHeight == nHeight &&
		m_pImageBmpInfo->bmiHeader.biBitCount == nBitCount)
		return 1;

	m_pImageBmpInfo->bmiHeader.biWidth     = nWidth;
	m_pImageBmpInfo->bmiHeader.biHeight    = nHeight;
	m_pImageBmpInfo->bmiHeader.biBitCount  = nBitCount;
	m_pImageBmpInfo->bmiHeader.biSizeImage = nWidth * nHeight * (nBitCount >> 3);

	if (m_pImageBmpData != NULL)
	{
		delete[] m_pImageBmpData;
		m_pImageBmpData = NULL;
	}

	m_pImageBmpData = new BYTE[m_pImageBmpInfo->bmiHeader.biSizeImage];

	KSJIZM_SetImageInfo(m_hZoom, m_pImageBmpInfo->bmiHeader.biWidth, m_pImageBmpInfo->bmiHeader.biHeight);

	TCHAR szBuf[256] = { 0 };
	_stprintf_s(szBuf, 256, _T("CKSJImageWnd::SetImageInfo::KSJIZM_SetImageInfo( %d,%d )\n"), m_pImageBmpInfo->bmiHeader.biWidth, m_pImageBmpInfo->bmiHeader.biHeight);
	//OutputDebugString(szBuf);


	return 1;
}

void CKSJImageWnd::LoadImage( BYTE *pImageData, int nWidth, int nHeight, int nBitCount )
{
	if (
		m_pImageBmpInfo->bmiHeader.biWidth != nWidth ||
		m_pImageBmpInfo->bmiHeader.biHeight != nHeight ||
		m_pImageBmpInfo->bmiHeader.biBitCount != nBitCount
		)
		SetImageInfo(nWidth, nHeight, nBitCount);

	if (m_pImageBmpData == NULL)    return;

	memcpy( m_pImageBmpData, pImageData, m_pImageBmpInfo->bmiHeader.biSizeImage );

	m_dZoomFactor                   = 1.0;
	m_dShowZoomH                    = 1.0;
	m_dShowZoomV                    = 1.0;
	m_ptOffset.x = m_ptOffset.y = 0;

	Invalidate( );
}

void CKSJImageWnd::LoadImageFromFile(LPCTSTR lpszFileName )
{
	HANDLE hBmpFile = CreateFile( lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hBmpFile )	
	{
		CloseHandle( hBmpFile );
		return;
	}

	// 然后读入Bmp文件
	// 读入文件头信息
	BITMAPFILEHEADER    BmpFileHeader;
	DWORD               dwNumberOfBytesRead;
	::ReadFile( hBmpFile, &BmpFileHeader, sizeof(BITMAPFILEHEADER), &dwNumberOfBytesRead, NULL);

	if( BmpFileHeader.bfType != 0x4d42 )					return;
	if( dwNumberOfBytesRead != sizeof(BITMAPFILEHEADER) )	return;


	// 读取信息头和颜色表
	::ReadFile( hBmpFile, m_pImageBmpInfo, BmpFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER), &dwNumberOfBytesRead, NULL );

	if (m_pImageBmpData != NULL)
	{
		delete[] m_pImageBmpData;
		m_pImageBmpData = NULL;
	}

	m_pImageBmpInfo->bmiHeader.biSizeImage      = WIDTHBYTES( m_pImageBmpInfo->bmiHeader.biWidth * m_pImageBmpInfo->bmiHeader.biBitCount ) * abs( m_pImageBmpInfo->bmiHeader.biHeight ) ;

	m_pImageBmpData    = new BYTE [m_pImageBmpInfo->bmiHeader.biSizeImage];
	::ReadFile( hBmpFile, m_pImageBmpData, m_pImageBmpInfo->bmiHeader.biSizeImage, &dwNumberOfBytesRead, NULL );

	CloseHandle( hBmpFile );

	if (m_hZoom != NULL)
	{
		KSJIZM_UnInit(&m_hZoom);
		m_hZoom = NULL;
	}

	RECT   rtClient;
	GetClientRect(&rtClient);
	KSJIZM_Init(&m_hZoom, m_pImageBmpInfo->bmiHeader.biWidth, m_pImageBmpInfo->bmiHeader.biHeight, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);

	Invalidate( );

	lstrcpy( m_szOpenFileFullName, lpszFileName );    // 保存当前打开文件的完整名称
}


void CKSJImageWnd::SaveImage( LPCTSTR lpszFileName )
{
	if( m_pImageBmpData == NULL )    return;

	HANDLE hBmpFile = CreateFile( lpszFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hBmpFile )    return;

	DWORD dwImageSize = m_pImageBmpInfo->bmiHeader.biSizeImage;

    BITMAPFILEHEADER *pBmpFileHeader = (BITMAPFILEHEADER*) new BYTE [sizeof(BITMAPFILEHEADER)];

	int    nColorTableSize = ( m_pImageBmpInfo->bmiHeader.biBitCount == 8 ? 256 * sizeof( RGBQUAD ) : 0 );
	int    nBmpInfoSize    = sizeof(BITMAPINFOHEADER) + nColorTableSize ;

	pBmpFileHeader->bfType			= 0x4d42; // "BM"
    pBmpFileHeader->bfSize			= sizeof(BITMAPFILEHEADER) + nBmpInfoSize + dwImageSize; 
    pBmpFileHeader->bfReserved1		= 0; 
    pBmpFileHeader->bfReserved2		= 0; 
    pBmpFileHeader->bfOffBits		= sizeof(BITMAPFILEHEADER) + nBmpInfoSize;

	DWORD    dwNumberOfByteWritten;
	WriteFile( hBmpFile, pBmpFileHeader,  sizeof(BITMAPFILEHEADER), &dwNumberOfByteWritten, NULL );


	WriteFile( hBmpFile, m_pImageBmpInfo, nBmpInfoSize,            &dwNumberOfByteWritten, NULL );
	WriteFile( hBmpFile, m_pImageBmpData, dwImageSize,             &dwNumberOfByteWritten, NULL );

	delete [] pBmpFileHeader;

	CloseHandle( hBmpFile );
}
/////////////////////////////////////////////////////////////////////////////
// CKSJImageWnd message handlers
BOOL CKSJImageWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	return CWnd::OnEraseBkgnd(pDC);
}

void CKSJImageWnd::DrawSelectRect()
{
	COLORREF    crFrameNoraml   = GetSysColor(COLOR_WINDOWFRAME);
	COLORREF    crFrameSelected = GetSysColor(COLOR_BTNHIGHLIGHT);
	int         nFrameX = GetSystemMetrics(SM_CXFRAME);

	HDC   hWndDC = ::GetWindowDC(m_hWnd);
	int   nWndDC = ::SaveDC(hWndDC);

	RECT  rtWnd;
	GetWindowRect(&rtWnd);

	int   nWndW = rtWnd.right - rtWnd.left;
	int   nWndH = rtWnd.bottom - rtWnd.top;

	HPEN  hPen;
	if (m_bSelected)
		hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	else
		hPen = ::CreatePen(PS_SOLID, 1, crFrameNoraml);       // RGB(255, 0, 0));

	HPEN hPenOld = (HPEN)::SelectObject(hWndDC, hPen);

	::SetBkMode(hWndDC, TRANSPARENT);
	::MoveToEx(hWndDC, 0, 0, NULL);
	::LineTo(hWndDC, nWndW - 1, 0);
	::MoveToEx(hWndDC, 0, 0, NULL);
	::LineTo(hWndDC, 0, nWndH - 1);

	::MoveToEx(hWndDC, nWndW - 1, 0, NULL);
	::LineTo(hWndDC, nWndW - 1, nWndH - 1);
	::MoveToEx(hWndDC, 0, nWndH - 1, NULL);
	::LineTo(hWndDC, nWndW - 1, nWndH - 1);

	::SelectObject(hWndDC, hPenOld);
	::DeleteObject(hPen);

	::RestoreDC(hWndDC, nWndDC);
	::ReleaseDC(m_hWnd, hWndDC);

}

void CKSJImageWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (FALSE == IsWindowVisible())    return;

	DrawSelectRect();

	if (m_hZoom == NULL)    return;


	HDC   hDC = dc.GetSafeHdc();

	HDC   hMemClientDC = ::CreateCompatibleDC(hDC);
	int   nMemClientDC = ::SaveDC(hMemClientDC);


	RECT  rtClient;
	::GetClientRect(m_hWnd, &rtClient);

	int nClientW = rtClient.right - rtClient.left;
	int nClientH = rtClient.bottom - rtClient.top;

	HBITMAP hMemClientBmp    = ::CreateCompatibleBitmap(hDC, nClientW, nClientH);
	HBITMAP hMemClientBmpOld = (HBITMAP)::SelectObject(hMemClientDC, hMemClientBmp);

	HBRUSH   hMemClientBrush = ::CreateSolidBrush(RGB(112, 128, 144));
	HBRUSH   hMemClientBrushOld = (HBRUSH)::SelectObject(hMemClientDC, hMemClientBrush);// ::GetStockObject(BLACK_BRUSH));

	::Rectangle(hMemClientDC, 0, 0, nClientW, nClientH);

	KSJIZM_GetPosition(m_hZoom, &m_nShowOffsetH, &m_nShowOffsetV, &m_nShowWidth, &m_nShowHeight);
	TCHAR szBuf[256] = { 0 };
	_stprintf_s(szBuf, 256, _T("CKSJImageWnd::OnPaint( %d,  %d,%d,%d,%d)\n"), m_nIndex, m_nShowOffsetH, m_nShowOffsetV, m_nShowWidth, m_nShowHeight);
	 //OutputDebugString(szBuf);

	if (m_nIndex != -1)    // 显示视频
	{
		::BitBlt(hDC, 0, 0, nClientW, nClientH, hMemClientDC, 0, 0, SRCCOPY);
		//KSJ_PreviewSetPos(m_nIndex, m_hWnd, m_nShowOffsetH, m_nShowOffsetV, m_nShowWidth, m_nShowHeight);
	}
	else                   // 显示图片
	{
		::SetStretchBltMode(hMemClientDC, COLORONCOLOR);
		::StretchDIBits(hMemClientDC, m_nShowOffsetH, m_nShowOffsetV, m_nShowWidth, m_nShowHeight, 0, 0, m_pImageBmpInfo->bmiHeader.biWidth, m_pImageBmpInfo->bmiHeader.biHeight, m_pImageBmpData, m_pImageBmpInfo, DIB_RGB_COLORS, SRCCOPY);

		::SetStretchBltMode(hDC, COLORONCOLOR);
		::BitBlt(hDC, 0, 0, nClientW, nClientH, hMemClientDC, 0, 0, SRCCOPY);
	}

	::SelectObject(hMemClientDC, hMemClientBrushOld);
	::DeleteObject(hMemClientBrush);

	::SelectObject(hMemClientDC, hMemClientBmpOld);
	::DeleteObject(hMemClientBmp);

	::RestoreDC(hMemClientDC, nMemClientDC);
	::DeleteDC(hMemClientDC);

}

void CKSJImageWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ClientToScreen( &point );
	GetParent()->SendMessage( WM_LBUTTONDBLCLK, nFlags, point.x | (point.y<<16 ) );
	SetFocus();
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CKSJImageWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_hZoom == NULL)    return;

	// 如果此窗口还没有被选中，则这次鼠标点击只是用于选中图像，并把另一个选中的窗口设置为未选中状态
	if ( !m_bSelected )
	{
		POINT    ptScreen = point;
		ClientToScreen(&ptScreen);

		POINT    ptParentClient = ptScreen;
		((CKSJGigeVision3D_DemoDlg*)GetParent())->ScreenToClient(&ptParentClient);
		
		
		// 转换为相对父窗口的客户区坐标
		GetParent()->SendMessage(WM_LBUTTONDOWN, nFlags, ptParentClient.x | (ptParentClient.y << 16));

		return;
	}

	SetFocus();
	// 判断鼠标点击点是否在图像内，如不在则不处理
	POINT    ptImage;

	if (KSJIZM_RET_SUCCESS != KSJIZM_MapPointClientToImage(m_hZoom, point.x, point.y, (int*)&ptImage.x, (int*)&ptImage.y))
		return;

	m_ptLButtonDownInClientPre = point;
	m_ptLButtonDownInImage     = ptImage;

	CWnd::OnLButtonDown(nFlags, point);
}

void CKSJImageWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	POINT    ptScreen = point;
	ClientToScreen(&ptScreen);
	SetFocus();
	POINT    ptParentClient = ptScreen;
	((CKSJGigeVision3D_DemoDlg*)GetParent())->ScreenToClient(&ptParentClient);

	// 转换为相对父窗口的客户区坐标
	GetParent()->SendMessage(WM_RBUTTONDOWN, nFlags, ptParentClient.x | (ptParentClient.y << 16));

	CWnd::OnRButtonDown(nFlags, point);
}


// point相对窗口左上角的相对位置
void CKSJImageWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bSelected)    return;
	if (m_hZoom == NULL) return;

	// 判断鼠标点击点是否在图像内，如不在则不处理
	POINT    ptImage;
	if (KSJIZM_RET_SUCCESS != KSJIZM_MapPointClientToImage(m_hZoom, point.x, point.y, (int*)&ptImage.x, (int*)&ptImage.y))
		return;

	// 如果鼠标移动过程中鼠标的左键被按下，当显示的大小大于窗口的大小时，需要改变鼠标形状，并支持移动
	if( nFlags & MK_LBUTTON )
	{
		SetCursor(::LoadCursor(NULL, IDC_HAND));    // MAKEINTRESOURCE()

		m_ptOffset = (point - m_ptLButtonDownInClientPre);
		m_ptLButtonDownInClientPre = point;

		KSJIZM_Offset(m_hZoom, m_ptOffset.x, m_ptOffset.y);

		if (m_nIndex != -1)
		{
			KSJIZM_GetPosition(m_hZoom, &m_nShowOffsetH, &m_nShowOffsetV, &m_nShowWidth, &m_nShowHeight);
			//KSJ_PreviewSetPos(m_nIndex, m_hWnd, m_nShowOffsetH, m_nShowOffsetV, m_nShowWidth, m_nShowHeight);
		}
		else
		{
			Invalidate();
			UpdateWindow();
		}

		return;
	}

	CWnd::OnMouseMove(nFlags, point);
}



// pt为屏幕坐标
BOOL CKSJImageWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	TCHAR szBuf[256] = { 0 };
	_stprintf_s(szBuf, 256, _T("CKSJImageWnd::OnMouseWheel::pt0( %d,%d )"), pt.x, pt.y);
	// OutputDebugString(szBuf);

	POINT ptClient;
	::ScreenToClient(m_hWnd, &pt);
	_stprintf_s(szBuf, 256, _T("CKSJImageWnd::OnMouseWheel::pt1( %d,%d )"), pt.x, pt.y);
	// OutputDebugString(szBuf);


	if (zDelta > 0)
	{
		KSJIZM_SetZoomMode(m_hZoom, ZM_ZOOM);
		ZoomIn();
	}
	if (zDelta < 0)
	{
		KSJIZM_SetZoomMode(m_hZoom, ZM_ZOOM);
		ZoomOut();
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CKSJImageWnd::ZoomIn( )
{
	if (m_hZoom == NULL)    return;

	POINT ptClient;
    ::GetCursorPos(&ptClient);
	::ScreenToClient(m_hWnd, &ptClient);

	TCHAR szBuf[256] = { 0 };
	_stprintf_s(szBuf, 256, _T("CKSJImageWnd::ZoomIn::ClientPos( %d,%d )"), ptClient.x, ptClient.y);
	OutputDebugString(szBuf);


	int nImageX, nImageY;
	if (KSJIZM_RET_SUCCESS != KSJIZM_MapPointClientToImage(m_hZoom, ptClient.x, ptClient.y, &nImageX, &nImageY))
		return;

	KSJIZM_ZoomIn(m_hZoom, nImageX, nImageY);

	Invalidate();
	UpdateWindow();
}


void  CKSJImageWnd::ZoomOut()
{
	if (m_hZoom == NULL)    return;

	POINT ptClient;
	::GetCursorPos(&ptClient);
	::ScreenToClient(m_hWnd, &ptClient);

	TCHAR szBuf[256] = { 0 };
	_stprintf_s(szBuf, 256, _T("CKSJImageWnd::ZoomOut::ClientPos( %d,%d )\n"), ptClient.x, ptClient.y);
	OutputDebugString(szBuf);


	int nImageX, nImageY;
	if (KSJIZM_RET_SUCCESS != KSJIZM_MapPointClientToImage(m_hZoom, ptClient.x, ptClient.y, &nImageX, &nImageY) )
		return;

	KSJIZM_ZoomOut(m_hZoom, nImageX, nImageY);

    Invalidate();
	UpdateWindow();
}


// 注意中文输入法
BOOL CKSJImageWnd::MyPreTranslateMessage(MSG* pMsg)
{
	if (m_hZoom == NULL)    return FALSE;

	if( pMsg->message == WM_KEYDOWN) 
	{ 
		POINT ptClient;
		::GetCursorPos(&ptClient);
		::ScreenToClient(m_hWnd, &ptClient);
		int nImageX, nImageY;
		switch (pMsg->wParam)
		{ 
		case VK_OEM_PLUS:
		case VK_ADD:  
			KSJIZM_SetZoomMode(m_hZoom, ZM_ZOOM);
			KSJIZM_MapPointClientToImage(m_hZoom, ptClient.x, ptClient.y, &nImageX, &nImageY);
			KSJIZM_ZoomIn(m_hZoom, nImageX, nImageY);
			break;   // '+'

		case VK_OEM_MINUS:  
		case VK_SUBTRACT:   
			KSJIZM_SetZoomMode(m_hZoom, ZM_ZOOM);
			KSJIZM_MapPointClientToImage(m_hZoom, ptClient.x, ptClient.y, &nImageX, &nImageY);
			KSJIZM_ZoomOut(m_hZoom, nImageX, nImageY);
			break;
		case VK_MULTIPLY:   
			KSJIZM_SetZoomMode(m_hZoom, ZM_FITIMG);
			break;
		case VK_OEM_2:
		case VK_DIVIDE:      
			KSJIZM_SetZoomMode(m_hZoom, ZM_ACTUALSIZE);
			break;
		case 0x38: 
			if( GetKeyState(VK_SHIFT) & 0x8000  )
			{
				if (m_hZoom)
					KSJIZM_SetZoomMode(m_hZoom, ZM_FITIMG);
			}
			break; 

		default: 
			break;
		} 
	}

	Invalidate();
	UpdateWindow();

	return TRUE;
}

void CKSJImageWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
#if 0
	CMenu menu;   
	VERIFY(menu.LoadMenu(IDR_MENU_IMAGE));   
	CMenu* pPopup = menu.GetSubMenu(0);   
	ASSERT(pPopup!=NULL);

	// menu.EnableMenuItem( ID_CONVERT_8BBP,   ( m_pImageBmpInfo->bmiHeader.biBitCount == 24     ? MF_ENABLED : MF_DISABLED)  | MF_BYCOMMAND );

	menu.CheckMenuItem( ID_ZOOM_FITIMAGE,   ( m_ZoomMode == ZM_FITIMG     ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND );
	menu.CheckMenuItem( ID_ZOOM_FITWINDOW,  ( m_ZoomMode == ZM_FITWND     ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND );
	menu.CheckMenuItem( ID_ZOOM_ACTUALSIZE, ( m_ZoomMode == ZM_ACTUALSIZE ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND );
	menu.CheckMenuItem( ID_ZOOM_FITIMAGE,   ( m_ZoomMode == ZM_FITIMG     ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND );
	menu.CheckMenuItem( ID_ZOOM_ZOOMIN,     ( m_ZoomMode == ZM_IN         ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND );
	menu.CheckMenuItem( ID_ZOOM_ZOOMOUT,    ( m_ZoomMode == ZM_OUT        ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND );


	menu.CheckMenuItem(ID_MOUSEMODE_NORMAL, (m_KMM == KMM_NORMAL ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
	menu.CheckMenuItem(ID_MOUSEMODE_DRAW,   (m_KMM == KMM_DRAW ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

	int   nCmd   =  pPopup->TrackPopupMenu( TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,point.x,point.y, this); 
	switch( nCmd )
	{
	case ID_ZOOM_FITIMAGE:
		if (m_hZoom)
			KSJIZM_SetZoomMode(m_hZoom, ZM_FITIMG);
		Invalidate();
		UpdateWindow();
		break;
	case ID_ZOOM_FITWINDOW:
		if (m_hZoom)
			KSJIZM_SetZoomMode(m_hZoom, ZM_FITWND);
			Invalidate();
		UpdateWindow();
		break;
	case ID_ZOOM_ACTUALSIZE:
		if (m_hZoom)
			KSJIZM_SetZoomMode(m_hZoom, ZM_ACTUALSIZE);
			Invalidate();
		UpdateWindow();
		break;
	case ID_ZOOM_ZOOMIN:
		if (m_hZoom)
			KSJIZM_SetZoomMode(m_hZoom, ZM_ZOOM);
			Invalidate();
		UpdateWindow();
		break;
	case ID_ZOOM_ZOOMOUT:
		if (m_hZoom)
			KSJIZM_SetZoomMode(m_hZoom, ZM_ZOOM);
		Invalidate();
		UpdateWindow();
		break;
	}
#endif
	return;
}

void CKSJImageWnd::SaveAs()
{
	CString strSaveFilePathAndName;

	CString strFilter = _T("BMP File(*.bmp)|*.bmp|All Files(*.*)|*.*||");
	CFileDialog FileDlg( FALSE, TEXT(""), NULL, NULL, strFilter );

	CString   strExt;
	if( FileDlg.DoModal() == IDOK )
	{
		strSaveFilePathAndName = FileDlg.GetPathName();
		strExt = FileDlg.GetFileExt( );
	}
	else
		return;

	return SaveImage( strSaveFilePathAndName );
}



BOOL CKSJImageWnd::PreTranslateMessage(MSG* pMsg)
{
	return CWnd::PreTranslateMessage(pMsg);
}

void CKSJImageWnd::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();
}

BOOL CKSJImageWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)::GetStockObject(BLACK_BRUSH), 0);
	return CWnd::PreCreateWindow(cs);
}

void CKSJImageWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_hZoom == NULL)    return;

	KSJIZM_SetClientInfo(m_hZoom, cx, cy);

	TCHAR szBuf[256] = { 0 };
	_stprintf_s(szBuf, 256, _T("CKSJImageWnd::OnSize::KSJIZM_SetClientInfo( %d,%d )\n"), cx, cy);
	OutputDebugString(szBuf);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    图像缩放及位置相关函数
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CKSJImageWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// if (m_bZoomVideo)    return;

	CRect    rtClient;
	GetClientRect(&rtClient);

	switch (nSBCode)
	{
	case SB_LEFT:				// Scroll to far left.
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - 10);
		break;
	case SB_LINELEFT:			// Scroll left.
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - 2);
		break;
	case SB_PAGELEFT:			// Scroll one page left.
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - rtClient.Width());		// 左侧空白区
		break;
	case SB_ENDSCROLL:			// End scroll.
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + 1);
		break;
	case SB_RIGHT:				// Scroll to far right.
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + 10);
		break;
	case SB_LINERIGHT:			// Scroll right.
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + 2);
		break;
	case SB_PAGERIGHT:			// Scroll one page right.
		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + rtClient.Width());
		break;
	case SB_THUMBPOSITION:		// Scroll to absolute position. The current position is specified by the nPos parameter.
		SetScrollPos(SB_HORZ, nPos);
		break;
	case SB_THUMBTRACK:
		SetScrollPos(SB_HORZ, nPos);
		break;
	}

	// MoveWinAccordToScrollPos();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CKSJImageWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// if (m_bZoomVideo)    return;

	CRect    rtClient;
	GetClientRect(&rtClient);

	switch (nSBCode)
	{
	case SB_TOP:			//  Scroll to top. 
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - 10);
		break;
	case SB_BOTTOM:		    // Scroll to bottom.
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + 10);
		break;
	case SB_ENDSCROLL:	    // End scroll.
		break;
	case SB_LINEUP:		    // Scroll one line up.
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - 2);
		break;
	case SB_LINEDOWN:	    // Scroll one line down.
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + 2);
		break;
	case SB_PAGEUP:		    // Scroll one page up.
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - rtClient.Height());		// 左侧空白区
		break;
	case SB_PAGEDOWN:	    // Scroll one page down.
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + rtClient.Height());		// 左侧空白区
		break;
	case SB_THUMBPOSITION:	// Scroll to the absolute position. The current position is provided in nPos.
		SetScrollPos(SB_VERT, nPos);
		break;
	case SB_THUMBTRACK:		// Drag scroll box to specified position. The current position is provided in nPos.
		SetScrollPos(SB_VERT, nPos);
		break;
	}

	// MoveWinAccordToScrollPos();
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}




