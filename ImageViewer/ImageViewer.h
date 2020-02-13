
#pragma once
typedef void(CALLBACK * REVIEWDETAILPADCALLBACK)();

class CImageViewer : public CView
{
	DECLARE_DYNCREATE(CImageViewer)
public:
	CImageViewer(void);
	virtual ~CImageViewer(void);
	
public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
	bool Reset();
	bool IsZoomInOut(bool bIsZoom);
	bool Update(CString szImageFilePath);
	void SetReviewPADDetailCallBack(REVIEWDETAILPADCALLBACK pFunc);	

private:
	REVIEWDETAILPADCALLBACK	m_pPADReviewCallBack;
	HBITMAP m_hBitmap;
	bool m_bIsZoomInOut;
	void ClearBackground();
	bool DrawImage(bool bIsZoom);

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

