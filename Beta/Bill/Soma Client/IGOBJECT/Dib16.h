// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB16_H__999FA3BB_C8C3_4974_A3A8_559BF3935E3F__INCLUDED_)
#define AFX_DIB16_H__999FA3BB_C8C3_4974_A3A8_559BF3935E3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDib16
{
protected:
	HGLOBAL m_hBuff;
	_BMP16_HEADER m_Header;

public:
	void FillColor(RECT* pRC, COLORREF crFill);
	BOOL New(int nWidth, int nHeight);
	BOOL Load(const char* szFileName); // 16비트 사용자 정의 비트맵 파일 부르기..
	BOOL Save(const char* szFileName); // 16비트 사용자 정의 비트맵 파일로 저장..
	BOOL ExportTo24BMP(const  char* szFileName); // 보통의 24비트 비트맵 파일로 저장.
	BOOL ImportGenericBMP(const char* szFileName); // 보통의 24비트 비트맵 파일 부르기..
	int	GetWidth() { return m_Header.nWidth; } // 너비 돌려준다.
	int GetHeight() { return m_Header.nHeight; } // 높이 돌려준다.
	WORD* Lock() { if(m_hBuff) return (WORD*)GlobalLock(m_hBuff); else return NULL; } // 16 비트 : R,G,B - 5,6,5 비트씩 | 24 비트 : B,G,R - 8비트씩 | 32 비트 A,B,G,R 8비트씩
	void Unlock() { if(m_hBuff) GlobalUnlock(m_hBuff); }
	void Release();
	
	CDib16();
	virtual ~CDib16();
public:
	WORD RGB16(COLORREF crColor) // COLORREF값을 5,6,5 워드 컬러 값으로..
		{ return (WORD) ( (((crColor&0x000000ff)>>3)<<11) | ((((crColor&0x0000ff00)>>8)>>2)<<5) | (((crColor&0x00ff0000)>>16)>>3) ); }
	WORD RGB16(WORD wR, WORD wG, WORD wB) // R,G,B 값을 현재 픽셀 포맷에 맞추어 16비트 값으로..
		{ return (((wR>>3)<<11) | ((wG>>2)<<5) | (wB>>3)); }
	COLORREF RGB24(WORD wColor) // 5,6,5 워드 컬러 값을 COLORREF값으로..
		{ COLORREF crColor = wColor; return ( (((crColor&0x0000f800)>>11)<<3) | ((((crColor&0x000007e0)>>5)<<2)<<8) | (((crColor&0x0000001f)<<3)<<16) ); }
};

#endif // !defined(AFX_DIB16_H__999FA3BB_C8C3_4974_A3A8_559BF3935E3F__INCLUDED_)
