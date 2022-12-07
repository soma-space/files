// Dialog_AccountLogin.h: interface for the CDialog_AccountLogin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Dialog_AccountLogin_H__2FB2B681_D569_11D3_B175_00105A735718__INCLUDED_)
#define AFX_Dialog_AccountLogin_H__2FB2B681_D569_11D3_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "./DManage/BB_Direct.h"
#include "SWater.h"

class BBDX_Editor;
class BBDX_TextOuter;
class CTWFont;

//우석 class CDialog_AccountLogin  : public CDialog_Box
class CDialog_AccountLogin  : public CUIRes
{
public:
	CDialog_AccountLogin();
	virtual ~CDialog_AccountLogin();

public:
	void GameOpen();

	int				m_Dialog_AccountLogin_Del;
	int				m_vCurOk, m_vOk;
	int				m_SuccessNewChar;
	int				m_SuccessLogin;
	BOOL			m_vSex;							// FALSE : 남자, TRUE : 여자
	int				m_vViewMode;
	CPoint			m_vMsgDlgPos;
	CRect			m_vMsgRect;

	int             m_PersonalCheckOK; // Personal Check 의 성공여부.

	BOOL MouseProcess(BBDX_MouseInformation *pMouseInfo, int *id_flag);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);


	BOOL  Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL  Draw( CDDSurface* pDestSurf );
	//사용자가 입력한 문자를 출력할 영역을 구한다.
	CRect GetTextOutRect(int index);
	void  CreateEditor(CWnd *pParent, int view_mode);
	void  DrawEditorText(CDDSurface *pSurface, int view_mode, int id_flag);
	void  DrawErrorText(DDSURFACEDESC2 &ddsd);

	void InitAccount();
	void InitNewAccount();
	void InitMsg();

	void	SetViewMode( int view_mode );
	int		GetViewMode() { return m_vViewMode; };
	void	SetMessage( CString str );

	void    DeletePersonalCheck();

	void KillFocus();
	void SetFocus();

public:
	BOOL			m_bShow;
	BBDX_Editor*     m_pBBDX_Editor_id;
    BBDX_Editor*     m_pBBDX_Editor_password;
    BBDX_Editor*     m_pBBDX_Editor_password2;//3 패스워드 확인
	BBDX_Editor*     m_pBBDX_Editor_name;//4
	BBDX_Editor*     m_pBBDX_Editor_email;//5
	BBDX_Editor*     m_pBBDX_Editor_number1;//6 주민번호
	BBDX_Editor*     m_pBBDX_Editor_number2;//6 주민번호
	BBDX_Editor*     m_pBBDX_Editor_address;//7 주소
	BBDX_Editor*     m_pBBDX_Editor_phonenumber;//8전화번호
	int				 *m_id_flag;// 마우스가 클릭된 곳으로 커서이동.
	bool             m_KorlangMode;
	bool             m_KorlangMode2;
	bool             m_JuMinCheck;
	bool             m_JuMinCheck2;

protected:
	CPoint			m_aniPoint;
	int				m_aniIndex;
	DWORD			m_TickCount;
	SWater			m_water;
	CString         m_ErrorText; // 에러 메세지.
	CTWFont			*m_ErrorDraw;
};

#endif // !defined(AFX_Dialog_AccountLogin_H__2FB2B681_D569_11D3_B175_00105A735718__INCLUDED_)

