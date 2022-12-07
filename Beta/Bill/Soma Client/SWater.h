// SWater.h: interface for the SWater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SWATER_H__0ADA3FA4_79AF_11D4_826D_00D0B769A081__INCLUDED_)
#define AFX_SWATER_H__0ADA3FA4_79AF_11D4_826D_00D0B769A081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  POING_SIZE   25

typedef struct tgWATER
{
	int		speed; //속도.
	int		r_max; //최대 반지름.
	int		r_cur; //현재 반지름.
	bool	bIsPong;//파동이 진행중인가.
	int		x_center;//x 중심점.
	int     y_center;//y 중심점.
} WATER;


class SWater  
{
protected:
	WATER	m_pWater[POING_SIZE];

public:
	SWater();
	~SWater(){};

	void InputData(int x, int y);
	void Draw(DDSURFACEDESC2 ddsd);
};

#endif // !defined(AFX_SWATER_H__0ADA3FA4_79AF_11D4_826D_00D0B769A081__INCLUDED_)
