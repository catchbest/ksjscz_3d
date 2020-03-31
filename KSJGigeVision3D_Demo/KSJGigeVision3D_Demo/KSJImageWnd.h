#if !defined(AFX_KSJWND_H__DD2F42A5_819F_4D99_A3CD_144154F3684A__INCLUDED_)
#define AFX_KSJWND_H__DD2F42A5_819F_4D99_A3CD_144154F3684A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSJWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKSJImageWnd window
// ��Ӧ��귽ʽ
// ��ΪKMM_NORMALʱ�����������²��ƶ����ʱ���ͼ�����λ�õ��϶�
// ��ΪKMM_DRAWʱ��
// �����ͼ����Ϊָ��ͼ�Σ���ʼ�µ�ͼ�λ��Ƽ���ӱ༭�㡣
// ���û��ָ��ͼ�Σ�����갴��ʱ���ͼ�ν���HIT���
// ��갴�²��ƶ�ʱ���HIT��ͼ�ν����ƶ�
// ����ƶ�ʱ����Բ���ͼ�ν���PASS��⣬
// ��갴��û�м�⵽HITʱ���Ὺʼһ����ͼ�εĻ��ơ�
// �����ڻ���ʱ��������ƶ�ʱ��

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
	int           m_nIndex;    // ��m_nIndex��Ϊ-1ʱ����ʾ�˴�����Ϊ�����Ԥ������ʹ�ã�Ҳ����KSJ_PreviewStart����ʾ�ڴ˴�����
	                           // ��=-1ʱ����ʾ��ͨ��LoadImage����ʾһ��ͼƬ�Ĵ��ڡ�

private:
	void          ReadIni(void);
	void          WriteIni(void);

public:
	int           SetImageInfo(int nCaptureWidth, int nCaptureHeight, int nBitCount);
	void          LoadImage(BYTE *pImageData, int nWidth, int nHeight, int nBitCount);	// ����ͼ�����ݼ���ͼ��
	void          LoadImageFromFile(LPCTSTR lpszFileName );								// ��һ��BMP�ļ���������
	BOOL          MyPreTranslateMessage(MSG* pMsg);

private:
	BOOL          m_bOpenFromFile;                                                      // ����ǵ����򿪵�ͼ���ļ�
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
	double			m_dShowZoomH;					// ͼ������سߴ���ͼ����ʾ�ĳߴ��ˮƽ���ű��� = m_nShowWidth  / m_nPreviewWidth;
	double			m_dShowZoomV;					// ͼ������سߴ���ͼ����ʾ�ĳߴ�Ĵ�ֱ���ű��� = m_nShowHeight / m_nPreviewHeight;

	// Ԥ��ͼ����ʾ�ڴ��ڵ�λ��
	int				m_nShowOffsetH;     // Ԥ��ͼ�����Ͻ��൱�ڿͻ������Ͻǵ�ˮƽƫ�ƣ�
	int				m_nShowOffsetV;     // Ԥ��ͼ�����Ͻ��൱�ڿͻ������ϽǵĴ�ֱƫ�ƣ�
	int				m_nShowWidth;       // ��ͼ����ʾ�Ŀ�ȣ�ע�ⲻ��ͼ��Ŀ�ȣ�������ʾ�Ŀ�ȣ�
	int				m_nShowHeight;	    // ��ͼ����ʾ�ĸ߶ȣ�ע�ⲻ��ͼ��ĸ߶ȣ�������ʾ�ĸ߶ȣ�

	// ���������
	CPoint			m_ptOffset;		    // ��������ˮƽ����ֱλ�ã� ���ù�����������갴�º��ƶ�ͼ�� ��
	// int             m_nHScrollPos;   // �Ժ���������ֵ�滻m_ptOffset,�����һЩ
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
