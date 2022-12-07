// AniObj.h: interface for the CAniObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIOBJ_H__65C6CB21_84CF_11D3_9925_00C026EE03FC__INCLUDED_)
#define AFX_ANIOBJ_H__65C6CB21_84CF_11D3_9925_00C026EE03FC__INCLUDED_

#include "stdafx.h"

class CAniObj // �⺻������ 8 ������ �����ϴ� ���ϸ��̼� ������Ʈ
{
public:
	_ANI_HEADER			m_HAni;

protected:
	_ANI_DATA*			m_pAnis[MAX_ANI];

	int		m_nCtrlQt; // ���ϸ��̼� ���� ��
	int*	m_nCurAnis; // ���� ������ ���ϸ��̼�
	int*	m_nDefaultAnis; // �⺻ ���ϸ��̼�.. �������� ������ ���� ���ϸ��̼��� �ȴ�..
	DWORD*	m_dwTickFrms; // ���ϸ��̼�..
	int*	m_nCurFrms; // ���� ������ ���ϸ��̼��� ������.
	int*	m_nCurDirs; // ���� ������ ����
	int*	m_nFlags; // ���ϸ��̼� ��Ʈ�� �÷���   FL_LOOP, FL_ONCE, FL_ONCE_FIX;

	float	m_fASpeed; // �׼� ���ǵ�..

public:
	int		FrmGetAnimated(int nCtrl = 0); // ���� ���ϸ��̼� �����Ϳ� ���� ���ϴ� ������ ��ȣ�� ����
	float	ActionSpeedSet(float fSpeed); // ���ϸ��̼� ��� �ӵ��� ����

	void	AniAlloc(int nQt); // ���ϸ��̼� ������ ���� ��ŭ �Ҵ�
	void	AniCtrlAlloc(int nQt); // ���ϸ��̼� ���� �� ����
	void	AniRelease(); // ���ϸ��̼� ������ ������
	void	AniCtrlRelease(); // ���ϸ��̼� ���� ������ ������.

	int		AniGet(int nCtrl = 0) { if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return 0; return m_nCurAnis[nCtrl]; } // ������ ���ϸ��̼� ��ȣ ��������
	void	AniSet(int nAni, int nFlag = FL_ANI_LOOP, int nCtrl = 0); // ���ϸ��̼� ��ȣ ����
	void	DirSet(int nDir, int nCtrl = 0); // ���� ����
	int		DirGet(int nCtrl = 0) { if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return 0; return m_nCurDirs[nCtrl]; } // ���� ���� ��������

	int		FrmGet(int nCtrl = 0) { if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return 0; return m_nCurFrms[nCtrl]; } // 
	void	FrmSet(int nFrm, int nCtrl = 0); // 
	int		FrmGetByAniAndDirAndSeq(int nAni, int nDir, int nFrm); // ���� ���ϸ��̼� ����� ��ȣ�� ���� ������ ��ȣ�� �Ѱ� �ش�.

	int		FrmGetMax(int nCtrl = 0);
	
	_ANI_DATA*	AniDataGet(int index); // ���ϸ��̼� ������ ������ ��������

	void		AniLoad(const char* szFileName); // ���ϸ��̼� ���� �ҷ�����
	
	CAniObj();
	virtual ~CAniObj();
};

#endif // !defined(AFX_ANIOBJ_H__65C6CB21_84CF_11D3_9925_00C026EE03FC__INCLUDED_)