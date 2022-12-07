// House.cpp: implementation of the CHouse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "House.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHouse::CHouse()
{
	m_pre_house_index = -1;
	m_house_index	  = -1;
	m_bHouse_In		  = FALSE;
	m_TickTime		  = 0;
	m_bChange		  = FALSE;
	m_data			  = CPoint(-1,-1);
	m_nFrame		  = 30;
	m_trans_step	  = 2;
}

CHouse::~CHouse()
{

}

void CHouse::FrameRate()
{
	static DWORD dwStartTick = GetTickCount();
	static int count = 0;
	DWORD dwTick = GetTickCount();
	m_nFrame = count;
	if (dwTick-dwStartTick>1000)
	{
		m_nFrame = count;
		dwStartTick = dwTick;
		count = 0;
		TRACE("frame %d\n",m_nFrame);
	}

	count++;
}

void CHouse::SetTransStep()
{
	if( m_nFrame > 30 )
		m_trans_step = 3;
	else
		m_trans_step = 3;
	TRACE("TRANS_STEP %d\n",m_trans_step);
}
void CHouse::SetHouseIndex( int house_index )
{
//	FrameRate();

	if( house_index != -1 ) 
	{
		m_bHouse_In = TRUE;
	}
	else m_bHouse_In = FALSE;

	static BOOL bSetting = FALSE;
	if( !bSetting )//한번만 수행되고 수행되지 않음
	{
		bSetting = TRUE;
		if(m_bHouse_In) m_trans_degree = 0;
		else			m_trans_degree = 32;
	}

	if( house_index != m_house_index )
	{
		m_bChange = TRUE;	
		m_pre_house_index = m_house_index; 
//		SetTransStep();
	}
	else m_bChange = FALSE;

	m_house_index = house_index;

	if( m_bChange && m_trans_degree == 32)
	{
		m_trans_degree = 31;
		
		//m_TickTime = GetTickCount() + TRANS_GAP_TIME;
	}
}

int CHouse::TransDegree()
{
	DWORD dwNowTime = GetTickCount();

	if( dwNowTime > m_TickTime + TRANS_GAP_TIME )
	{
		m_TickTime = dwNowTime;
		if( m_bHouse_In ) 
		{
//			m_trans_degree -= m_trans_step;
			m_trans_degree -= TRANS_STEP;
			if( m_trans_degree < 0){ m_trans_degree = 0; }
		}
		else
		{
//			m_trans_degree += m_trans_step;
			m_trans_degree += TRANS_STEP;
			if( m_trans_degree > 32){ m_trans_degree = 32; }
		}
	}
	return m_trans_degree;
}
