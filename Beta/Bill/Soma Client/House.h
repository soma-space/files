// House.h: interface for the CHouse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOUSE_H__62015921_D7F2_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_HOUSE_H__62015921_D7F2_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TRANS_GAP_TIME 50
#define TRANS_STEP 3

class CHouse  
{
private:
	int		m_pre_house_index;
	int		m_house_index;
	BOOL	m_bHouse_In;
	BOOL	m_bChange;
	CPoint	m_data;
	DWORD	m_TickTime;
	int		m_trans_degree;
	int		m_trans_step;
	int		m_nFrame;
public:
	void	SetObjCenterPoint( CPoint pt ){ m_data = pt; }
	CPoint	GetHousePoint(){ return m_data; }
	BOOL	IsChange(){ return m_bChange;}
	BOOL	IsHouseIn(){ return m_bHouse_In;}
	void	SetHouseIndex( int house_index );
	int		GetHouseIndex(){ return m_house_index; }
	void	SetHousePoint( CPoint pt ){ m_data = pt; }
	int		TransDegree();
	void	SetTransStep();
	void	FrameRate();
	int		GetTransDegree(){return m_trans_degree;}

	CHouse();
	virtual ~CHouse();

};

#endif // !defined(AFX_HOUSE_H__62015921_D7F2_11D4_956A_00D0B77259E8__INCLUDED_)
