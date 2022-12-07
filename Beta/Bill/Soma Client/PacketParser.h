// PacketParser.h: interface for the CPacketParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKETPARSER_H__85110601_4E72_11D3_8423_00105A6B96E7__INCLUDED_)
#define AFX_PACKETPARSER_H__85110601_4E72_11D3_8423_00105A6B96E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChildView;
class CUser;
#include "SocketMng.h"

class CPacketParser  //패킷 분석기
{
public:
	void			ChangeItemInfo(PBYTE bypData);						// 아이템 정보 변화
	void			ChatParse(PBYTE bypData);							//채팅데이터
	void			AttackProcess(PBYTE bypData);						//공격
	void			ArrowAttackProcess(PBYTE bypData);					//화살공격 
	void			ZoneChange(int uid, int x, int y, int mapnum);		//Zone의 이동
	void			MoveRequest(CUser *pUserMe, CPoint ptDstCellPos, int FindStep = 20);	//이동 요청
	void			MoveProcess(PBYTE bypData, BYTE kind);				//이동 처리
	void			LoginProcess(PBYTE bypData);						//Login 처리

	BOOL			PacketProcess(PBYTE bypData, int size);				//초기 패킷처리함수(위의 함수들로 분기됨)
	BOOL			SessionPacketProcess(PBYTE bypData, int size);		//Session과의 통신중일때 처리
	
	//패킷에서 빼내는 함수
	static	short&	GetShort(PBYTE bypData, int &offset);				
	static	int&	GetInt(PBYTE bypData, int &offset);
	static	BYTE&	GetByte(PBYTE bypData, int &offset);
	static	WORD&	GetWord(PBYTE bypData, int &offset);
	static	DWORD&	GetDword(PBYTE bypData, int &offset);
	static	void	GetString(PBYTE bypData, int &offset, PBYTE dest, int len);

	//패킷을 만들때 사용하는 함수
	static	void	SetShort(PBYTE bypData, int &offset, short data);
	static	void	SetInt(PBYTE bypData, int &offset, int data);
	static	void	SetByte(PBYTE bypData, int &offset, BYTE data);	
	static	void	SetWord(PBYTE bypData, int &offset, WORD data);
	static	void	SetDword(PBYTE bypData, int &offset, DWORD data);
	static	void	SetString(PBYTE bypData, int &offset, PBYTE sor, int len);
	static  CPoint  Conv2ServerMapPos(CPoint p);//에버랜드에서 가져온거..
	static  CPoint  Conv2ClientMapPos(CPoint p);//에버랜드에서 가져온거..


	CPacketParser();
	virtual ~CPacketParser();

	//Session과의 통신인가 Server와의 통신인가 구별
	void			SetSessionLoginStatus(BOOL tf) {m_bIsSessionLogin = tf;}
	BOOL			GetSessionLoginStatus() {return m_bIsSessionLogin;}
public:
	void ConnectInfoProcess(PBYTE bypData);
	void BattleModeProcess(PBYTE bypData);
	void MoveRunRequest(CUser *pUserMe, CPoint ptDstCellPos, int FindStep = 20);
	void Magic_CircleProcess(PBYTE bypData);
	void Magic_ReadyProcess(PBYTE bypData);
	void Magic_PoisonProcess(PBYTE bypData);
	void CharStateChange(PBYTE bypData);
	void AcountLoginProcess(PBYTE bypData);
	void DeadProcess(PBYTE bypData);
	void SetTimeProcess(PBYTE bypData);
	void ShowMagicProcess(PBYTE bypData);
	void ArrowMagicRailAttackProcess(PBYTE bypData);
	void ApplyWeight(PBYTE bypData);
	CChildView*		m_bpConnectedView;

	void OpenGuildMenu(PBYTE bypData);

protected:
	BOOL			m_bIsSessionLogin;

	bool            m_bPathFind;//우석..
};

inline short& CPacketParser::GetShort(PBYTE bypData, int &offset)
{
	return SocketMng::Parse_GetShort(bypData, offset);
}

inline int& CPacketParser::GetInt(PBYTE bypData, int &offset)
{
	offset += 4;
	return *(int*)(bypData+offset-4);
}

inline BYTE& CPacketParser::GetByte(PBYTE bypData, int &offset)
{
	return SocketMng::Parse_GetByte(bypData, offset);
}

inline WORD& CPacketParser::GetWord(PBYTE bypData, int &offset)
{
	return SocketMng::Parse_GetWord(bypData, offset);
}

inline DWORD& CPacketParser::GetDword(PBYTE bypData, int &offset)
{
	return SocketMng::Parse_GetDword(bypData, offset);
}

inline void CPacketParser::GetString(PBYTE bypData, int &offset, PBYTE dest, int len)
{
	SocketMng::Parse_GetString(bypData, offset, dest, len);
}

inline void CPacketParser::SetShort(PBYTE bypData, int &offset, short data)
{
	SocketMng::MP_AddShort(bypData, offset, data);
}

inline void CPacketParser::SetInt(PBYTE bypData, int &offset, int data)
{
	SocketMng::MP_AddInt(bypData, offset, data);
}

inline void CPacketParser::SetByte(PBYTE bypData, int &offset, BYTE data)
{
	SocketMng::MP_AddByte(bypData, offset, data);
}

inline void	CPacketParser::SetString(PBYTE bypData, int &offset, PBYTE pSor, int len)
{
	SocketMng::MP_AddString(bypData, offset, pSor, len);
}

inline void CPacketParser::SetWord(PBYTE bypData, int &offset, WORD data)
{
	SocketMng::MP_AddWord(bypData, offset, data);
}

inline void CPacketParser::SetDword(PBYTE bypData, int &offset, DWORD data)
{
	SocketMng::MP_AddDword(bypData, offset, data);
}

#endif // !defined(AFX_PACKETPARSER_H__85110601_4E72_11D3_8423_00105A6B96E7__INCLUDED_)
