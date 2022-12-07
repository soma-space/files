// Dialog_KrBoard.h: interface for the CDialog_BoardCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_KRBOARD_H__6566C202_6153_11D4_A1F5_00D0B769A081__INCLUDED_)
#define AFX_DIALOG_KRBOARD_H__6566C202_6153_11D4_A1F5_00D0B769A081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"


#include "Soma.h"
#include "ChildView.h"
#include "DManage\BBDX_MouseInformation.h"
#include "DManage/BB_Direct.h"
#include "DManage/AlphaProcess.h"
#include "illstruct.h"

class CDirectDraw;
class CChildView;

class CDialog_BoardList;
class CDialog_BoardRead;
class CDialog_BoardWrite;

class CTWFont;


#define LINE_GAP 3


#define BOARDDRAW_NONE	0
#define BOARDDRAW_LIST	1
#define BOARDDRAW_READ	2
#define BOARDDRAW_WRITE 3

#define BOARD_NORMAL	0		// 읽기, 쓰기, 자기글 지우기
#define BOARD_READONLY	1		// 읽기만 가능
#define BOARD_READWRITE	2		// 읽기, 쓰기만 가능, 지우기 불가
#define BOARD_FULL		3		// 모든 권한(클라이언트는 0번과 같음)

class LIST_DATA;
typedef CTypedPtrArray<CPtrArray, LIST_DATA*> ARR_LIST;
class LIST_DATA {
public:
	int m_nNum;
	CString m_strData[3];
};


class CDialog_BoardCtrl
{
public:
	BOOL IsUnique(int nNum);
	void BBS_SetRead(PBYTE bypData);
	void BBS_PKT_Process(PBYTE bypData);
	BOOL BBS_SetList(PBYTE bypData);
	void BBS_SetOpen(PBYTE bypData);
	void DrawMode(int nMode);
	int BoardMouseProcess(BBDX_MouseInformation *pMouseInfo);
	BOOL InitData(CDirectDraw *pDD, CChildView *pWnd);
	void Draw(CDDSurface *pDest);

	int GetBoardNum() {return m_nBoardNum;};
	void RemoveAllSaveList();

	CDialog_BoardCtrl();
	virtual ~CDialog_BoardCtrl();

	CUIRes *m_pDrawBoard;		// 그려주기 위한 포인터로서 위의 세개의  Board중 하나를 가르킨다
								// Virtual로 Dynamic 실행Type을 따라가고 있다. 
	ARR_LIST m_arrList;
	BYTE m_nBoardSpec;		// 보드 속성

	CDialog_BoardList *m_pBoardList;	//  Board List보여주기.
	CDialog_BoardRead *m_pBoardRead;	//  Board 읽기.
	CDialog_BoardWrite *m_pBoardWrite;	//  Board 쓰기.

protected:
	CDirectDraw *m_pDD;
	CChildView *m_pWnd;

	int m_nBoardNum;	// BoardCtrl이 관리하는 게시판의 번호.

	int m_nDrawMode;
	
private:
	BOOL m_bInit;
};

#endif // !defined(AFX_DIALOG_KRBOARD_H__6566C202_6153_11D4_A1F5_00D0B769A081__INCLUDED_)
