#if !defined(AFX_BB_SOCKET_H__DCC1F189_0D32_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_BB_SOCKET_H__DCC1F189_0D32_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BB_Socket.h : header file
//


class BB_CircularBuffer;
class SocketMng;

/////////////////////////////////////////////////////////////////////////////
// BB_Socket command target

class BB_Socket : public CSocket
{
// Attributes
public:
	BOOL				SendDelayedData();
	BB_CircularBuffer*	GetReceiveBuffer(){return m_pRCBuffer;}
	void				CustomizedInit();
// Operations
public:
	BB_Socket(SocketMng *pSocketMng, BOOL bSession);
	virtual ~BB_Socket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BB_Socket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(BB_Socket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	BB_CircularBuffer*	m_pRCBuffer;
	SocketMng*			m_bpSocketMng;
	BYTE*				m_pReceiveBuf;

	BOOL				m_bErr;
	DWORD				m_dwFirstErrorTick;
	BOOL				m_bSession;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BB_SOCKET_H__DCC1F189_0D32_11D3_8422_0020AF9F40BD__INCLUDED_)
