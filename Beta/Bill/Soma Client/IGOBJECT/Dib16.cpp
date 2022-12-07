// Dib.cpp: implementation of the CDib16 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dib16.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDib16::CDib16()
{
	m_hBuff = NULL;
	MAKE_ZERO(m_Header);
}

CDib16::~CDib16()
{
	Release();
}

void CDib16::Release()
{
	if(m_hBuff)
	{
		GlobalFree(m_hBuff); // 메모리 해제..
		m_hBuff = NULL;
	}
	MAKE_ZERO(m_Header);
}

BOOL CDib16::ImportGenericBMP(const char* szFileName)
{
	DWORD dwAccessed = 0;
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString("비트맵 Import 실패 : 파일 열기 실패 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return FALSE;
	}

	BITMAPFILEHEADER bmHFile;
	BITMAPINFOHEADER bmHInfo;

	ReadFile(hFile, &bmHFile, sizeof(BITMAPFILEHEADER), &dwAccessed, NULL);
	if(bmHFile.bfType != 0x4d42)
	{
		OutputDebugString("비트맵 Import 실패 : 비트맵 파일이 아님 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		CloseHandle(hFile);
		return FALSE;
	}

	ReadFile(hFile, &bmHInfo, sizeof(BITMAPINFOHEADER), &dwAccessed, NULL);
	
	if(New(bmHInfo.biWidth, bmHInfo.biHeight) == FALSE) // 생성에 실패하면 파일로부터 읽는다.
	{
		CloseHandle(hFile);
		return FALSE;
	}

	DWORD dwAllocSize = 0;
	RGBQUAD* pcrIndices = NULL;
	DWORD dwPixelSize = 0;
	if(bmHInfo.biClrUsed != 0 && bmHInfo.biBitCount == 8) // 인덱스가 있고 8 비트 비트맵이면..
	{
		dwAllocSize = (bmHInfo.biWidth+bmHInfo.biWidth%2)*bmHInfo.biHeight;
		dwPixelSize = 1;
		
		pcrIndices = new RGBQUAD[256]; // 팔레트 버퍼 생성
		ReadFile(hFile, pcrIndices, sizeof(RGBQUAD)*256, &dwAccessed, NULL); // 팔레트를 읽는다.
	}
	else if(bmHInfo.biClrUsed == 0 && bmHInfo.biBitCount == 24) // 인덱스가 없고 24 비트 비트맵이면..
	{
		dwAllocSize = (((bmHInfo.biWidth*3)+(bmHInfo.biWidth*3)%2))*bmHInfo.biHeight;
		dwPixelSize = 3;
	}
	else
	{
		OutputDebugString("비트맵 Import 실패 : 알수 없는 비트맵 파일 형식 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		CloseHandle(hFile);
		return FALSE;
	}

	HGLOBAL hSrc = GlobalAlloc(GMEM_FIXED, dwAllocSize); // 임시 버퍼 생성
	unsigned char* pbiSrc = (unsigned char*)GlobalLock(hSrc);
	ReadFile(hFile, pbiSrc, dwAllocSize, &dwAccessed, NULL); // 파일에서 읽기

	WORD* pwDest = this->Lock(); // 내부 버퍼 락
	for(int y = 0; y < bmHInfo.biHeight; y++)
	{
		unsigned char* pbiTmpSrc = pbiSrc + (bmHInfo.biHeight - y - 1) * (bmHInfo.biWidth*dwPixelSize + (bmHInfo.biWidth*dwPixelSize)%2); // 비트맵은 거꾸로 있다..
		WORD* pwTmpDest = pwDest + y * bmHInfo.biWidth;
		for(int x = 0; x < bmHInfo.biWidth; x++)
		{
			if(dwPixelSize == 3) // 24 비트 일때
			{
				*pwTmpDest = this->RGB16(pbiTmpSrc[2], pbiTmpSrc[1], pbiTmpSrc[0]);
			}
			else
			{
				*pwTmpDest = this->RGB16(pbiTmpSrc[2], pbiTmpSrc[1], pbiTmpSrc[0]);
			}
			pbiTmpSrc += dwPixelSize;
			pwTmpDest++;
		}
	}
	this->Unlock(); // 내부 버퍼 언락

	CloseHandle(hFile); // 파일 핸들 닫기

	GlobalUnlock(hSrc);
	GlobalFree(hSrc); hSrc = NULL; // 임시 버퍼 해제..
	if(pcrIndices) { delete [] pcrIndices; pcrIndices = NULL; } // 팔레트 버퍼 해제
	return TRUE;
}

BOOL CDib16::ExportTo24BMP(const char *szFileName)
{
	if(m_Header.nWidth <= 0 || m_Header.nHeight <= 0)
	{
		OutputDebugString("비트맵 Export 실패 : 가로 혹은 세로 0 보다 작음 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return FALSE;
	}

	DWORD dwAccessed = 0;
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString("비트맵 Export 실패 : 파일 열기 실패 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return FALSE;
	}
	
	BITMAPFILEHEADER bmHFile; MAKE_ZERO(bmHFile);
	BITMAPINFOHEADER bmHInfo; MAKE_ZERO(bmHInfo);

	bmHFile.bfSize = sizeof(BITMAPFILEHEADER);
	bmHFile.bfType = 0x4d42; // 'BM'
	bmHFile.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	
	bmHInfo.biSize = sizeof(BITMAPINFOHEADER);
	bmHInfo.biWidth = m_Header.nWidth;
	bmHInfo.biHeight = m_Header.nHeight;
	bmHInfo.biBitCount = 24;
	bmHInfo.biCompression = BI_RGB;
	bmHInfo.biPlanes = 1;
	bmHInfo.biSizeImage = 0; // 압축된 형식이 아니다....!!
	bmHInfo.biClrUsed = 0; // 24 비트 컬러~!!
	bmHInfo.biClrImportant = 0;

	WriteFile(hFile, &bmHFile, sizeof(BITMAPFILEHEADER), &dwAccessed, NULL);
	WriteFile(hFile, &bmHInfo, sizeof(BITMAPINFOHEADER), &dwAccessed, NULL);
	
	WORD* pwSrc = this->Lock(); // 내부 버퍼 락

	int nPitch = m_Header.nWidth*3+m_Header.nWidth%2;
	HGLOBAL hDest = GlobalAlloc(GMEM_FIXED, nPitch*m_Header.nHeight);  // 임시 버퍼 메모리 할당..
	BYTE* pDest = (BYTE*)GlobalLock(hDest); // 임시 버퍼 락..
	// 데이터 읽기..
	COLORREF crPixel = 0;
	for(int y = m_Header.nHeight - 1, y2 = 0; y >= 0; y--, y2++) // 거꾸로 써야 할걸...
	{
		WORD* pwTmpSrc = pwSrc + y2*m_Header.nWidth;
		BYTE* pTmpDest = pDest + y*nPitch;
		for(int x = 0; x < m_Header.nWidth; x++)
		{
			crPixel = this->RGB24(*(pwTmpSrc+x));
			*(pTmpDest+0) = (BYTE)((crPixel&0x00ff0000)>>16); // r
			*(pTmpDest+1) = (BYTE)((crPixel&0x0000ff00)>>8); // g
			*(pTmpDest+2) = (BYTE)((crPixel&0x000000ff)); // b
			
			pTmpDest += 3;
		}
	}

	// 파일에 쓰기..
	WriteFile(hFile, pDest, nPitch*m_Header.nHeight, &dwAccessed, NULL);

	GlobalUnlock(hDest); // 임시 버퍼 언락
	GlobalFree(hDest); hDest = NULL;
	this->Unlock(); // 내부 버퍼 언락

	CloseHandle(hFile);
	return TRUE;
}

BOOL CDib16::New(int nWidth, int nHeight)
{
	if(	nWidth <= 0 || nHeight <= 0)
	{
		OutputDebugString("비트맵 생성 실패 : 너비, 높이가 0보다 작음");
		OutputDebugString("\n");
		return FALSE;
	}
	
	Release();
	
	m_hBuff = GlobalAlloc(GMEM_FIXED, nWidth*nHeight*2);  // 메모리 할당..
	if(m_hBuff != NULL)
	{
		m_Header.nWidth = nWidth;
		m_Header.nHeight = nHeight;
		m_Header.szID[0] = 16;
		m_Header.szID[1] = 'B';
		m_Header.szID[2] = 'M';
		m_Header.szID[3] = 'P';
		WORD* pwDest = (WORD*)GlobalLock(m_hBuff);
		memset(pwDest, 0, nWidth*nHeight*2);
		GlobalUnlock(m_hBuff);
	}
	else
	{
		Release();
		OutputDebugString("비트맵 새로 만들기 : 메모리 할당 실패\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CDib16::Load(const char* szFileName)
{
	DWORD dwAccessed = 0;
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString("비트맵 파일 부르기 실패 : 파일 열기 실패 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return FALSE;
	}

	// 헤더를 읽고 16 비트 비트맵인지 확인...
	_BMP16_HEADER Header;
	ReadFile(hFile, &Header, sizeof(_BMP16_HEADER), &dwAccessed, NULL);
	if(Header.szID[0] == 'B' && Header.szID[1] == 'M' && Header.szID[2] == 'C' && Header.szID[3] == 16)
	{
		int data_size, d_count;
		int i = 0, j = 0, k;
		WORD filter;

		ReadFile(hFile, &data_size, sizeof(int), &dwAccessed, NULL);
		WORD *src_data = new WORD[data_size];
		ReadFile(hFile, src_data, data_size*2, &dwAccessed, NULL);
		// 새로 생성..
		if(New(Header.nWidth, Header.nHeight)) // 생성에 성공하면 파일로부터 읽는다.
		{
			lstrcpy(m_Header.szRemark, Header.szRemark); // 헤더의 주석을 붙여주고..
			WORD* pBuff = this->Lock();
			while(i < data_size)
			{
				// 실제 데이터
				d_count = *(int *)(src_data+i); i += 2;
				if(d_count > 0)
				{
					CopyMemory(pBuff+j, src_data+i, d_count*2);
					j += d_count;
					i += d_count;
				}
				d_count = *(int *)(src_data+i); i += 2;
				filter = src_data[i++];
				for(k = 0; k < d_count; k++,j++) *(pBuff+j) = filter;
			}
			this->Unlock();
		}

		delete[] src_data;
		CloseHandle(hFile);
		return TRUE;
	}
	else if(Header.szID[0] == 'B' && Header.szID[1] == 'M' && Header.szID[2] == 'P' && Header.szID[3] == 16)
	{
		if(New(Header.nWidth, Header.nHeight)) // 생성에 성공하면 파일로부터 읽는다.
		{
			lstrcpy(m_Header.szRemark, Header.szRemark); // 헤더의 주석을 붙여주고..
			WORD* pBuff = this->Lock();
			ReadFile(hFile, pBuff, m_Header.nWidth*m_Header.nHeight*2, &dwAccessed, NULL); // 읽는다..
			this->Unlock();
		}

		CloseHandle(hFile);
		return TRUE;
	}
	else
	{
		CloseHandle(hFile);
		return FALSE;
	}
}

BOOL CDib16::Save(const char *szFileName)
{
	DWORD dwAccessed = 0;
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString("비트뱁 저장 : 파일 열기 실패 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return FALSE;
	}

	// 헤더를 읽고 16 비트 비트맵인지 확인...
	m_Header.szID[0] = 'B', m_Header.szID[1] = 'M', m_Header.szID[2] = 'P', m_Header.szID[3] = 16;
	WriteFile(hFile, &m_Header, sizeof(_BMP16_HEADER), &dwAccessed, NULL);

	WORD* pBuff = this->Lock();
	WriteFile(hFile, pBuff, m_Header.nWidth*m_Header.nHeight*2, &dwAccessed, NULL); // 읽는다..
	this->Unlock();

	CloseHandle(hFile);

	return TRUE;
}

// 정해진 영역을 지정된 색깔로 칠한다. 영역포인터가 NULL이면 전체를 칠한다..
void CDib16::FillColor(RECT *pRC, COLORREF crFill)
{
	if(m_Header.nWidth <= 0 || m_Header.nHeight <= 0) return;
	
	RECT rcFill;
	if(pRC == NULL) SetRect(&rcFill, 0, 0, m_Header.nWidth, m_Header.nHeight);
	else
	{
		RECT rcWhole = {0, 0, m_Header.nWidth, m_Header.nHeight};
		IntersectRect(&rcFill, pRC, &rcWhole);
		if(rcFill.right - rcFill.left <= 0 || rcFill.bottom - rcFill.top <= 0) return; // 채울 영역이 잘못되면 돌아감..
	}

	WORD wFillColor = this->RGB16(crFill);
	int nPixel = m_Header.nHeight*m_Header.nWidth;
	
	WORD* pwDest = this->Lock();
	for(int i = 0; i < nPixel; i++) *(pwDest+i) = wFillColor;
	this->Unlock();
}
