// MapObj.h: interface for the CMapObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPOBJ_H__421F291A_2F93_4654_8E53_CA5B035E4EA0__INCLUDED_)
#define AFX_MAPOBJ_H__421F291A_2F93_4654_8E53_CA5B035E4EA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct SAVE_BUF
{
	int index;
	short value;
} SAVE_BUF;

class CMapObj  
{
public:
	_MAP_OBJ_HEADER m_Header;
	HGLOBAL m_hData; // 맵 데이터 메모리 핸들
	short int* m_pnData; // 실제 맵 데이터.. 상위 4비트는 플래그.. 0-없음 1-충돌 2-투명찍기 4-안찍기. 하위 12비트는 인덱스
	SAVE_BUF *m_pTData;
	int m_vTCount;

public:
	void Alloc(int nWidth, int nHeight);
	short int DataGet(int x, int y) // 앞의 8비트는 세트 번호, 뒤의 8비트는 서브세트 (에니메이션) 번호 이다. 만약 -1 이면 그자리엔 암것도 없다.
	{
		if(x < 0 || y < 0 || x >= m_Header.nWidth || y >= m_Header.nHeight) return -1;
		else return m_pnData[y*m_Header.nWidth + x];
	}
	void DataSet(int x, int y, short int nData)
	{
		if(x < 0 || y < 0 || x >= m_Header.nWidth || y >= m_Header.nHeight) return;
		m_pnData[y*m_Header.nWidth + x] = nData;
	}
	void Load(const char* szFileName);
	void Load(HANDLE& hFile);
	void Release();

	CMapObj();
	virtual ~CMapObj();

};

#endif // !defined(AFX_MAPOBJ_H__421F291A_2F93_4654_8E53_CA5B035E4EA0__INCLUDED_)
