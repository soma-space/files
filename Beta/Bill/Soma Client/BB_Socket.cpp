// BB_Socket.cpp : implementation file
//

#include "stdafx.h"
#include "soma.h"
#include "BB_Socket.h"
#include "BB_CircularBuffer.h"
#include "SocketMng.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "DManage/BB_Direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RECEIVE_BUF_SIZE	8192

/////////////////////////////////////////////////////////////////////////////
// BB_Socket

BB_Socket::BB_Socket(SocketMng *pSocketMng, BOOL bSession)
{
	m_pRCBuffer = new BB_CircularBuffer;
	m_bpSocketMng = pSocketMng;
	m_pReceiveBuf = new BYTE[RECEIVE_BUF_SIZE];

	m_bErr = FALSE;
	m_dwFirstErrorTick = 0L;
	m_bSession = bSession;
}

BB_Socket::~BB_Socket()
{
	ASSERT(m_pRCBuffer);
	SAFE_DELETE(m_pRCBuffer);
	SAFE_DELETES(m_pReceiveBuf);
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(BB_Socket, CSocket)
	//{{AFX_MSG_MAP(BB_Socket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// BB_Socket member functions

void BB_Socket::OnReceive(int nErrorCode) 
{
	if (nErrorCode) 
	{
		TRACE("수신부 OnReceive 에러발생\n");
		return;
	}

	int count=0;
	DWORD totReadNum;
	DWORD readCount=0L;
	DWORD totReadCount= 0L;

	do
	{
		if (0==IOCtl(FIONREAD, &totReadNum)) break;
		if (totReadNum<=0) break;

		readCount = 0L;
		do
		{
			count = CAsyncSocket::Receive(m_pReceiveBuf, RECEIVE_BUF_SIZE, 0);

			if (count==SOCKET_ERROR) 
			{
				TRACE("데이터 처리 Missing\n");
				break;
			}

			if (count) 
			{
				readCount+= count;
				m_pRCBuffer->PutData(m_pReceiveBuf, count);
			}
		} while (readCount<totReadNum);//남은 데이터가 있을 수 있음

		totReadCount += readCount;

	} while (totReadNum>0);

	if (!totReadCount) return;

	m_bErr = FALSE;

//	if(CChildView::PacketParser.GetSessionLoginStatus()) m_bpSocketMng->RawPacketReceive();
//	m_bpSocketMng->TriggerReceive();
	if(m_bSession) m_bpSocketMng->RawPacketReceive();
	else m_bpSocketMng->TriggerReceive();
}

void BB_Socket::CustomizedInit()
{
	AsyncSelect(0);
	AsyncSelect(FD_READ|FD_WRITE|FD_ACCEPT|FD_CONNECT|FD_CLOSE);
}

void BB_Socket::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	CMainFrame* main_frame = __GetMainWnd();
	if(main_frame)
	{
		if (main_frame->m_bFullScreen) 
		{
			CChildView::DirectXMng.m_pDD->GetInterface()->FlipToGDISurface();
		}
		AfxMessageBox("서버와의 연결이 종료되었습니다.");
		m_bpSocketMng->EndManage();
		AfxPostQuitMessage(0);
	}
}

void BB_Socket::OnSend(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
#ifndef MULTITHREAD_SOCKET_PROCESS
	if (nErrorCode)
	{
		TRACE("송신부 ErrorCode발생 %d\n", nErrorCode);
		return;
	}

	ASSERT(m_bpSocketMng);
	DP_PtrList &dpList = m_bpSocketMng->m_listSendDataPack;
	if ((m_hSocket!=INVALID_SOCKET)&&(dpList.GetCount()>0)&&(FALSE==SendDelayedData())) 
	{
		::PostMessage(AfxGetModuleThreadState()->m_hSocketWindow, WM_SOCKET_NOTIFY, m_hSocket, FD_WRITE);
	}
#endif
}

BOOL BB_Socket::SendDelayedData()
{
	DP_PtrList &dpList = m_bpSocketMng->m_listSendDataPack;

	POSITION pos = dpList.GetHeadPosition();
	DataPack *pDtPack;
	int send_count;
	int totalSendCount;
	int count = dpList.GetCount();
	int initCount = count;

	while (pos)
	{
		pDtPack = dpList.GetNext(pos);
		totalSendCount = 0;
		while (pDtPack->m_Size>totalSendCount)
		{
			send_count = CAsyncSocket::Send(pDtPack->m_pData+totalSendCount, pDtPack->m_Size-totalSendCount);
			if (send_count==SOCKET_ERROR) 
			{
				if (count==initCount) 
				{
					if (m_bErr)
					{
						if (GetTickCount()-m_dwFirstErrorTick>30000)
						{
							if (__GetMainWnd()->m_bFullScreen) 
							{
								CChildView::DirectXMng.m_pDD->GetInterface()->FlipToGDISurface();
							}
							AfxMessageBox("서버와의 연결이 좋지 않습니다.");
							AfxPostQuitMessage(0);
							return TRUE;
						}
					}
					else
					{
						m_bErr = TRUE;
						m_dwFirstErrorTick = GetTickCount();
					}
				}
				
				if (totalSendCount)
				{
					dpList.RemoveHead();
					dpList.AddHead(new DataPack(pDtPack->m_Size-totalSendCount, pDtPack->m_pData+totalSendCount, FALSE));
					delete pDtPack;
					m_bErr = FALSE;
				}
				return FALSE;
			}
			else 
			{
				totalSendCount += send_count;
				m_bErr = FALSE;
			}
		}

		delete pDtPack;
		dpList.RemoveHead();

		count--;
	}
	return TRUE;
}