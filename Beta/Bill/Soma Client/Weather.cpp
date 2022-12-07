// Weather.cpp: implementation of the CWeather class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Weather.h"
#include "rain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeather::CWeather()
{
	m_Year	= 0;
	m_Month = 0;
	m_Day	= 0;	
//	m_Hour	= 0;	
	m_Rain	= 0;
	m_Snow	= 0;
	m_bRainStateChange = FALSE;
	m_NightState = 4;
}

CWeather::~CWeather()
{

}

void CWeather::SetWeatherInfo( short Year, short month, short day, DWORD dwHourTime, DWORD dwSubTime, BYTE rain, BYTE NightState )
{
	m_Year	= Year;
	m_Month = month;
	m_Day	= day;	
//	m_Hour	= hour;	
	m_NightState = NightState;
	m_dwHourTime = dwHourTime;
	m_dwSubTime = dwSubTime;
	if( m_Rain != rain )
	{
		m_bRainStateChange = TRUE;
		if( rain ) CRain::SetRain();
		else  CRain::SetEraseRain(); 
		m_Rain = rain;
	}
	else  m_bRainStateChange = FALSE; 
}

int CWeather::GetNightState()
{
	return m_NightState;
	//3:¿œ√‚ 4: ≥∑ 1: ¿œ∏Ù 2: π„
/*
//	switch( m_Hour )
	switch( m_dwHourTime - m_dwSubTime )
	{
	case  0: case 1: return 2;

	case  2: return 3;

	case  3: case  4: case  5: case  6: case  7:
	case  8: case  9: case 10: case 11: case 12:
	case 13: case 14: case 15: case 16: case 17:
	case 18: case 19: case 20: return 4;//≥∑

	case 21: return 1;

	case 22: case 23: return 2;
	}
	return 0;
*/
}
BOOL CWeather::GetRainState()
{
	if( m_Rain == 1 ) return TRUE;
	else			  return FALSE;
}

BOOL CWeather::GetSnowState()
{
	if( m_Snow == 1 ) return TRUE;
	else			  return FALSE;
}

void CWeather::GetWeatherInfo(short& year, short& month, short& day, short& hour, BYTE& rain)
{
	year = m_Year;
	month = m_Month;
	day = m_Day;
	//hour = m_Hour;
	hour = short( m_dwHourTime - m_dwSubTime );
	rain = m_Rain;
}