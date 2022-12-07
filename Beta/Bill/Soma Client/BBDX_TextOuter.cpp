#include "stdafx.h"
#include "Soma.h"
#include "DManage/BB_Direct.h"
#include "BBDX_TextOuter.h"
#include "TWFont.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//#define DEFAULT_SPLIT_WIDTH	640
#define DEFAULT_SPLIT_WIDTH	800
#define DEFAULT_MAX_LINE	100 //만약 DEFAULT_MAX_LINE보다 라인 수가 넘을 경우는 위에서 지워나감

BBDX_TextOuter::BBDX_TextOuter(CString fontname, int x, int y)
{
	m_iWidth = x;
	m_iHeight = y;

	m_pBitmapFont = new CTWFont;
	if (!m_pBitmapFont->CreateFontWithoutPath(fontname, m_iWidth, m_iHeight))
	{
		delete m_pBitmapFont;
		m_pBitmapFont = NULL;
	}

	m_bShouldDelete = TRUE;
	m_iSplitWidth = DEFAULT_SPLIT_WIDTH;
	m_iMaxLineNum = DEFAULT_MAX_LINE;
}

BBDX_TextOuter::BBDX_TextOuter(CTWFont *pFont, int x, int y)
{
	m_bShouldDelete = FALSE;
	m_pBitmapFont = pFont;
	m_iWidth = x;
	m_iHeight = y;
	m_iSplitWidth = DEFAULT_SPLIT_WIDTH;
	m_iMaxLineNum = DEFAULT_MAX_LINE;
}

BBDX_TextOuter::~BBDX_TextOuter()
{
	if (m_bShouldDelete&&m_pBitmapFont) delete m_pBitmapFont;
	m_strList.RemoveAll();
}

void BBDX_TextOuter::SetString(CString& str)
{
	m_strList.RemoveAll();
	AddString(str);
}

int BBDX_TextOuter::AddString(CString &str)
{
	if (!m_pBitmapFont) return 0;
	CString Msg = str;
	int str_len = str.GetLength()+10;
	char *first = new char[str_len];
	char *second = new char[str_len];
	ZeroMemory(first, str_len);
	ZeroMemory(second, str_len);

	int added_linecount = 0;
	while (m_pBitmapFont->GetLineWidth(Msg)>m_iSplitWidth)
	{
		m_pBitmapFont->SplitString(Msg, first, second, m_iSplitWidth);

		m_strList.AddTail(first);
		Msg = second;
		added_linecount++;
	}
	m_strList.AddTail(Msg);
	added_linecount++;

	if (m_strList.GetCount()>m_iMaxLineNum)
		while (m_strList.GetCount()>m_iMaxLineNum) 
		{
			m_strList.RemoveHead();
//			added_linecount--;
		}

	delete[] first;
	delete[] second;
	return added_linecount;
}

void BBDX_TextOuter::ReArrange()
{
	CString totalString;
	int count = m_strList.GetCount();

	POSITION pos = m_strList.GetHeadPosition();
	for (int i=0; i<count && pos; i++)
	{
		totalString += m_strList.GetNext(pos);
	}
	m_strList.RemoveAll();
	AddString(totalString);
}

//===============================
//우석.
void BBDX_TextOuter::DrawMultiLineTexts(CDDSurface *dest, CRect r, WORD color, int lineGap, bool bInvert)
{

	if (!m_pBitmapFont) return;

	r.NormalizeRect();

	int count = m_strList.GetCount();
	POSITION pos = m_strList.GetHeadPosition();
	CString str = m_strList.GetNext(pos);
	
	DDSURFACEDESC2 ddsd;
	dest->GetDescription(&ddsd);
	ddsd.lpSurface = dest->Lock();

	if(bInvert) m_pBitmapFont->IC_TextOutClip(r, str, color, ddsd, 0, str.GetLength(), -1, 0); 
	else        m_pBitmapFont->IC_TextOutClip(r, str, color, ddsd, 0, 0, -1, 0); 

	dest->Unlock();
}
//==========================

void BBDX_TextOuter::DrawTexts(CDDSurface *dest, CRect r, WORD color, int lineGap)
{
//	if (m_iSplitWidth!=r.Width()) TRACE("BBDX_TextOuter's split width is not equal drawing rect's width\n");

	if (!m_pBitmapFont) return;

	r.NormalizeRect();

	int count = m_strList.GetCount();
	POSITION pos = m_strList.GetHeadPosition();
	CString string;
	
	DDSURFACEDESC2 ddsd;
	dest->GetDescription(&ddsd);
	ddsd.lpSurface = dest->Lock();

	CRect curRect;
	CRect tmpR;

	for (int i=0; i<count&&pos; i++)
	{
		curRect.SetRect(r.left, r.top+i*(m_iHeight+lineGap), r.right, 0);
		curRect.bottom = curRect.top+m_iHeight;

		curRect.NormalizeRect();
		tmpR = curRect;
		tmpR &= r;
		if (tmpR!=curRect) break;
//		if (tmpR.IsRectEmpty()) break;

		m_pBitmapFont->IC_TextOutClip(curRect, m_strList.GetNext(pos), color, ddsd, 0, 0, -1, 0); 
	}

	dest->Unlock();
}

void BBDX_TextOuter::DrawShadowTexts(CDDSurface *dest, CRect r, WORD color, int lineGap)
{
//	if (m_iSplitWidth!=r.Width()) TRACE("BBDX_TextOuter's split width is not equal drawing rect's width\n");

	if (!m_pBitmapFont) return;

	r.NormalizeRect();

	int count = m_strList.GetCount();
	POSITION pos = m_strList.GetHeadPosition();
	CString string;
	
	DDSURFACEDESC2 ddsd;
	dest->GetDescription(&ddsd);
	ddsd.lpSurface = dest->Lock();

	CRect curRect;
	CRect tmpR;

	for (int i=0; i<count&&pos; i++)
	{
		curRect.SetRect(r.left, r.top+i*(m_iHeight+lineGap), r.right, 0);
		curRect.bottom = curRect.top+m_iHeight;

		curRect.NormalizeRect();
		tmpR = curRect;
		tmpR &= r;
		if (tmpR!=curRect) break;
//		if (tmpR.IsRectEmpty()) break;

		m_pBitmapFont->IC_TextOutClipShadow(curRect, m_strList.GetNext(pos), color, ddsd, 0, 0, -1, 0);
	}

	dest->Unlock();
}

void BBDX_TextOuter::DrawTextsFromIndexedLine(CDDSurface *dest, CRect r, int from, WORD color, int lineGap)
{
	if (m_iSplitWidth!=r.Width()) TRACE("BBDX_TextOuter's split width is not equal drawing rect's width\n");

	if (!m_pBitmapFont) return;

	r.NormalizeRect();

	int count = m_strList.GetCount();
	POSITION pos = m_strList.FindIndex(from);
	
	DDSURFACEDESC2 ddsd;
	dest->GetDescription(&ddsd);
	ddsd.lpSurface = dest->Lock();

	CRect curRect;
	CRect tmpR;

	for (int i=from; i<count&&pos; i++)
	{
		curRect.SetRect(r.left, r.top+(i-from)*(m_iHeight+lineGap), r.right, 0);
		curRect.bottom = curRect.top+m_iHeight+lineGap;

		curRect.NormalizeRect();
		tmpR = curRect;
		tmpR &= r;
		if (tmpR!=curRect) break;
//		if (tmpR.IsRectEmpty()) break;

		m_pBitmapFont->IC_TextOutClip(curRect, m_strList.GetNext(pos), color, ddsd, 0, 0, -1, lineGap);
	}

	dest->Unlock();
}

//======================================================================
//우석.
//======================================================================
void BBDX_TextOuter::SetMultiLineString(CString &str)
{
	m_strList.RemoveAll();
	if (m_pBitmapFont) m_strList.AddTail(str);
}