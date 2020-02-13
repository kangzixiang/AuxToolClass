#include "StdAfx.h"
#include "ImageViewer.h"

IMPLEMENT_DYNCREATE(CImageViewer, CView)

CImageViewer::CImageViewer(void)
{
	m_hBitmap=NULL;
	m_bIsZoomInOut=false;
}


CImageViewer::~CImageViewer(void)
{
	if (m_hBitmap != NULL)
	{
		::DeleteObject(m_hBitmap) ;
		m_hBitmap = NULL;
	}
}

BEGIN_MESSAGE_MAP(CImageViewer, CView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CChildView drawing

void CImageViewer::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	if(m_hBitmap!=NULL)
	{
		DrawImage(m_bIsZoomInOut);
	}
}

#ifdef _DEBUG
void CImageViewer::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CImageViewer::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG

BOOL CImageViewer::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	pDC->FillRect(rc, &CBrush(RGB(0,0,0)));
	return TRUE;
}

bool CImageViewer::Update(CString szImageFilePath)
{
	if ( m_hBitmap != NULL )//clear old image
    {
        DeleteObject(m_hBitmap);
		m_hBitmap=NULL;
    }

	CFileFind filefind;
	BOOL IsFileFind = filefind.FindFile(szImageFilePath);
	if(IsFileFind)//Load image
	{
		m_hBitmap=(HBITMAP)::LoadImage(
			NULL,
			szImageFilePath,
			IMAGE_BITMAP,
			0,0,
			LR_LOADFROMFILE|LR_CREATEDIBSECTION);

		if(m_hBitmap!=NULL)
		{
			return DrawImage(m_bIsZoomInOut);
		}
	}
	else//The image doesn’t exist, will fill with black background without message notice.
	{
		ClearBackground();
		return false;
	}

	return false;
}


bool CImageViewer::Reset()
{
	try
	{
		if(m_hBitmap!= NULL)
		{
			::DeleteObject(m_hBitmap);
			m_hBitmap=NULL;
		}	
		ClearBackground();
	}
	catch(COleException e)
	{
		return false;
	}

	return true;
}


bool CImageViewer::IsZoomInOut(bool bIsZoom)
{
	m_bIsZoomInOut=bIsZoom;
	return DrawImage(m_bIsZoomInOut);
	
}
	
bool CImageViewer::DrawImage(bool bIsZoom)
{
	ClearBackground();

	if(m_hBitmap!=NULL)
	{
		BITMAP bm;
		::GetObject( m_hBitmap, sizeof( bm ), &bm ); 
		CRect rc;
		this->GetClientRect(&rc);
		
		int lefPost=0, topPos=0, bmWidth=0, bmHeight=0;
		if(m_bIsZoomInOut)//m_bIsZoomInOut==false,zoom in/out image to fill this view.
		{
			double dScaleValueWidth=(double)rc.Width()/(double)bm.bmWidth;
			double dScaleValueHeight=(double)rc.Height()/(double)bm.bmHeight;
			if(dScaleValueWidth>=dScaleValueHeight)//ZoomInOut by height
			{
				bmWidth =(int)(dScaleValueHeight*bm.bmWidth);
				bmHeight =rc.Height();
				lefPost=(rc.Width()/2-bmWidth/2);
				topPos=0;
			}
			if(dScaleValueWidth<dScaleValueHeight)//ZoomInOut by width
			{
				bmWidth =rc.Width();
				bmHeight =(int)(dScaleValueWidth*bm.bmHeight);
				lefPost=0;
				topPos=rc.Height()/2-bmHeight/2;
			}
		}
		else//m_bIsZoomInOut==false,display original image size.
		{
			bmWidth = bm.bmWidth; 
			bmHeight = bm.bmHeight; 
			lefPost=rc.Width()/2-bmWidth/2;
			topPos=rc.Height()/2- bmHeight/2;
		}
		
		// StretchBlt bitmap onto client area which this pointed to.
		CDC *pDC=(CDC*)this->GetDC();
		if(pDC!=NULL)
		{
			CDC MemDC;
			if(!MemDC.CreateCompatibleDC(pDC))
				return false;
			HBITMAP* pBmpOld = (HBITMAP*) ::SelectObject(MemDC.m_hDC, m_hBitmap);

			pDC->StretchBlt(lefPost,topPos,bmWidth,bmHeight,&MemDC, 0, 0, bm.bmWidth-1, bm.bmHeight-1,SRCCOPY);	
			::SelectObject(MemDC.m_hDC, pBmpOld);
		}
		
		if(!this->ReleaseDC(pDC) )    
		{
			AfxMessageBox("1 error code:%d\n",GetLastError());    
		} 
	}

	return true;
}

void CImageViewer::SetReviewPADDetailCallBack(REVIEWDETAILPADCALLBACK pFunc)
{
	m_pPADReviewCallBack = pFunc;
}

void CImageViewer::ClearBackground()
{
	CRect rc;
	this->GetClientRect(&rc);
	CBrush newbrush;
	newbrush.CreateSolidBrush(RGB(0,0,0));
	CClientDC dc(this);
	dc.FillRect(&rc,&newbrush);	
	newbrush.DeleteObject();
}

void CImageViewer::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_pPADReviewCallBack();
	CView::OnLButtonDblClk(nFlags, point);
}
