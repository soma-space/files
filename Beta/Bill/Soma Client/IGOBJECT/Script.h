// Script.h: interface for the CScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCT_H__4F1571A3_BCDF_11D2_B0A3_0060970DBAD5__INCLUDED_)
#define AFX_SCT_H__4F1571A3_BCDF_11D2_B0A3_0060970DBAD5__INCLUDED_

class CScript
{
public:
	const _SCT_DATA* Get();
	const _SCT_DATA* Get(int index);
	void Release(); // 스크립트 초기화..
	
	int GetQuantity() { return m_Header.nQt; }
	void Load(const char* szFileName);
	CScript();
	virtual ~CScript();

protected:
	_SCT_HEADER m_Header;				// 스프라이트 헤더
	_SCT_DATA* m_pDatas[MAX_SCT];	// 널이면 데이터가 없는 상태이다.
	int m_nIndex; // 현재 처리중인 인덱스..
	
	DWORD 	m_dwTickDelay; // 지연 시간 설정
	char		m_szFileName[256]; // 최근 로딩한 파일 이름..

private:
	void Alloc(int nQt);
};

#endif // !defined(AFX_SCT_H__4F1571A3_BCDF_11D2_B0A3_0060970DBAD5__INCLUDED_)
