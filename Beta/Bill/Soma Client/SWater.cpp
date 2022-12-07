// SWater.cpp: implementation of the SWater class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "SWater.h"

#include <time.h>
#include <math.h>
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define PI 3.141592

SWater::SWater()
{
	int i;

	for(i = 0; i < POING_SIZE; i++)	m_pWater[i].bIsPong = false;
}

void SWater::InputData(int x, int y)
{
	int i;

	srand((unsigned)time(NULL));

	for(i = 0; i < POING_SIZE; i++)
	{
		if(!m_pWater[i].bIsPong)
		{
			m_pWater[i].x_center = x;
			m_pWater[i].y_center = y;
			m_pWater[i].r_max = 60 + rand() % 70;
			m_pWater[i].r_cur = 0;
			m_pWater[i].bIsPong = true;
			m_pWater[i].speed = 1 + rand() % 2;

			break;
		}
	}
}

void SWater::Draw(DDSURFACEDESC2 ddsd)
{
	int		i;
	double	rad;
	int     y_off;
	int     x_off;
	int     put_offset;
	int     index;
	WORD	color;
	WORD    *pSurface = (WORD *)ddsd.lpSurface;
	int     pitch = ddsd.lPitch/2;

	for(index = 0; index < POING_SIZE; index++)
	{
		if(m_pWater[index].bIsPong)
		{
			for(i = 0; i < 360; i++)
			{
				rad = PI/180 * i;
				y_off = (int)(m_pWater[index].r_cur * sin(rad));
				x_off = (int)(m_pWater[index].r_cur * cos(rad));

				put_offset = (m_pWater[index].y_center + y_off) * pitch + m_pWater[index].x_center + x_off;
				if( (put_offset >= 0) && ( put_offset < pitch * 600 ) ) color = pSurface[put_offset];

				if( (i >= 0 && i < 90) || (i >= 270 && i < 360) )
				{
					put_offset = (m_pWater[index].y_center + y_off - 1) * pitch + m_pWater[index].x_center + x_off;

					if( (put_offset >= 0) && ( put_offset < pitch * 600 ) )
					{
						pSurface[put_offset] = color;
						pSurface[put_offset + 1] = color;
						pSurface[put_offset - 1] = color;
					}

					put_offset = (m_pWater[index].y_center + y_off - 2) * pitch + m_pWater[index].x_center + x_off;
					if( (put_offset >= 0) && ( put_offset < pitch * 600 ) )
					{
						pSurface[put_offset] = color;
						pSurface[put_offset + 1] = color;
						pSurface[put_offset - 1] = color;
					}
				}

				if(i >= 90 && i < 270)
				{
					put_offset = (m_pWater[index].y_center + y_off +1) * pitch + m_pWater[index].x_center + x_off;
					if( (put_offset >= 0) && ( put_offset < pitch * 600 ) )
					{
						pSurface[put_offset] = color;
						pSurface[put_offset + 1] = color;
						pSurface[put_offset - 1] = color;
					}

					put_offset = (m_pWater[index].y_center + y_off + 2) * pitch + m_pWater[index].x_center + x_off;
					if( (put_offset >= 0) && ( put_offset < pitch * 600 ) )
					{
						pSurface[put_offset] = color;
						pSurface[put_offset + 1] = color;
						pSurface[put_offset - 1] = color;
					}
				}

			} //for

			m_pWater[index].r_cur += m_pWater[index].speed;
			if(m_pWater[index].r_cur >= m_pWater[index].r_max) m_pWater[index].bIsPong = false;
		} // if
	}  //for
}