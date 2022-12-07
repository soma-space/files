// TWFont.cpp : implementation file
//

#include "stdafx.h"
#include "Soma.h"
#include "TWFont.h"
#include "DManage/BB_Direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTWFont class

CTWFont::CTWFont ()
{
	m_pBuf = NULL;

	CDC dc;
	dc.CreateIC("Display", NULL, NULL, NULL);
	int bitsPixel = dc.GetDeviceCaps(BITSPIXEL);
	m_bIs256 = (bitsPixel==8) ? TRUE:FALSE;

	m_PutCharCount = 0;//快籍.
}

CTWFont::~CTWFont ()
{
	m_font.DeleteObject ();

	if (m_pBuf)
		free (m_pBuf);
}

void CTWFont::TextOut (CPoint pt, LPCTSTR lpszText, COLORREF bColor, 
					   CDC& dc, UINT uFlag)
{
	CFont* pFont = (CFont*)dc.SelectObject (&m_font);
	dc.SetTextColor (bColor);
	dc.SetTextAlign (uFlag);
	dc.TextOut (pt.x, pt.y, lpszText, strlen (lpszText));
	dc.SelectObject (pFont);
}

int CTWFont::TextOutClip (CRect rc, LPCTSTR lpszText, COLORREF bColor, 
						  CDC& dc, int nLine, BOOL fWordBreak)
{
	CFont* pFont = (CFont*)dc.SelectObject (&m_font);
	dc.SetTextColor (bColor);

	UINT uFormat = DT_LEFT | (fWordBreak ? DT_WORDBREAK : 0) |
		((nLine > 0) ? DT_EXTERNALLEADING : 0);
	int nHeight = dc.DrawText (lpszText, -1, rc, uFormat);
	dc.SelectObject (pFont);
	return nHeight;
}

BOOL CTWFont::CreateFont (CString strFontName, int nSizeX, int nSizeY)
{
	m_nX = nSizeX;
	m_nY = nSizeY;
//	m_nCharLen = nSizeY * nSizeX / 8;
	m_nCharLen = 32;
	
	CFile file;
	if (!file.Open (strFontName + _T(".mxf"), CFile::modeRead))
		return FALSE;

	DWORD dwFileLen = file.GetLength ();
	m_pBuf = malloc (dwFileLen);
	if (m_pBuf == NULL || dwFileLen != (DWORD)m_nCharLen * 2592 ||
		dwFileLen != file.Read (m_pBuf, dwFileLen))
	{
		free(m_pBuf);
		m_pBuf = NULL;
		file.Close ();
		return FALSE;
	}

	m_pBitsAsc = (BYTE*)m_pBuf;
	m_pBitsHCode1 = m_pBitsAsc + m_nCharLen * 16 * 6;
	m_pBitsHCode2 = m_pBitsHCode1 + m_nCharLen * 16 * 6;
	file.Close ();
	return TRUE;
}

BOOL CTWFont::ChangeFont (CString strFontName, int nSizeX, int nSizeY)
{
	CFile file;
	if (!file.Open (strFontName + _T(".mxf"), CFile::modeRead))
		return FALSE;

	DWORD dwFileLen = file.GetLength ();
	void* pBuf = malloc (dwFileLen);
	if (pBuf == NULL || dwFileLen != (DWORD)m_nCharLen * 2592 ||
		dwFileLen != file.Read (pBuf, dwFileLen))
	{
		free(pBuf);
		file.Close ();
		return FALSE;
	}

	if (m_pBuf) free(m_pBuf);
	m_pBuf = pBuf;
	m_nX = nSizeX;
	m_nY = nSizeY;

	m_pBitsAsc = (BYTE*)m_pBuf;
	m_pBitsHCode1 = m_pBitsAsc + m_nCharLen * 16 * 6;
	m_pBitsHCode2 = m_pBitsHCode1 + m_nCharLen * 16 * 6;
	file.Close ();
	return TRUE;
}

BOOL CTWFont::CreateFontWithoutPath(CString strFontName, int nSizeX, int nSizeY)
{
	CString exePath = CDirectDraw::GetExecutionPath(TRUE);
	CString strFontNameWithPath;
	strFontNameWithPath = exePath;
	strFontNameWithPath += strFontName;

	return CreateFont(strFontNameWithPath, nSizeX, nSizeY);
}
BOOL CTWFont::ChangeFontWithoutPath(CString strFontName, int nSizeX, int nSizeY)
{
	CString exePath = CDirectDraw::GetExecutionPath(TRUE);
	CString strFontNameWithPath;
	strFontNameWithPath = exePath;
	strFontNameWithPath += strFontName;

	return ChangeFont(strFontNameWithPath, nSizeX, nSizeY);
}


BOOL CTWFont::CreateFont (CString strFontName, int nSize)
{
	return m_font.CreatePointFont (nSize, strFontName);
}

void CTWFont::TextOut (CPoint pt, LPCTSTR lpszText, BYTE bColor, 
					   DDSURFACEDESC2& ddsd, UINT uFlag/*=TA_LEFT*/)
{
	CPoint ptPos = pt;
	int nWidth;
	switch (uFlag)
	{
	case TA_CENTER :
		nWidth = GetLineWidth (lpszText);
		ptPos.x -= nWidth / 2;
		break;
	case TA_RIGHT :
		nWidth = GetLineWidth (lpszText);
		ptPos.x -= nWidth;
		break;
	}

	BYTE* psz = (BYTE*)lpszText;
	while (*psz)
	{
		if (*psz == '\n' || *psz == '\r')
		{
			ptPos.y += m_nY - 2;
			ptPos.x = pt.x;

			switch (uFlag)
			{
			case TA_CENTER :
				nWidth = GetLineWidth ((LPCTSTR)psz);
				ptPos.x -= nWidth / 2;
				break;
			case TA_RIGHT :
				nWidth = GetLineWidth ((LPCTSTR)psz);
				ptPos.x -= nWidth;
				break;
			}
		}
		else
		{
			if (*psz <= 0x7F)
			{
				if (0x20 == *psz)
					ptPos.x += m_nX * 3 / 8;
				else if (0x20 < *psz)
				{
					BYTE* pBuf = m_pBitsAsc + ((int)*psz - 0x20) * m_nCharLen;
					DrawChar (pBuf, ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
				else
				{
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawChar (pBuf, ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
			}
			else
			{
				if (*psz == 0xA4)
				{
					psz++;
					if (*psz >= 0xA0)
					{
						BYTE* pBuf = m_pBitsHCode1 + ((int)*psz - 0xA0) * m_nCharLen;
						DrawChar (pBuf, ptPos, bColor, ddsd);
						ptPos.x += m_nX;
					}
					else
					{
						BYTE temp = '?'; 
						BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
						DrawChar (pBuf, ptPos, bColor, ddsd);
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else if (0xB0 <= *psz && *psz <= 0xC8)
				{
					int nOff = ((int)*psz - 0xB0) * 0x60;
					psz++;
					if (*psz >= 0xA0)
					{
						BYTE* pBuf = m_pBitsHCode2 + ((int)*psz - 0xA0 + nOff) * m_nCharLen;
						DrawChar (pBuf, ptPos, bColor, ddsd);
						ptPos.x += m_nX;
					}
					else
					{
						BYTE temp = '?'; 
						BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
						DrawChar (pBuf, ptPos, bColor, ddsd);
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else
				{
					psz++;
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawChar (pBuf, ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
			}
		}
		psz++;
	}
}

void CTWFont::TextOutHi (CPoint pt, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, UINT uFlag)
{
	CPoint ptPos = pt;
	int nWidth;
	switch (uFlag)
	{
	case TA_CENTER :
		nWidth = GetLineWidth (lpszText);
		ptPos.x -= nWidth / 2;
		break;
	case TA_RIGHT :
		nWidth = GetLineWidth (lpszText);
		ptPos.x -= nWidth;
		break;
	}

	BYTE* psz = (BYTE*)lpszText;
	while (*psz)
	{
		if (*psz == '\n' || *psz == '\r')
		{
			ptPos.y += m_nY - 2;
			ptPos.x = pt.x;
			psz++;

			switch (uFlag)
			{
			case TA_CENTER :
				nWidth = GetLineWidth ((LPCTSTR)psz);
				ptPos.x -= nWidth / 2;
				break;
			case TA_RIGHT :
				nWidth = GetLineWidth ((LPCTSTR)psz);
				ptPos.x -= nWidth;
				break;
			}
			psz--;
		}
		else
		{
			if (*psz <= 0x7F)
			{
				if (0x20 == *psz)
					ptPos.x += m_nX * 3 / 8;
				else if (0x20 < *psz)
				{
					BYTE* pBuf = m_pBitsAsc + ((int)*psz - 0x20) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
				else
				{
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
			}
			else
			{
				if (*psz == 0xA4)
				{
					psz++;
					if (*psz >= 0xA0)
					{
						BYTE* pBuf = m_pBitsHCode1 + ((int)*psz - 0xA0) * m_nCharLen;
						DrawCharHi (pBuf, ptPos, bColor, ddsd);
						ptPos.x += m_nX;
					}
					else
					{
						BYTE temp = '?'; 
						BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
						DrawCharHi (pBuf, ptPos, bColor, ddsd);
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else if (0xB0 <= *psz && *psz <= 0xC8)
				{
					int nOff = ((int)*psz - 0xB0) * 0x60;
					psz++;
					if (*psz >= 0xA0)
					{
						BYTE* pBuf = m_pBitsHCode2 + ((int)*psz - 0xA0 + nOff) * m_nCharLen;
						DrawCharHi (pBuf, ptPos, bColor, ddsd);
						ptPos.x += m_nX;
					}
					else
					{
						BYTE temp = '?'; 
						BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
						DrawCharHi (pBuf, ptPos, bColor, ddsd);
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else
				{
					psz++;
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
			}
		}
		psz++;
	}
}


int CTWFont::TextOutClip (CRect rc, LPCTSTR lpszText, BYTE bColor, 
						   DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine, BOOL fWordBreak)
{
	int nTop = rc.top;
	CPoint ptPos = rc.TopLeft ();
	BYTE* psz = (BYTE*)lpszText;
	int curCharOffset = 0;
	BOOL makeBlock = FALSE;
	BOOL caretDraw = FALSE;
	BOOL lastLineFeed=FALSE;
	while (*psz && ptPos.y + m_nY + nLine <= rc.bottom)
	{
		makeBlock = ((blockStart!=blockEnd)&&(curCharOffset>=blockStart&&curCharOffset<blockEnd))? TRUE:FALSE;
		caretDraw = (caretPos==curCharOffset) ? TRUE: FALSE;
//		if (*psz == '\n' || *psz == '\r')
		if (*psz == '\r') {if (caretDraw) CaretDraw(ptPos, bColor, ddsd); psz++; curCharOffset++; continue;}
		else if (*psz == '\n')
		{
			if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
			ptPos.y += m_nY + nLine;
			ptPos.x = rc.left;
		}
		else
		{
			lastLineFeed = FALSE;
			if (*psz <= 0x7F)
			{
				if (0x20 == *psz || 0x09 == *psz)
				{
					BYTE* pBuf = m_pBitsAsc;
					DrawChar(pBuf, ptPos, bColor, ddsd, makeBlock);
					if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
					ptPos.x += m_nX * 3 / 8;
					if (fWordBreak && 
						ptPos.x + GetWordWidth ((LPCTSTR)psz + 1) >= rc.right)
					{
						ptPos.y += m_nY + nLine;
						ptPos.x = rc.left;
					}
				}
				else if (0x20 < *psz)
				{
					BYTE* pBuf = m_pBitsAsc + ((int)*psz - 0x20) * m_nCharLen;
					DrawChar (pBuf, ptPos, bColor, ddsd, makeBlock);
					if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
				else
				{
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawChar (pBuf, ptPos, bColor, ddsd, makeBlock);
					if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
			}
			else
			{
				if (*psz == 0xA4)
				{
					psz++;
					if (*psz >= 0xA0)
					{
						BYTE* pBuf = m_pBitsHCode1 + ((int)*psz - 0xA0) * m_nCharLen;
						DrawChar (pBuf, ptPos, bColor, ddsd, makeBlock);
						if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
						ptPos.x += m_nX;
						curCharOffset++;
					}
					else
					{
						BYTE temp = '?'; 
						BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
						DrawChar (pBuf, ptPos, bColor, ddsd, makeBlock);
						if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else if (0xB0 <= *psz && *psz <= 0xC8)
				{
					int nOff = ((int)*psz - 0xB0) * 0x60;
					psz++;
					if (*psz >= 0xA0)
					{
						BYTE* pBuf = m_pBitsHCode2 + ((int)*psz - 0xA0 + nOff) * m_nCharLen;
						DrawChar (pBuf, ptPos, bColor, ddsd, makeBlock);
						if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
						ptPos.x += m_nX;
						curCharOffset++;
					}
					else
					{
						BYTE temp = '?'; 
						BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
						DrawChar (pBuf, ptPos, bColor, ddsd, makeBlock);
						if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else
				{
					psz++;
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawChar (pBuf, ptPos, bColor, ddsd, makeBlock);
					if (caretDraw) CaretDraw(ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
			}
		}
		psz++;
		curCharOffset++;
		if (ptPos.x >= rc.right)
		{
			ptPos.y += m_nY + nLine;
			ptPos.x = rc.left;
			lastLineFeed = TRUE;
		}
	}
	if (caretPos==curCharOffset) 
	{
		if (lastLineFeed) 
		{
			ptPos.y -= m_nY+nLine;
			ptPos.x = rc.right;
		}
		CaretDraw(ptPos, bColor, ddsd);
	}
	return ptPos.y + m_nY + nLine - nTop;
}

int CTWFont::TextOutClipHi (CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine, BOOL fWordBreak)
{
	int nTop = rc.top;
	CPoint ptPos = rc.TopLeft ();
	BYTE* psz = (BYTE*)lpszText;
	int curCharOffset = 0;
	BOOL makeBlock = FALSE;
	BOOL caretDraw = FALSE;
	BOOL lastLineFeed = FALSE;

	m_PutCharCount = 0; //快籍.

	while (*psz && ptPos.y + m_nY/* + nLine*/ <= rc.bottom)
	{
		m_PutCharCount++; //快籍..

		makeBlock = ((blockStart!=blockEnd)&&(curCharOffset>=blockStart&&curCharOffset<blockEnd))? TRUE:FALSE;
		caretDraw = (caretPos==curCharOffset) ? TRUE: FALSE;
//		if (*psz == '\n' || *psz == '\r')

		if (*psz == '\r')
		{
			if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
			psz++;
			curCharOffset++; 
			continue;
		}
		else if (*psz == '\n')
		{
			if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
			ptPos.y += m_nY + nLine;
			ptPos.x = rc.left;
		}
		else
		{
			lastLineFeed = FALSE;
			if (*psz <= 0x7F)
			{
				if (0x20 == *psz || 0x09 == *psz)
				{
					BYTE* pBuf = m_pBitsAsc;
					DrawCharHi(pBuf, ptPos, bColor, ddsd, makeBlock);
					if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
					ptPos.x += m_nX * 3 / 8;
					if (fWordBreak && 
						ptPos.x + GetWordWidth ((LPCTSTR)psz + 1) >= rc.right)
					{
						ptPos.y += m_nY + nLine;
						ptPos.x = rc.left;
					}
				}
				else if (0x20 < *psz)
				{
					BYTE* pBuf = m_pBitsAsc + ((int)*psz - 0x20) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd, makeBlock);
					if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
				else
				{
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd, makeBlock);
					if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
			}
			else
			{
				if (*psz == 0xA4)
				{
					psz++;
					m_PutCharCount++; //快籍..

					if (*psz >= 0xA0)
					{
						BYTE* pBuf = m_pBitsHCode1 + ((int)*psz - 0xA0) * m_nCharLen;
						DrawCharHi (pBuf, ptPos, bColor, ddsd, makeBlock);
						if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
						ptPos.x += m_nX;
						curCharOffset++;
					}
					else
					{
						BYTE temp = '?'; 
						BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
						DrawCharHi (pBuf, ptPos, bColor, ddsd, makeBlock);
						if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else if (0xB0 <= *psz && *psz <= 0xC8)
				{
					int nOff = ((int)*psz - 0xB0) * 0x60;
					psz++;
					m_PutCharCount++; //快籍..

					if (*psz >= 0xA0)
					{
						BYTE* pBuf = m_pBitsHCode2 + ((int)*psz - 0xA0 + nOff) * m_nCharLen;
						DrawCharHi (pBuf, ptPos, bColor, ddsd, makeBlock);
						if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
						ptPos.x += m_nX;
						curCharOffset++;
					}
					else
					{
						BYTE temp = '?'; 
						BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
						DrawCharHi (pBuf, ptPos, bColor, ddsd, makeBlock);
						if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else
				{
					psz++;
					m_PutCharCount++; //快籍..

					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd, makeBlock);
					if (caretDraw) CaretDrawHi(ptPos, bColor, ddsd);
					ptPos.x += m_nX * 4 / 8;
				}
			}
		}
		psz++;
		curCharOffset ++;

		if (ptPos.x >= rc.right)
		{
			ptPos.y += m_nY + nLine;
			ptPos.x = rc.left;

			lastLineFeed = TRUE;
		}
		else if( ptPos.x + m_nX >= rc.right) //快籍......
		{
			ptPos.y += m_nY + nLine;
			ptPos.x = rc.left;

			lastLineFeed = TRUE;
		}

	}
	if (caretPos==curCharOffset) 
	{
		if (lastLineFeed) 
		{
			ptPos.y -= m_nY+nLine;
			ptPos.x = rc.right;
		}
		CaretDrawHi(ptPos, bColor, ddsd);
	}
	return ptPos.y + m_nY + nLine - nTop;
}

void CTWFont::CaretDraw(CPoint pt, BYTE bColor, DDSURFACEDESC2& ddsd)
{
	BYTE* pSurfaceBits = (BYTE*)ddsd.lpSurface + pt.x + pt.y * ddsd.lPitch;
	BYTE CaretColor = 0xFF;
	for (int y = 0; y < m_nY; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			pSurfaceBits[x] = CaretColor;
		}
		pSurfaceBits += ddsd.lPitch;
	}
}

void CTWFont::CaretDrawHi(CPoint pt, WORD bColor, DDSURFACEDESC2& ddsd)
{
	BYTE* pSurfaceBits = (BYTE*)ddsd.lpSurface + pt.x*2 + pt.y * ddsd.lPitch;
	WORD CaretColor = 0xFFFF;//~bColor;
	for (int y = 0; y < m_nY; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			pSurfaceBits[x*2] = CaretColor&0x00FF;
			pSurfaceBits[x*2+1] = CaretColor>>8;
		}
		pSurfaceBits += ddsd.lPitch;
	}
}

void CTWFont::DrawChar (BYTE* pBuf, CPoint pt, BYTE bColor, DDSURFACEDESC2& ddsd, BOOL makeBlock)
{
	CRect compRect(pt, CSize(m_nX, m_nY));
	CRect surfaceRect(0,0,ddsd.dwWidth, ddsd.dwHeight);
	surfaceRect &= compRect;
	if (compRect!=surfaceRect) return;

	static BYTE bMask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
	BYTE* pSurfaceBits = (BYTE*)ddsd.lpSurface + pt.x + pt.y * ddsd.lPitch;
	BYTE xorColor = (~bColor) ^ 0xbb;
	for (int y = 0; y < m_nY; y++)
	{
		int nOff = 0;
		for (int x = 0; x < m_nX; x++)
		{
			if (!(*pBuf & bMask[nOff++]))
				pSurfaceBits[x] = bColor;
			else if (makeBlock) pSurfaceBits[x] = xorColor;

			if (nOff == 8||x==m_nX-1)
			{
				nOff = 0;
				pBuf++;
			}
		}
		pSurfaceBits += ddsd.lPitch;
	}
}

void CTWFont::DrawCharHi(BYTE* pBuf, CPoint pt, WORD bColor, DDSURFACEDESC2& ddsd, BOOL bMakeBlock)
{
	CRect compRect(pt, CSize(m_nX, m_nY));
	CRect surfaceRect(0,0,ddsd.dwWidth, ddsd.dwHeight);
	surfaceRect &= compRect;
	if (compRect!=surfaceRect) return;

	static BYTE bMask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
	BYTE* pSurfaceBits = (BYTE*)ddsd.lpSurface + pt.x*2 + pt.y * ddsd.lPitch;
	WORD xorColor = (~bColor) ^ 0xbbbb;
	for (int y = 0; y < m_nY; y++)
	{
		int nOff = 0;
		for (int x = 0; x < m_nX; x++)
		{
			if (!(*pBuf & bMask[nOff++]))
			{
				pSurfaceBits[x*2] = bColor&0x00FF;
				pSurfaceBits[x*2+1] = bColor>>8;
			}
			else if (bMakeBlock)
			{
				pSurfaceBits[x*2] = xorColor&0x00FF;
				pSurfaceBits[x*2+1] = xorColor>>8;
			}
			if (nOff == 8||x==m_nX-1)
			{
				nOff = 0;
				pBuf++;
			}
		}
		pSurfaceBits += ddsd.lPitch;
	}
}

int CTWFont::GetLineWidth (LPCTSTR lpszText)
{
	CPoint ptPos(0,0);
	int nLine = 0;
	BYTE* psz = (BYTE*)lpszText;
	BYTE* prev_psz = psz;
	while (*psz)
	{
		if (*psz == '\n' || *psz == '\r')
		{
			ptPos.y += m_nY + nLine;
			ptPos.x = 0;
		}
		else
		{
			if (*psz <= 0x7F)
			{
				if (0x20 == *psz)
				{
					ptPos.x += m_nX * 3 / 8;
				}
				else if (0x20 < *psz)
				{
					ptPos.x += m_nX * 4 / 8;
				}
				else
				{
					ptPos.x += m_nX * 4 / 8;
				}
			}
			else
			{
				if (*psz == 0xA4)
				{
					psz++;
					if (*psz >= 0xA0)
					{
						ptPos.x += m_nX;
					}
					else
					{
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else if (0xB0 <= *psz && *psz <= 0xC8)
				{
					int nOff = ((int)*psz - 0xB0) * 0x60;
					psz++;
					if (*psz >= 0xA0)
					{
						ptPos.x += m_nX;
					}
					else
					{
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else
				{
					psz++;
					ptPos.x += m_nX * 4 / 8;
				}
			}
		}
		psz++;
	}
	return ptPos.x;
}

int CTWFont::GetWordWidth (LPCTSTR lpszText)
{
	int nWidth = 0;
	BYTE* psz = (BYTE*)lpszText;
	while (*psz && *psz != '\n' && *psz != '\r' && *psz != ' ')
	{
		if (*psz <= 0x7F)
			nWidth += m_nX * 4 / 8;
		else
		{
			if (*psz == 0xA4)
			{
				psz++;
				if (*psz >= 0xA0)
					nWidth += m_nX;
				else
					nWidth += m_nX * 4 / 8;
			}
			else if (0xB0 <= *psz && *psz <= 0xC8)
			{
				int nOff = ((int)*psz - 0xB0) * 0x60;
				psz++;
				if (*psz >= 0xA0)
					nWidth += m_nX;
				else
					nWidth += m_nX * 4 / 8;
			}
			else
				nWidth += m_nX * 4 / 8;
		}
		psz++;
	}

	return nWidth;
}

void CTWFont::SplitString(LPCTSTR lpszText, char *first, char *second, int width)
{
	CPoint ptPos(0,0);
	int nLine = 0;
	BYTE* psz = (BYTE*)lpszText;
	BYTE* prev_psz = psz;
	while (*psz)
	{
		if (*psz == '\n' || *psz == '\r')
		{
			ptPos.y += m_nY + nLine;
			ptPos.x = 0;
		}
		else
		{
			if (*psz <= 0x7F)
			{
				if (0x20 == *psz)
				{
					ptPos.x += m_nX * 3 / 8;
				}
				else if (0x20 < *psz)
				{
					ptPos.x += m_nX * 4 / 8;
				}
				else
				{
					ptPos.x += m_nX * 4 / 8;
				}
			}
			else
			{
				if (*psz == 0xA4)
				{
					psz++;
					if (*psz >= 0xA0)
					{
						ptPos.x += m_nX;
					}
					else
					{
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else if (0xB0 <= *psz && *psz <= 0xC8)
				{
					int nOff = ((int)*psz - 0xB0) * 0x60;
					psz++;
					if (*psz >= 0xA0)
					{
						ptPos.x += m_nX;
					}
					else
					{
						ptPos.x += m_nX * 4 / 8;
					}
				}
				else
				{
					psz++;
					ptPos.x += m_nX * 4 / 8;
				}
			}
		}
		psz++;
		if (ptPos.x >= width)
		{
			ptPos.y += m_nY + nLine;
			ptPos.x = 0;

			int firstlen = prev_psz-(BYTE*)lpszText;
			int len = strlen(lpszText);

			CopyMemory(first, lpszText, firstlen);
			first[firstlen] = 0;
			CopyMemory(second, prev_psz, len-firstlen);
			second[len-firstlen] = 0;
			return;
		}
		else prev_psz = psz;
	}
	TRACE("SplitString Error\n");
}

int CTWFont::ChatOut(CRect rc, LPCTSTR lpszText, WORD bColor, DDSURFACEDESC2& ddsd, int nLine, BOOL fWordBreak)
{
	int nTop = rc.top;
	CPoint ptPos = rc.TopLeft ();
	BYTE* psz = (BYTE*)lpszText;
	int line_feed = 0;

	while (*psz && ptPos.y + m_nY/* + nLine*/ <= rc.bottom)
	{
		if (*psz <= 0x7F)
		{
			if (0x20 == *psz || 0x09 == *psz)
			{
				if(fWordBreak)
				{
					if(ptPos.x + m_nX*3/8 > rc.right)
					{
						ptPos.y += m_nY + nLine;
						ptPos.x = rc.left;
						line_feed++;
						continue;
					}
				}

				BYTE* pBuf = m_pBitsAsc;
				DrawCharHi(pBuf, ptPos, bColor, ddsd, FALSE);
				ptPos.x += m_nX * 3 / 8;
			}
			else if (0x20 < *psz)
			{
				if(fWordBreak)
				{
					if(ptPos.x + m_nX*4/8 > rc.right)
					{
						ptPos.y += m_nY + nLine;
						ptPos.x = rc.left;
						line_feed++;
						continue;
					}
				}
				BYTE* pBuf = m_pBitsAsc + ((int)*psz - 0x20) * m_nCharLen;
				DrawCharHi (pBuf, ptPos, bColor, ddsd, FALSE);
				ptPos.x += m_nX * 4 / 8;
			}
			else
			{
				if(fWordBreak)
				{
					if(ptPos.x + m_nX*4/8 > rc.right)
					{
						ptPos.y += m_nY + nLine;
						ptPos.x = rc.left;
						line_feed++;
						continue;
					}
				}
				BYTE temp = '?'; 
				BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
				DrawCharHi (pBuf, ptPos, bColor, ddsd, FALSE);
				ptPos.x += m_nX * 4 / 8;
			}
		}
		else
		{
			if (*psz == 0xA4)
			{
				psz++;
				if (*psz >= 0xA0)
				{
					if(fWordBreak)
					{
						if(ptPos.x + m_nX > rc.right)
						{
							ptPos.y += m_nY + nLine;
							ptPos.x = rc.left;
							line_feed++;
							psz--;
							continue;
						}
					}
					BYTE* pBuf = m_pBitsHCode1 + ((int)*psz - 0xA0) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd, FALSE);
					ptPos.x += m_nX;
				}
				else
				{
					if(fWordBreak)
					{
						if(ptPos.x + m_nX*4/8 > rc.right)
						{
							ptPos.y += m_nY + nLine;
							ptPos.x = rc.left;
							line_feed++;
							continue;
						}
					}
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd, FALSE);
					ptPos.x += m_nX * 4 / 8;
				}
			}
			else if (0xB0 <= *psz && *psz <= 0xC8)
			{
				int nOff = ((int)*psz - 0xB0) * 0x60;
				psz++;
				if (*psz >= 0xA0)
				{
					if(fWordBreak)
					{
						if(ptPos.x + m_nX > rc.right)
						{
							ptPos.y += m_nY + nLine;
							ptPos.x = rc.left;
							line_feed++;
							psz--;
							continue;
						}
					}
					BYTE* pBuf = m_pBitsHCode2 + ((int)*psz - 0xA0 + nOff) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd, FALSE);
					ptPos.x += m_nX;
				}
				else
				{
					if(fWordBreak)
					{
						if(ptPos.x + m_nX*4/8 > rc.right)
						{
							ptPos.y += m_nY + nLine;
							ptPos.x = rc.left;
							line_feed++;
							continue;
						}
					}
					BYTE temp = '?'; 
					BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
					DrawCharHi (pBuf, ptPos, bColor, ddsd, FALSE);
					ptPos.x += m_nX * 4 / 8;
				}
			}
			else
			{
				if(fWordBreak)
				{
					if(ptPos.x + m_nX*4/8 > rc.right)
					{
						ptPos.y += m_nY + nLine;
						ptPos.x = rc.left;
						line_feed++;
						continue;
					}
				}
				psz++;
				BYTE temp = '?'; 
				BYTE* pBuf = m_pBitsAsc + ((int)temp - 0x20) * m_nCharLen;
				DrawCharHi (pBuf, ptPos, bColor, ddsd, FALSE);
				ptPos.x += m_nX * 4 / 8;
			}
		}
		psz++;
	}

	return (m_nY + nLine)*(line_feed+1);
}

int CTWFont::GetChatLineCount(int width, LPCTSTR lpszText)
{
	int t_x = 0;
	BYTE* psz = (BYTE*)lpszText;
	int line_feed = 0;

	while(*psz)
	{
		if (*psz <= 0x7F)
		{
			if (0x20 == *psz || 0x09 == *psz)
			{
				if(t_x + m_nX*3/8 > width)
				{
					t_x = 0;
					line_feed++;
					continue;
				}
				t_x += m_nX * 3 / 8;
			}
			else if (0x20 < *psz)
			{
				if(t_x + m_nX*4/8 > width)
				{
					t_x = 0;
					line_feed++;
					continue;
				}
				t_x += m_nX * 4 / 8;
			}
			else
			{
				if(t_x + m_nX*4/8 > width)
				{
					t_x = 0;
					line_feed++;
					continue;
				}
				t_x += m_nX * 4 / 8;
			}
		}
		else
		{
			if (*psz == 0xA4)
			{
				psz++;
				if (*psz >= 0xA0)
				{
					if(t_x + m_nX > width)
					{
						t_x = 0;
						line_feed++;
						psz--;
						continue;
					}
					t_x += m_nX;
				}
				else
				{
					if(t_x + m_nX*4/8 > width)
					{
						t_x = 0;
						line_feed++;
						continue;
					}
					t_x += m_nX * 4 / 8;
				}
			}
			else if (0xB0 <= *psz && *psz <= 0xC8)
			{
				int nOff = ((int)*psz - 0xB0) * 0x60;
				psz++;
				if (*psz >= 0xA0)
				{
					if(t_x + m_nX > width)
					{
						t_x = 0;
						line_feed++;
						psz--;
						continue;
					}
					t_x += m_nX;
				}
				else
				{
					if(t_x + m_nX*4/8 > width)
					{
						t_x = 0;
						line_feed++;
						continue;
					}
					t_x += m_nX * 4 / 8;
				}
			}
			else
			{
				if(t_x + m_nX*4/8 > width)
				{
					t_x = 0;
					line_feed++;
					continue;
				}
				psz++;
				t_x += m_nX * 4 / 8;
			}
		}
		psz++;
	}

	return line_feed+1;
}
