// BB_CircularBuffer.cpp: implementation of the BB_CircularBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "BB_CircularBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BB_CircularBuffer::BB_CircularBuffer(int size)
{
	ASSERT(size>0);
	m_iBufSize = size;
	m_pBuffer = new BYTE[m_iBufSize];

	m_iHeadPos = 0;
	m_iTailPos = 0;
}

BB_CircularBuffer::~BB_CircularBuffer()
{
	ASSERT(m_pBuffer);
	SAFE_DELETES(m_pBuffer);
	m_pBuffer = NULL;
}
