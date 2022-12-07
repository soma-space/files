// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__F726F1E5_A707_11D2_93E9_00C02615866B__INCLUDED_)
#define AFX_DIB_H__F726F1E5_A707_11D2_93E9_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DECLARE_HANDLE(HDIB);
#define PALVERSION				0x300
#define IS_WIN30_DIB(lpbi)		((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)		((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)		((lpRect)->bottom - (lpRect)->top)
#define WIDTHBYTES(bits)		(((bits) + 31) / 32 * 4)

class CDib  
{
public:
	CDib();
	virtual ~CDib();

	BOOL ReadFile(CString lpszPathName);
	void StretchDraw(CDC *pdc, CPoint start_point, CSize limit_size);
	void Draw(CDC *pdc, CPoint start_point);
	void TransparentDraw(CDC *pdc, CPoint start_point, COLORREF color = RGB(0,0,0));
	BOOL IsDataAvailable() {return (m_hDIB != NULL);};
	void RemoveData();

	BOOL Save(CArchive& file);
	BOOL Load(CArchive& file);
	BOOL LoadCore(CArchive& file, LPSTR pheader, DWORD pheader_size);
	BOOL SaveCore(CArchive& file);
	BOOL GetBitmapInfoCopy(LPSTR *pbinfo, DWORD& size);
	void Copy(CDib& dib);
	BOOL ReadData(LPTSTR data, DWORD data_size);

	static LPSTR FindDIBBits(LPSTR lpbi);
	static WORD PaletteSize(LPSTR lpbi);
	static WORD DIBNumColors(LPSTR lpbi);
	static DWORD DIBWidth(LPSTR lpDIB);
	static DWORD DIBHeight(LPSTR lpDIB);
public:
	HDIB m_hDIB;
	DWORD m_vDIBSize;
	CSize m_vSize;
};

#endif // !defined(AFX_DIB_H__F726F1E5_A707_11D2_93E9_00C02615866B__INCLUDED_)
