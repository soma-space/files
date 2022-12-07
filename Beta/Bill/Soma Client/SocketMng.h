// SocketMng.h: interface for the SocketMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETMNG_H__010054CC_0204_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_SOCKETMNG_H__010054CC_0204_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
class BB_Socket;

#define SM_EVENTS_RECEIVE			0
#define SM_EVENTS_SEND				1

#define PS_1						0XAA
#define PS_2						0X55
#define PE_1						0X55
#define PE_2						0XAA

#define SM_PACKETPENDING_SEND		0
#define	SM_PACKETPENDING_RECEIVE	1

class DataPack
{
public:
	DataPack(int size, BYTE *pData, BOOL bSend=FALSE)	//통신용 데이터 패킷(시작(PS_1, PS_2)과 끝(PE_1,PE_2)로 구성
	{													//bSend인자가 TRUE이면 데이터 패킷(Server와 통신) FALSE이면 Raw패킷(Session과 통신)
		ASSERT(size);
		if (bSend)
		{
			m_Size = size+6;
			m_pData = new BYTE[m_Size];
			m_pData[0] = PS_1;
			m_pData[1] = PS_2;
			CopyMemory(m_pData+2, &size, 2);
			CopyMemory(m_pData+4, pData, size);
			m_pData[size+4] = PE_1;
			m_pData[size+5] = PE_2;
		}
		else
		{
			m_Size = size;
			m_pData = new BYTE[size];
			CopyMemory(m_pData, pData, size);
		}
	}
	virtual ~DataPack(){delete[] m_pData;}

	int		m_Size;
	BYTE*	m_pData;
};

typedef CTypedPtrList<CPtrList, DataPack*> DP_PtrList;


//#define MULTITHREAD_SOCKET_PROCESS
#define WM_SOCKET_NOTIFY	0x0373
#define WM_SOCKET_DEAD		0x0374


class SocketMng  
{
public:
	void			SendChat(CString &str, int type=1);
	void			SendLogout(short mode = 0);
	void			SendLogin();

	void			AddReceiveData(int size, BYTE *pData);	//소켓에서 받은데이터를 DP_PtrList에 보관함
	void			RawPacketReceive();						//서버와의 통신이 아닌Session과의 통신에 사용
	void			SendRawPacket(int size, BYTE *pData);	//Session에 Packet보내기
	void			Send(int size, BYTE *pData);			
	BOOL			ConnectSocket();						//소켓 연결 (대상은 Session: Zone Server로 구성되기 때문)
	BOOL			ConnectGameSocket(CString dstIP, UINT dstPort);	//게임 소켓 연결
	void			SetDestination(CString dstIp, UINT dstPort); //소켓 대상 설정
	void			TriggerSend();							
	void			TriggerReceive();
	BOOL			EndManage();
	BOOL			InitManage();
	static	BOOL	SendProcess(SocketMng *pSocketMng);
	static	BOOL	ReceiveProcess(SocketMng *pSocketMng);
	static	UINT	ThreadRProc(LPVOID pSocketMng);
	static	UINT	ThreadSProc(LPVOID pSocketMng);

	//패킷 만들기 함수
	static	void	MP_AddShort(BYTE *dest, int& offset, short value);
	static	void	MP_AddInt(BYTE *dest, int& offset, int value);
	static	void	MP_AddByte(BYTE *dest, int& offset, BYTE byte);
	static	void	MP_AddWord(BYTE *dest, int& offset, WORD data);
	static	void	MP_AddString(BYTE *dest, int &offset, BYTE *sor, int len);
	static	void	MP_AddDword(BYTE *dest, int &offset, DWORD dword);

	//패킷 Parsing 함수
	static	short&	Parse_GetShort(BYTE *buf, int& offset);
	static	WORD&	Parse_GetWord(BYTE *buf, int& offset);
	static	BYTE&	Parse_GetByte(BYTE *buf, int &offset);
	static	void	Parse_GetString(BYTE *buf, int &offset, BYTE *dest, int len);
	static	DWORD&	Parse_GetDword(BYTE *buf, int &offset);

    CString			m_szLoginId1;
	CString			m_szLoginId2;
	CString			m_szLoginId3;

	SocketMng();
	virtual ~SocketMng();

protected:
	void			CreateEvents(int mode);
	void			RemoveEvents(int mode);
public:
	void SendAcountLogin();

	//MultiThread로 송신용, 수신용 Thread를 사용할 경우
#ifdef MULTITHREAD_SOCKET_PROCESS
	//SM_EVENTS_RECEIVE
	CEvent*			m_pSDone;
	CEvent*			m_pSTrigger;
	int				m_iSTriggerCount;
	CEvent*			m_pSCancel;
	CWinThread*		m_pSThread;	//Socket Send용

	//SM_EVENTS_SEND
	CEvent*			m_pRDone;
	CEvent*			m_pRTrigger;
	CEvent*			m_pRCancel;
	CWinThread*		m_pRThread; //Socket Receive용
#endif

	CString			m_szDestIp;
	UINT			m_iPort;
	BOOL			m_bInit;
	BB_Socket*		m_pSocket;
	BB_Socket		*m_pGameSocket;
	CString			m_szAcountLoginId;
	CString			m_szLoginId;
	CString			m_szPasswd;


	DP_PtrList		m_listSendDataPack;			//Send용 단위패킷 보관장소
	DP_PtrList		m_listReceiveDataPack;		//Receive용 단위패킷 보관장소

	HWND			m_bhRelatedWnd;				//패킷 도착시 메세지를 Post할 대상 윈도우 Handle
};

inline void SocketMng::SetDestination(CString dstIp, UINT dstPort)
{
	m_szDestIp = dstIp;
	m_iPort = dstPort;
}

inline void SocketMng::MP_AddShort(BYTE *dest, int& offset, short value)
{
	CopyMemory(dest+offset, &value, 2);
	offset += 2;
}

inline void SocketMng::MP_AddInt(BYTE *dest, int& offset, int value)
{
	CopyMemory(dest+offset, &value, 4);
	offset += 4;
}

inline void SocketMng::MP_AddByte(BYTE *dest, int& offset, BYTE byte)
{
	CopyMemory(dest+offset, &byte, 1);
	offset ++;
}

inline void SocketMng::MP_AddWord(BYTE *dest, int& offset, WORD data)
{
	CopyMemory(dest+offset, &data, 2);
	offset += 2;
}

inline void SocketMng::MP_AddString(BYTE *dest, int &offset, BYTE *sor, int len)
{
	CopyMemory(dest+offset, sor, len);
	offset += len;
}

inline void SocketMng::MP_AddDword(BYTE *dest, int &offset, DWORD dword)
{
	CopyMemory(dest+offset, &dword, 4);
	offset += 4;
}

inline short& SocketMng::Parse_GetShort(BYTE *buf, int& offset)
{
	offset += 2;
	return *(short*)(buf+offset-2);
}

inline WORD& SocketMng::Parse_GetWord(BYTE *buf, int &offset)
{
	offset += 2;
	return *(PWORD)(buf+offset-2);
}

inline BYTE& SocketMng::Parse_GetByte(BYTE *buf, int &offset)
{
	offset ++;
	return *(BYTE*)(buf+offset-1);
}

inline void SocketMng::Parse_GetString(BYTE *buf, int &offset, BYTE *dest, int len)
{
	if (len)
	{
		CopyMemory(dest, buf+offset, len);
		offset += len;
	}
	dest[len] = 0;
}

inline DWORD& SocketMng::Parse_GetDword(BYTE *buf, int &offset)
{
	offset += 4;
	return *(DWORD*)(buf+offset-4);
}

#endif // !defined(AFX_SOCKETMNG_H__010054CC_0204_11D3_8422_0020AF9F40BD__INCLUDED_)
