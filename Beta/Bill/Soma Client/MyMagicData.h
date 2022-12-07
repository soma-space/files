// MyMagicData.h: interface for the CMyMagicData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MyMagicData_H__A0ECA1A1_E9F8_11D3_B175_00105A735718__INCLUDED_)
#define AFX_MyMagicData_H__A0ECA1A1_E9F8_11D3_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyMagicData  
{
public:
	CMyMagicData();
	virtual ~CMyMagicData();

public:
	void Init();

	short m_vNumber;	// 마법 번호
	BYTE m_vMethod;		// 마법 구동 방법
	BYTE m_vCharOrAxis;	// 마법 구동 방식(캐릭터 대상인지 좌표 대상인지)
	short m_vStartTime;	// 마법 발동 시간
	BYTE m_vType;		// 마법 타입
	BYTE m_vTgtMethod;	// 마법 목표 타입
	CString m_vName;	// 마법 이름
	CString m_vExplain;	// 마법 설명
	short m_vDistance;	// 마법 거리
	short m_vUseMp;		// 마법력 감소량
	int m_vUseTime;		// 마법 지속 시간
	short m_vDamage;	// 마법 위력
	short m_vGamePic;	// 게임상에 사용되는 그림 번호
};

#endif // !defined(AFX_MyMagicData_H__A0ECA1A1_E9F8_11D3_B175_00105A735718__INCLUDED_)
