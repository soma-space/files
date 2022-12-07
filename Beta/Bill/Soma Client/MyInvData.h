// MyInvData.h: interface for the CMyInvData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYINVDATA_H__A0ECA1A1_E9F8_11D3_B175_00105A735718__INCLUDED_)
#define AFX_MYINVDATA_H__A0ECA1A1_E9F8_11D3_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyInvData  
{
public:
	CMyInvData();
	virtual ~CMyInvData();

	void Init();

	CMyInvData& operator=(CMyInvData& other);

public:
	short m_vIndex;
	BYTE m_vType;//무기 타입 모션 조정을 위해서...
	BYTE m_vArmPos;// 장착위
	short m_vPicNum;//그림 번호..
	BYTE m_vGender;//성별
	short m_vMinDamage;		// 최소 위력
	short m_vMaxDamage;		// 최대 위력
	short m_vLimitStr;		// 필요 Str
	short m_vLimitInt;		// 필요 Int
	short m_vLimitSkill;	// 필요 숙련도
	CString m_strMagic;	// 마법
	CString m_strSNum;		// S 항목
	short m_vUsage;
	short m_vMaxUsage;		// 현재 최대 사용회수
	short m_vOrgMaxUsage;	// 원래 최대 사용회수
	short m_vSpecial[7];		// 특수 능력(Str,Dex,Int,Atk,Def,HP,MP)
	DWORD m_vValue;			// 가격
	CString m_vName;
};

#endif // !defined(AFX_MYINVDATA_H__A0ECA1A1_E9F8_11D3_B175_00105A735718__INCLUDED_)
