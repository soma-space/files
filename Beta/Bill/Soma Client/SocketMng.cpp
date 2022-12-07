// SocketMng.cpp: implementation of the SocketMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "MainFrm.h"

#include "SocketMng.h"
#include "BB_Socket.h"
#include "BB_CircularBuffer.h"
#include "ChildView.h"
#include "BBDX_ChatOuter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SocketMng::SocketMng()
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	m_pSTrigger = NULL;
	m_pSThread = NULL;
	m_pSCancel = NULL;
	m_pSDone = NULL;
	m_iSTriggerCount = 0;

	m_pRTrigger = NULL;
	m_pRThread = NULL;
	m_pRCancel = NULL;
	m_pRDone = NULL;
#endif
	m_bInit = FALSE;
	m_iPort = 0;

//	m_pSocket = new BB_Socket(this);
	m_pSocket = NULL;
	m_pGameSocket = NULL;
	m_bhRelatedWnd = NULL;
}

SocketMng::~SocketMng()
{
	EndManage();
	POSITION pos = m_listSendDataPack.GetHeadPosition();
	DataPack* pSDP;
	while (pos)
	{
		pSDP = m_listSendDataPack.GetNext(pos);
		SAFE_DELETE(pSDP);
	}
	m_listSendDataPack.RemoveAll();

	pos = m_listReceiveDataPack.GetHeadPosition();
	while (pos)
	{
		pSDP = m_listReceiveDataPack.GetNext(pos);
		SAFE_DELETE(pSDP);
	}
	m_listReceiveDataPack.RemoveAll();
}

BOOL SocketMng::EndManage()
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	if (m_pRCancel)
	{
		if (m_pRThread)
		{
			DWORD dwCode;
			// signal a cancel if still running....
			if ( ::GetExitCodeThread( m_pRThread->m_hThread, &dwCode )
				&& dwCode == STILL_ACTIVE )
			{
				// signal a cancel....
				m_pRCancel->SetEvent();
				// wait for the thread to die....
				WaitForSingleObject( m_pRThread->m_hThread, INFINITE );
			}
			// kill the object...
			SAFE_DELETE(m_pRThread);
		}
		SAFE_DELETE(m_pRCancel);
	}
	RemoveEvents(SM_EVENTS_RECEIVE);

	if (m_pSCancel)
	{
		if (m_pSThread)
		{
			DWORD dwCode;
			// signal a cancel if still running....
			if ( ::GetExitCodeThread( m_pSThread->m_hThread, &dwCode )
				&& dwCode == STILL_ACTIVE )
			{
				// signal a cancel....
				m_pSCancel->SetEvent();
				// wait for the thread to die....
				WaitForSingleObject( m_pSThread->m_hThread, INFINITE );
			}
			// kill the object...
			SAFE_DELETE(m_pSThread);
		}
		SAFE_DELETE(m_pSCancel);
	}
	RemoveEvents(SM_EVENTS_SEND);
#endif

	SAFE_DELETE(m_pGameSocket);
	SAFE_DELETE(m_pSocket);
	return TRUE;
}

BOOL SocketMng::InitManage()
{
	BOOL bOk = TRUE;
#ifdef MULTITHREAD_SOCKET_PROCESS
	bOk = FALSE;
	CreateEvents(SM_EVENTS_RECEIVE);
	m_pRThread = AfxBeginThread((AFX_THREADPROC)SocketMng::ThreadRProc, (LPVOID) this, 
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
	if ( m_pRThread )
	{
		// don't self-destruct (we must delete)....
		m_pRThread->m_bAutoDelete = FALSE;
		// resume...
		m_pRThread->ResumeThread();
		bOk = TRUE;
	}

	if (!bOk) 
	{
		RemoveEvents(SM_EVENTS_RECEIVE);
		return FALSE;
	}
	bOk = FALSE;

	CreateEvents(SM_EVENTS_SEND);
	m_pSThread = AfxBeginThread((AFX_THREADPROC)SocketMng::ThreadSProc, (LPVOID) this, 
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
	if ( m_pSThread )
	{
		// don't self-destruct (we must delete)....
		m_pSThread->m_bAutoDelete = FALSE;
		// resume...
		m_pSThread->ResumeThread();
		bOk = TRUE;
	}

	if (!bOk) RemoveEvents(SM_EVENTS_SEND);
#endif
	m_bInit = bOk;
	if (m_bInit)
	{
		m_pSocket = new BB_Socket(this, TRUE);
		m_pGameSocket = new BB_Socket(this, FALSE);
	}
	return bOk;
}

UINT SocketMng::ThreadRProc(LPVOID pSocketMng)
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	SocketMng *pSM = (SocketMng*)pSocketMng;
	HANDLE Handles[2];
	DWORD wait_result;
	Handles[0] = (HANDLE) *pSM->m_pRTrigger;
	Handles[1] = (HANDLE) *pSM->m_pRCancel;

	do
	{
		wait_result=WaitForMultipleObjects(2, Handles, FALSE, INFINITE);
		if (wait_result==WAIT_OBJECT_0)
		{
			while(ReceiveProcess(pSM));
			pSM->m_pRDone->SetEvent();
			pSM->m_pRTrigger->ResetEvent();
		}
	}while (wait_result==WAIT_OBJECT_0);
	ASSERT(wait_result==WAIT_OBJECT_0+1);
#endif
	return 0;
}

//패킷의 시작과 끝 쌍을 찾아서 Core를 빼냄
BOOL SocketMng::ReceiveProcess(SocketMng *pSocketMng)
{
	static bCount = 0;
	BOOL bFound = FALSE;
	BB_CircularBuffer *pCB = pSocketMng->m_pGameSocket->GetReceiveBuffer();
	int count = pCB->GetValidCount();

	if (count>=6)
	{
		BYTE *pData = new BYTE[count];
		pCB->GetData(pData, count);
		short core_len = 0;
		int head_inc_size = 0;
		BYTE *pCore = NULL;

		for (int i=0; i<count-6; i++)
		{
			if (pData[i]==PS_1&&pData[i+1]==PS_2)
			{
				CopyMemory(&core_len, &pData[i+2], 2);
				
				if(core_len < 0) 
				{
 					TRACE("리시브에서 에러가 났다.\n");
					delete[] pData;
					return FALSE;
				}

				if (i+4+core_len+2<=count)
				{
					if (pData[i+4+core_len]==PE_1&&pData[i+4+core_len+1]==PE_2)
					{
						bFound = TRUE;
						head_inc_size = i+6+core_len;
						pCore = new BYTE[core_len];
						CopyMemory(pCore, &pData[i+4], core_len);
						break;
					}
				}
				else break;
			}
		}

		if (bFound) //Core 발견
		{
			bCount++;
			pCB->HeadIncrease(head_inc_size);
			pSocketMng->AddReceiveData(core_len, pCore);
			SAFE_DELETES(pCore);
		}
		SAFE_DELETES(pData);
	}
	return bFound;
}

BOOL SocketMng::SendProcess(SocketMng *pSocketMng)
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	DP_PtrList& pl = pSocketMng->m_listSendDataPack;
	int num = pSocketMng->m_iSTriggerCount;
	POSITION pos = pl.GetHeadPosition();
	POSITION prev_pos;
	DataPack *p;
	
	int send_count;
	while (pos && num && pSocketMng->m_pGameSocket->m_hSocket!=INVALID_SOCKET)
	{
		prev_pos = pos;
		p = pl.GetNext(pos);
		if (p)
		{
			int tmpSendCount=0;
			send_count = 0;
			while(send_count<p->m_Size && pSocketMng->m_pSocket->m_hSocket!=INVALID_SOCKET)
			{
				tmpSendCount = (int)((CAsyncSocket*)pSocketMng->m_pSocket)->Send(p->m_pData, p->m_Size, 0);
				if (tmpSendCount>=0)
				{
					send_count += tmpSendCount;
				}
			}
					
			SAFE_DELETE(p);
			pSocketMng->m_iSTriggerCount--;
		}
		num--;
		pl.RemoveAt(prev_pos);
	}
#endif
	return TRUE;
}

UINT SocketMng::ThreadSProc(LPVOID pSocketMng)
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	SocketMng *pSM = (SocketMng*)pSocketMng;
	HANDLE Handles[2];
	DWORD wait_result;
	Handles[0] = (HANDLE) *pSM->m_pSTrigger;
	Handles[1] = (HANDLE) *pSM->m_pSCancel;

	do
	{
		wait_result=WaitForMultipleObjects(2, Handles, FALSE, INFINITE);
		if (wait_result==WAIT_OBJECT_0)
		{
			SendProcess(pSM);
			if (pSM->m_iSTriggerCount)
			{
				TRACE("전송부: TriggerCount 이상: %d\n", pSM->m_iSTriggerCount);
				pSM->m_pSTrigger->SetEvent();
			}
			else
			{
				pSM->m_pSTrigger->ResetEvent();
				pSM->m_pSDone->SetEvent();
			}
		}
	}while (wait_result==WAIT_OBJECT_0);
	ASSERT(wait_result==WAIT_OBJECT_0+1);
#endif
	return 0;
}

void SocketMng::RemoveEvents(int mode)
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	if (mode==SM_EVENTS_RECEIVE)
	{
		SAFE_DELETE(m_pRDone);
		SAFE_DELETE(m_pRTrigger);
		SAFE_DELETE(m_pRCancel);
	}
	else
	{
		ASSERT(mode==SM_EVENTS_SEND);
		SAFE_DELETE(m_pSDone);
		SAFE_DELETE(m_pSTrigger);
		SAFE_DELETE(m_pSCancel);
	}
#endif
}

void SocketMng::CreateEvents(int mode)
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	if (mode==SM_EVENTS_RECEIVE)
	{
		ASSERT(m_pRDone==0);
		ASSERT(m_pRTrigger==0);
		ASSERT(m_pRCancel==0);

		m_pRDone = new CEvent;
		m_pRDone->SetEvent();
		m_pRTrigger = new CEvent;
		m_pRTrigger->ResetEvent();
		m_pRCancel = new CEvent;
		m_pRCancel->ResetEvent();
	}
	else
	{
		ASSERT(mode==SM_EVENTS_SEND);

		ASSERT(m_pSDone==0);
		ASSERT(m_pSTrigger==0);
		ASSERT(m_pSCancel==0);

		m_pSDone = new CEvent;
		m_pSDone->SetEvent();
		m_pSTrigger = new CEvent;
		m_pSTrigger->ResetEvent();
		m_pSCancel = new CEvent;
		m_pSCancel->ResetEvent();
	}
#endif
}

void SocketMng::TriggerSend()
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	ASSERT(m_pSTrigger);
	ASSERT(m_pSDone);

	if (WaitForSingleObject(*m_pSDone, 0)!=WAIT_OBJECT_0)
	{
		::PostMessage(m_bhRelatedWnd, WM_PACKET_PROCESS_PENDING, SM_PACKETPENDING_SEND, 0L);
		return;
	}


	m_iSTriggerCount = m_listSendDataPack.GetCount();
	if (m_iSTriggerCount>0)
	{
		if (m_pSDone) m_pSDone->ResetEvent();
		if (m_pSTrigger) m_pSTrigger->SetEvent();
	}
#endif
}

void SocketMng::TriggerReceive()
{
#ifdef MULTITHREAD_SOCKET_PROCESS
	ASSERT(m_pRTrigger);
	ASSERT(m_pRDone);
	
	if (WaitForSingleObject(*m_pRDone, 0)!=WAIT_OBJECT_0)
	{
		::PostMessage(m_bhRelatedWnd, WM_PACKET_PROCESS_PENDING, SM_PACKETPENDING_RECEIVE, 0L);
		return;
	}
	

	m_pRDone->ResetEvent();
	m_pRTrigger->SetEvent();
#else
	while(ReceiveProcess(this));
#endif
}

BOOL SocketMng::ConnectSocket()
{
//	ASSERT(m_iPort);
//	ASSERT(!m_szDestIp.IsEmpty());
//	ASSERT(m_pSocket);

	if (!m_pSocket->Create())
	{
		AfxMessageBox("소켓 생성 실패");
		return FALSE;
	}
	while (!m_pSocket->Connect((LPCTSTR)m_szDestIp, m_iPort))
	{
		int errorcode = GetLastError();

		if (AfxMessageBox("재 접속을 시도 하시겠습니까?",MB_YESNO) == IDNO)	return FALSE;
	}
	m_pSocket->CustomizedInit();
	return TRUE;
}

BOOL SocketMng::ConnectGameSocket(CString dstIP, UINT dstPort)
{
	if (!m_pGameSocket->Create())
	{
		AfxMessageBox("게임 소켓 생성 실패");
		return FALSE;
	}
	while (!m_pGameSocket->Connect((LPCTSTR)dstIP, dstPort))
	{
		int errorcode = GetLastError();

		if (AfxMessageBox("게임 접속을 시도 하시겠습니까?",MB_YESNO) == IDNO)	return FALSE;
	}
	m_pGameSocket->CustomizedInit();
	return TRUE;
}

void SocketMng::Send(int size, BYTE *pData)
{
	int iCount = m_listSendDataPack.GetCount();
	m_listSendDataPack.AddTail(new DataPack(size, pData, TRUE));
	if (m_pGameSocket && m_pGameSocket->m_hSocket!=INVALID_SOCKET) 
	{
#ifdef MULTITHREAD_SOCKET_PROCESS
		TriggerSend();
#else
		//메세지 방식에 의한 지연 Packet보내기
//		if (0==iCount) ::PostMessage(AfxGetModuleThreadState()->m_hSocketWindow, WM_SOCKET_NOTIFY, m_pSocket->m_hSocket, FD_WRITE);
		::PostMessage(AfxGetModuleThreadState()->m_hSocketWindow, WM_SOCKET_NOTIFY, m_pGameSocket->m_hSocket, FD_WRITE);
#endif
	}
}

void SocketMng::SendRawPacket(int size, BYTE *pData)
{
	int iCount = m_listSendDataPack.GetCount();
	m_listSendDataPack.AddTail(new DataPack(size, pData, FALSE));	//시작,끝을 표시하지 않는 패킷생성
#ifdef MULTITHREAD_SOCKET_PROCESS
	TriggerSend();
#else
	if (0==iCount) ::PostMessage(AfxGetModuleThreadState()->m_hSocketWindow, WM_SOCKET_NOTIFY, m_pSocket->m_hSocket, FD_WRITE);
#endif
}

void SocketMng::AddReceiveData(int size, BYTE *pData)
{
	m_listReceiveDataPack.AddTail(new DataPack(size, pData));
	if (m_bhRelatedWnd) ::PostMessage(m_bhRelatedWnd, WM_PACKET_ARRIVED, 0, 0L);
}

//Session과의 통신중일때 Receive함수 (NetGame과 동일)
void SocketMng::RawPacketReceive()
{
	BB_CircularBuffer *pCB = m_pSocket->GetReceiveBuffer();
	int count = pCB->GetValidCount();

	BYTE *pData = new BYTE[count];
	pCB->GetData(pData, count);
	PBYTE pTmpData = pData;

	WORD wSize;
	int offset;
	while (4 <= count && *((WORD*)(pTmpData + 2)) + 4 <= count)
	{
		offset = 2;
		wSize = Parse_GetWord(pTmpData, offset);
		wSize += 4;
		CChildView::PacketParser.SessionPacketProcess(pTmpData, wSize);
		pCB->HeadIncrease(wSize);

		pTmpData += wSize;
		count -= wSize;
	}

	SAFE_DELETES(pData);
}

void SocketMng::SendAcountLogin()
{
	BYTE buf[100];
	int offset=0;

	//게임 서버에 접속할때 로그인...^^
	MP_AddByte(buf, offset, 0xfe);
	MP_AddByte(buf, offset, m_szAcountLoginId.GetLength());
	MP_AddString(buf, offset, (PBYTE)(LPCTSTR)m_szAcountLoginId, m_szAcountLoginId.GetLength());
//패스워드는 서버에서 사용하지 않음
//	MP_AddByte(buf, offset, m_szPasswd.GetLength());
//	MP_AddString(buf, offset, (PBYTE)(LPCTSTR)m_szPasswd, m_szPasswd.GetLength());
	ASSERT(offset<100);
	Send(offset, buf);
}


void SocketMng::SendLogin()
{
	BYTE buf[100];
	int offset=0;

	//게임 서버에 접속할때 로그인...^^
	MP_AddByte(buf, offset, 0x01);
	MP_AddByte(buf, offset, m_szLoginId.GetLength());
	MP_AddString(buf, offset, (PBYTE)(LPCTSTR)m_szLoginId, m_szLoginId.GetLength());
	TRACE("Login....!!\n");
	ASSERT(offset<100);
	Send(offset, buf);
}

void SocketMng::SendLogout(short mode)
{
	BYTE buf[10];
	int offset = 0;
	MP_AddByte(buf, offset, PKT_LOGOUT);
	MP_AddShort(buf, offset, mode);
	Send(offset, buf);
}

void SocketMng::SendChat(CString &str, int type)
{
	if(type == 1 && !str.CompareNoCase("/clear"))
	{
		BBDX_ChatOuter *outer = __GetMainWnd()->m_wndView.GetChatOuter();
		outer->ClearString();
		return;
	}

	// 구조를 바꿔줘야한다.
	PBYTE pBuf = new BYTE[str.GetLength()+30];
	int offset = 0;
	
	MP_AddByte(pBuf, offset, PKT_CHAT);
	//채팅종류:  1. 시야채팅, 2. 쪽지 
	MP_AddByte(pBuf, offset, type);
	if(type == 2)//쪽지일때..
	{
		CString str_id = __GetMainWnd()->m_wndView.GetCurMemoChatId();// 임시..
		MP_AddByte(pBuf, offset, (BYTE)str_id.GetLength());// 아이디 이름 길이..
		MP_AddString(pBuf, offset, (PBYTE)(LPCTSTR)str_id, str_id.GetLength());// 아이디이름.
	}
//	MP_AddByte(pBuf, offset, (BYTE)str.GetLength());// 빼버림..
	MP_AddString(pBuf, offset, (PBYTE)(LPCTSTR)str, str.GetLength());

	Send(offset, pBuf);
	SAFE_DELETES(pBuf);
}

