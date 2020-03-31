#if !defined(AFX_KSJWND_H__DD2F42A5_819F_4D99_A3CD_144154F3684A__INCLUDED_)
#define AFX_KSJWND_H__DD2F42A5_819F_4D99_A3CD_144154F3684A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSJWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKSJImageWnd window
// 响应鼠标方式
// 当为KMM_NORMAL时，鼠标左键按下并移动鼠标时会对图像进行位置的拖动
// 当为KMM_DRAW时，
// 如果绘图工具为指定图形，则开始新的图形绘制及添加编辑点。
// 如果没有指定图形，则鼠标按下时会对图形进行HIT检测
// 鼠标按下并移动时会对HIT的图形进行移动
// 鼠标移动时，会对测量图形进行PASS检测，
// 鼠标按下没有检测到HIT时，会开始一个新图形的绘制。
// 当处于绘制时，在鼠标移动时会

class CKSJImageWnd : public CWnd
{
protected:
	DECLARE_DYNCREATE(CKSJImageWnd)

// Construction
public:
	CKSJImageWnd( CWnd *pParent = NULL, int nIndex = 0 );
	virtual ~CKSJImageWnd();
private:
	CWnd    *m_pParent;

public:
	void ConnectCamera(int nIndex);

private:
	int           m_nIndex;    // 当m_nIndex不为-1时，表示此窗口作为相机的预览窗口使用，也就是KSJ_PreviewStart将显示在此窗口上
	                           // 当=-1时，表示是通过LoadImage来显示一张图片的窗口。

private:
	void          ReadIni(void);
	void          WriteIni(void);

public:
	int           SetImageInfo(int nCaptureWidth, int nCaptureHeight, int nBitCount);
	void          LoadImage(BYTE *pImageData, int nWidth, int nHeight, int nBitCount);	// 传入图像数据加载图像
	void          LoadImageFromFile(LPCTSTR lpszFileName );								// 从一个BMP文件加载数据
	BOOL          MyPreTranslateMessage(MSG* pMsg);

private:
	BOOL          m_bOpenFromFile;                                                      // 标记是单幅打开的图像文件
	TCHAR         m_szOpenFileFullName[MAX_PATH];

private:
	void          SaveAs();
	void          SaveImage( LPCTSTR lpszFileName );


private:
	BYTE          *m_pImageBmpData;
	BITMAPINFO    *m_pImageBmpInfo;

public:
	void          SetSelected( BOOL bSelect );
	bool          GetSelected() { return m_bSelected; };
private:
	BOOL          m_bSelected;


private:
	HANDLE          m_hZoom;


public:
	void            ZoomIn();
	void            ZoomOut();

private:
	double			m_dZoomFactor;
	double			m_dShowZoomH;					// 图像的像素尺寸与图像显示的尺寸的水平缩放比例 = m_nShowWidth  / m_nPreviewWidth;
	double			m_dShowZoomV;					// 图像的像素尺寸与图像显示的尺寸的垂直缩放比例 = m_nShowHeight / m_nPreviewHeight;

	// 预览图像显示在窗口的位置
	int				m_nShowOffsetH;     // 预览图像左上角相当于客户区左上角的水平偏移，
	int				m_nShowOffsetV;     // 预览图像左上角相当于客户区左上角的垂直偏移，
	int				m_nShowWidth;       // 是图像显示的宽度（注意不是图像的宽度，而是显示的宽度）
	int				m_nShowHeight;	    // 是图像显示的高度（注意不是图像的高度，而是显示的高度）

	// 虚拟滚动条
	CPoint			m_ptOffset;		    // 滚动条的水平、垂直位置（ 不用滚动条，用鼠标按下后移动图像 ）
	// int             m_nHScrollPos;   // 以后用这两个值替换m_ptOffset,更清楚一些
	// int             m_nVScrollPos;


	CPoint          m_ptLButtonDownInClientPre;
	CPoint          m_ptLButtonDownInImage;


// Attributes
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKSJImageWnd)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CKSJImageWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

private:
	void DrawSelectRect();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSJWND_H__DD2F42A5_819F_4D99_A3CD_144154F3684A__INCLUDED_)
