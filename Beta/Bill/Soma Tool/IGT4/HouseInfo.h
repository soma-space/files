// HouseInfo.h: interface for the CHouseInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOUSEINFO_H__CF6FF901_84C6_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_HOUSEINFO_H__CF6FF901_84C6_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class HOUSECELL
{
public:
	HOUSECELL();
	virtual ~HOUSECELL();

public:
	char m_vMove;		// -1 : 무시, 0 : 이동 불가, 1 : 이동 가능
	char m_vInner;		// -1 : 무시, 1 : 내부

	HOUSECELL& operator=(HOUSECELL& house);
};

class CHouseInfo  
{
public:
	CHouseInfo();
	virtual ~CHouseInfo();

public:
	CSize m_vSize;
	HOUSECELL **m_ppData;

	void DeleteData();

	void Initialize(CSize size);
	void Initialize(int x, int y);

	HOUSECELL *GetCellInfo(int x, int y);
	HOUSECELL *GetCellInfo(CPoint point);
	BOOL SetCellInfo(int x, int y, HOUSECELL move_info);
	void SetCellInfo(POINT *p, HOUSECELL move_info);

	char GetMoveInfo(int x, int y);
	char GetMoveInfo(CPoint point);
	BOOL SetMoveInfo(int x, int y, char move_info);
	void SetMoveInfo(POINT *p, char move_info);

	char GetInnerInfo(int x, int y);
	char GetInnerInfo(CPoint point);
	BOOL SetInnerInfo(int x, int y, char inner_info);
	void SetInnerInfo(POINT *p, char inner_info);

	void Save(CFile& file);
	void Load(CFile& file);
};

#endif // !defined(AFX_HOUSEINFO_H__CF6FF901_84C6_11D4_98B0_00C02615866B__INCLUDED_)
