// Item_Inv.cpp: implementation of the CItem_Inv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Item_Inv.h"

#include "soma.h"
#include "User.h"
#include "DManage/BB_Direct.h"
#include "PathFind.h"
#include "MapManager/SM_MapManager.h"
#include "DirectXMng.h"
#include "DManage/BBDX_MouseInformation.h"
#include "ChildView.h"
#include "DManage/DirectSound.h"
#include "Magic.h"
#include "ArmatureManager.h"
#include "DManage/Direct3D.h"
#include "MainFrm.h"

#define SAFE_DELETEARRAY(p)  { if(p) { delete [] (p);     (p)=NULL; } }
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }

extern   DirectXMng			DirectXMng;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//================================================================
//
//================================================================
INV_SPR_DATA::INV_SPR_DATA()
{
	m_arrSprData = NULL;
	m_arrVData = NULL;	
}

INV_SPR_DATA::~INV_SPR_DATA()
{
	if(m_arrSprData)
	{
		delete[] m_arrSprData;
	}

	if(m_arrVData)
	{
		for(int i = 0; i < m_HSpr.nQt; i++)
		{
			delete[] m_arrVData[i];
		}
		delete[] m_arrVData;
	}
}

INV_ANI_DATA::INV_ANI_DATA()
{
	m_AniData.pnFrms = NULL;
}

INV_ANI_DATA::~INV_ANI_DATA()
{
	if(m_AniData.pnFrms) { delete[] m_AniData.pnFrms; m_AniData.pnFrms = NULL; }
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItem_Inv::CItem_Inv()
{
	m_pre_AniCount = 0;
	m_timegab = 55;

	m_CharDrawDir = true;
	m_bLoginMode = true;	

	for(int i = 0; i < ANI_FRAME_COUNT; i++) m_pTargetSurface[i] = NULL;
	m_alpha = 12;
	m_alphastate = false;

	m_SkinColor = 0; //0:백인. 1:황인 2:흑인.
	m_MyHair = 101;
	for(i = 0; i< 10; i++) m_Draw_Order[i] = 0;

	for(i = 0; i < INV_KIND_NUM; i++) m_arrInv[i] = NULL;
}

CItem_Inv::~CItem_Inv()
{
	for (int i = 0; i < INV_KIND_NUM; i++)
	{
		if(m_arrInv[i]) SAFE_DELETE(m_arrInv[i]);
	}

	DestoryTexSurface();
}

void CItem_Inv::Init(int Sex)
{
	m_AniCount = 0;
	for(int i=0;i<11;i++)
		m_OK_Flag[i] = 0;// 처음에는 착용을 안하고 있다..음냐..
	
	m_dwTick = GetTickCount();
	m_Gender = Sex;


	if(m_Gender == 1) //여자.
	{
		LoadAni("girlinv.ani");
		Load_spl("Inv", 11, m_SkinColor, 1);
	}
	else //남자.
	{
		LoadAni("maninv.ani");
		Load_spl("Inv", 1, m_SkinColor, 1);
	}
//우석.

}

//우석.. 9월 24일.
void CItem_Inv::DestoryTexSurface()
{
	m_bLoginMode = false;
	m_bCahrBlend = false;

	for(int i = 0; i < ANI_FRAME_COUNT; i++)
	{
		SAFE_DELETE(m_pTargetSurface[i]);
	}
}

//우석. 9월 24일.
void CItem_Inv::CreateTexture()
{
	m_bLoginMode = true;
	m_bCahrBlend = false;
	m_AniCount = 0;

	for(int i = 0; i < ANI_FRAME_COUNT; i++)
	{
		m_pTargetSurface[i] = new CDDSurface();
		m_pTargetSurface[i]->Create(CChildView::DirectXMng.m_pDD, 256, 256, FALSE);
		m_pTargetSurface[i]->SurfaceFill(RGB(0,0,0));
		m_pTargetSurface[i]->SetColorKey(RGB(0,0,0));
		Draw(120, 205);
		m_AniCount++;
	}
	m_AniCount = 0;
}

void CItem_Inv::Load_spl(LPSTR szFileName, int type, int file_index, int arrindex)
{
	int Count = 0, index = 0, i = 0;
	UINT count;
	CFile spl_file;
	WORD *word;
	INV_SPR_DATA *spr_data;
	CString szSplFileName;
	const CString szExePath = CDirectDraw::GetExecutionPath();

	if(index >= 500) index -= 500;
	szSplFileName.Format("%sinv\\%s%02d%03d.spl", (LPCTSTR)szExePath ,szFileName,type, file_index);

	if(spl_file.Open(szSplFileName, CFile::modeRead) == FALSE)
	{
		TRACE("그림을 찾을수없음. %s\n", szSplFileName);
		return;
	}		

	if(m_arrInv[arrindex]) SAFE_DELETE(m_arrInv[arrindex]);
		
	spr_data = new INV_SPR_DATA;
	
	spl_file.Read(&spr_data->m_HSpr, sizeof(_SPR_HEADER));
	spl_file.Seek(sizeof(_SPR_TOOL)*spr_data->m_HSpr.nQt, CFile::current);
	
	if(spr_data->m_HSpr.nQt > 0)
	{
		spr_data->m_arrSprData = new _SPR_LSP[spr_data->m_HSpr.nQt];
		spr_data->m_arrVData = new WORD *[spr_data->m_HSpr.nQt];
		
		for (i=0; i<spr_data->m_HSpr.nQt; i++)
		{
			spl_file.Read((PVOID)&spr_data->m_arrSprData[i], sizeof(_SPR_LSP));
			spr_data->m_arrSprData[i].pwV = NULL;
		}
		
		for (i=0; i<spr_data->m_HSpr.nQt; i++)
		{
			count = spr_data->m_arrSprData[i].nDataSize;
			if(count)
			{
				word = new WORD[count>>1];
				spl_file.Read((PVOID)word, count);
				if(CChildView::CharManager.GetPixelFormat()==PF_555) CCharRes::Decode2_555(spr_data->m_arrSprData[i].nLine, word);
				spr_data->m_arrVData[i] = word;
			}
			else
			{
				spr_data->m_arrVData[i] = NULL;
			}
		}
	}
		
	m_arrInv[arrindex] = spr_data;
		
	spl_file.Close();

}


void CItem_Inv::LoadAni(LPCTSTR szAniFileName)
{
	const CString szExePath = CDirectDraw::GetExecutionPath();
	int i=0;

	int Count = 0;
//	UINT count;
	CFile ani_file;
//	WORD *word;
	int index = 0;

	CString szFileName;
	szFileName.Format("%sinv\\%s", (LPCTSTR)szExePath ,szAniFileName);
	if(ani_file.Open(szFileName, CFile::modeRead) == FALSE)
	{
		TRACE("애니화일을 열수 없음.. %s\n",szAniFileName);
		return;
	}
	if(m_aniInv.m_AniData.pnFrms) { delete[] m_aniInv.m_AniData.pnFrms; m_aniInv.m_AniData.pnFrms = NULL; }
	// 애니메이션	
	ani_file.Read((PVOID)&m_aniInv.m_HAni, sizeof(_ANI_HEADER));
	ani_file.Read((PVOID)&m_aniInv.m_AniData, sizeof(_ANI_DATA));	
	m_aniInv.m_AniData.pnFrms = NULL;

	if(m_aniInv.m_AniData.nMaxFrm > 0)
	{
		int nQt = m_aniInv.m_HAni.nDirQt * m_aniInv.m_AniData.nMaxFrm; // 갯수 계산 = 방향 * 프레임갯수
		m_aniInv.m_AniData.pnFrms = new short int[nQt]; // 갯수 만큼 생성
		ani_file.Read(m_aniInv.m_AniData.pnFrms, nQt*2); // 갯수 만큼 읽는다.
	}
	
	ani_file.Close();

}


void CItem_Inv::SetHair(int style,int color)
{
	m_MyHair = style * 10 + color;
}

void CItem_Inv::Change_InvItem(int type, int index, int PicNum)
{
	if(type > 11) return;
	if(index < 0) index = 0;

	if(type == 1 || type == 2 || type == 8 || type == 9) return;

	if(index !=0)
		m_OK_Flag[type] = 1;
	else
	{
		if(type != 0)
		{
			m_OK_Flag[type] = 0;
			m_Draw_Order[type] = 0;
			return;
		}
		else 
		{
			m_OK_Flag[type] = 1;
			index = m_MyHair;
		}
	}

	m_Draw_Order[type] = __GetMainWnd()->m_wndView.GetItemDrawOrderIndex(PicNum);

	int arrindex = type;

	if(m_Gender == 1)// 여자
		type += 10;

	Load_spl("Inv", type, index, arrindex);
}

void CItem_Inv::DrawPrimitiveChar(int x,int y)
{
	if(m_AniCount >= 0 && m_AniCount < ANI_FRAME_COUNT && m_pTargetSurface[m_AniCount])
	{
		CRect rect,srect;
		CDDSurface* pDestSurf = CChildView::DirectXMng.m_pDrawSurface;

		rect.SetRect(x,y,x+256, y+256);
		if(m_bCahrBlend)
		{
			DDSURFACEDESC2 ddsd;
			DWORD RBBitMask, GBitMask;//, TransColor;
			
			if( CChildView::CharManager.GetPixelFormat() == 565 ) 
			{
				RBBitMask = 0x0000f81f;
				GBitMask  = 0x000007e0;
			}
			else
			{
				RBBitMask = 0x00007c1f;
				GBitMask  = 0x000003e0;
			}
			

			pDestSurf->Lock();
			m_pTargetSurface[m_AniCount]->Lock();

			ddsd = pDestSurf->GetDescription();
			WORD *pDest = (WORD *)ddsd.lpSurface; 
			pDest += rect.top * pDestSurf->GetPitch() / 2 + rect.left;

			ddsd = m_pTargetSurface[m_AniCount]->GetDescription();
			WORD *pSrc = (WORD *)ddsd.lpSurface; 
			if(pDest && pSrc)
			{
				for(int i = 0; i < m_pTargetSurface[m_AniCount]->GetHeight(); i++)
				{
					PutPixelTrans(pDest, pSrc, m_pTargetSurface[m_AniCount]->GetPitch() / 2, m_alpha, RBBitMask, GBitMask);
					pSrc += m_pTargetSurface[m_AniCount]->GetPitch() / 2;
					pDest += pDestSurf->GetPitch() / 2;
				}
			}

			m_pTargetSurface[m_AniCount]->Unlock();
			pDestSurf->Unlock();
		}
		else
		{
			m_pTargetSurface[m_AniCount]->GetRect(srect);
			pDestSurf->Blt(&rect, m_pTargetSurface[m_AniCount],&srect, DDBLT_KEYSRC);
		}
	}
//	else
//	{
//		CString t_str;

//		t_str.Format("%d", m_AniCount);
//		AfxGetMainWnd()->SetWindowText(t_str);
//	}


	m_pre_AniCount = m_AniCount;

	DWORD dwTick = GetTickCount();
	if (dwTick-m_dwTick > (unsigned int)m_timegab)
	{
		m_dwTick = GetTickCount();
		//if(m_AniCount >= 6)//여자
		if(m_AniCount >= ANI_FRAME_COUNT - 1 && m_CharDrawDir) m_CharDrawDir = false; //남자			
		if(m_AniCount < 1 && !m_CharDrawDir) m_CharDrawDir = true;

		if(m_CharDrawDir) m_AniCount++;
		else m_AniCount--;
	}

//	Draw(120, 205);

}
void CItem_Inv::Draw(int tmp_x, int tmp_y)
{
	if( m_Draw_Order[4] == 1 && m_Draw_Order[3] >= 1)
	{
		//우석.. 9월 24일..
		Draw_Man(tmp_x,tmp_y);// 남자몸..여자문제..두있지..

		//if(m_OK_Flag[0] != 0) Draw_Helmet(tmp_x,tmp_y);// 머리
		if(m_OK_Flag[5] != 0) Draw_Boot(tmp_x,tmp_y);//신발
		if(m_OK_Flag[3] != 0) Draw_Armor(tmp_x,tmp_y);//갑옷 상의
		if(m_OK_Flag[4] != 0) Draw_Jean(tmp_x,tmp_y);//바지	
		if(m_OK_Flag[6] != 0) Draw_Arm(tmp_x,tmp_y);//검
		if(m_OK_Flag[7] != 0) Draw_Shield(tmp_x,tmp_y);//방패
	}
	else if(m_Draw_Order[4])
	{
		Draw_Man(tmp_x,tmp_y);// 남자몸..여자문제..두있지..
		if(m_OK_Flag[0] != 0) Draw_Helmet(tmp_x,tmp_y);// 머리
		if(m_OK_Flag[3] != 0) Draw_Armor(tmp_x,tmp_y);//갑옷 상의			
		if(m_OK_Flag[5] != 0) Draw_Boot(tmp_x,tmp_y);//신발
		if(m_OK_Flag[4] != 0) Draw_Jean(tmp_x,tmp_y);//바지						
		if(m_OK_Flag[6] != 0) Draw_Arm(tmp_x,tmp_y);//검
		if(m_OK_Flag[7] != 0) Draw_Shield(tmp_x,tmp_y);//방패
	}
	else if(m_Draw_Order[3])
	{
		switch(m_Draw_Order[3])
		{
		case 1:	Draw_Man(tmp_x,tmp_y);// 남자몸..여자문제..두있지..
				if(m_OK_Flag[0] != 0) Draw_Helmet(tmp_x,tmp_y);// 머리
				if(m_OK_Flag[4] != 0) Draw_Jean(tmp_x,tmp_y);//바지
				if(m_OK_Flag[5] != 0) Draw_Boot(tmp_x,tmp_y);//신발
				if(m_OK_Flag[3] != 0) Draw_Armor(tmp_x,tmp_y);//갑옷 상의			
				if(m_OK_Flag[6] != 0) Draw_Arm(tmp_x,tmp_y);//검
				if(m_OK_Flag[7] != 0) Draw_Shield(tmp_x,tmp_y);//방패
				break;
		case 2: Draw_Man(tmp_x,tmp_y);// 남자몸..여자문제..두있지..			
				if(m_OK_Flag[4] != 0) Draw_Jean(tmp_x,tmp_y);//바지
				if(m_OK_Flag[5] != 0) Draw_Boot(tmp_x,tmp_y);//신발
				if(m_OK_Flag[3] != 0) Draw_Armor(tmp_x,tmp_y);//갑옷 상의			
				if(m_OK_Flag[6] != 0) Draw_Arm(tmp_x,tmp_y);//검
				if(m_OK_Flag[7] != 0) Draw_Shield(tmp_x,tmp_y);//방패
				break;
		case 3: Draw_Man(tmp_x,tmp_y);// 남자몸..여자문제..두있지..
				if(m_OK_Flag[3] != 0) Draw_Armor(tmp_x,tmp_y);//갑옷 상의
				if(m_OK_Flag[4] != 0) Draw_Jean(tmp_x,tmp_y);//바지			
				if(m_OK_Flag[5] != 0) Draw_Boot(tmp_x,tmp_y);//신발
				if(m_OK_Flag[6] != 0) Draw_Arm(tmp_x,tmp_y);//검
				if(m_OK_Flag[7] != 0) Draw_Shield(tmp_x,tmp_y);//방패
				break;
		case 0:
		default: break;
		}
	}
	else
	{
		//우석.. 9월 24일..
		Draw_Man(tmp_x,tmp_y);// 남자몸..여자문제..두있지..
		if(m_OK_Flag[0] != 0) Draw_Helmet(tmp_x,tmp_y);// 머리
		if(m_OK_Flag[3] != 0) Draw_Armor(tmp_x,tmp_y);//갑옷 상의
		if(m_OK_Flag[4] != 0) Draw_Jean(tmp_x,tmp_y);//바지		
		
		if(m_OK_Flag[5] != 0) Draw_Boot(tmp_x,tmp_y);//신발
		if(m_OK_Flag[6] != 0) Draw_Arm(tmp_x,tmp_y);//검
		if(m_OK_Flag[7] != 0) Draw_Shield(tmp_x,tmp_y);//방패
	}

 	//우석.
	if(m_bLoginMode == false)
	{
		m_pre_AniCount = m_AniCount;

		DWORD dwTick = GetTickCount();
		if (dwTick-m_dwTick > (unsigned int)m_timegab)
		{
			m_AniCount ++;
			m_dwTick = GetTickCount();
			//if(m_AniCount >= 6)//여자
			if(m_AniCount >= 12)//남자
				m_AniCount=0;
		}
	}
}

void CItem_Inv::HairStyleDraw(int tmp_x, int tmp_y, int hairindex)
{
	int pre_anicount = m_AniCount;

	hairindex = hairindex * 10 + 101;

	DestoryTexSurface();
	Change_InvItem(0,hairindex);
	CreateTexture();

	m_AniCount = pre_anicount;
}

void CItem_Inv::HairDraw(int tmp_x, int tmp_y, int hairstyle, int hairindex)
{
	int pre_anicount = m_AniCount;

	hairindex = hairstyle * 10 + hairindex + 100;

	DestoryTexSurface();

	Change_InvItem(0,hairindex);
	CreateTexture();

	m_AniCount = pre_anicount;
}

void CItem_Inv::Draw_Helmet(int tmp_x, int tmp_y)
{
	int x,y;

	CDDSurface* pDestSurf;
	if(m_bLoginMode)
	{
		if(m_Gender == 1) { x = tmp_x; y = tmp_y - 150; } //여자..
		else { x = tmp_x + 3; y = tmp_y - 161; } //남자..
		
		pDestSurf = m_pTargetSurface[m_AniCount]; //우석. 9월 24일.
	}
	else
	{
		if(m_Gender == 1) {	x = 95 + tmp_x; y = 210 + tmp_y; } //여자..
		else { x = 98 + tmp_x; y = 199 + tmp_y; } //남자..

		pDestSurf = CChildView::DirectXMng.m_pDrawSurface; //우석. 9월 24일.
	}

	INV_SPR_DATA *spr_data;
	_SPR_LSP	 *spr;
	PWORD pV;
	int frame;

	spr_data = m_arrInv[INV_KIND_HELMET];
	if(spr_data == NULL)
	{
		spr = NULL;
		pV = NULL;
		TRACE("spl 데이타가 널이다...\n");
		return;
	}


	if(m_AniCount < 0 || m_AniCount >= m_aniInv.m_HAni.nDirQt * m_aniInv.m_AniData.nMaxFrm)
	{
		TRACE("애니메이션 범위를 벗어났다..\n");
		return;
	}
	frame = m_aniInv.m_AniData.pnFrms[m_AniCount];

	if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
	{
		TRACE("스프라이트 범위를 벗어났다..\n");
		return;
	}

	spr = &spr_data->m_arrSprData[frame];
	pV = spr_data->m_arrVData[frame];

	RECT rcV = spr->rcV;

	BltLSPNormal(pDestSurf, x, y, &rcV, pV);	
}

void CItem_Inv::Draw_Armor(int tmp_x, int tmp_y)
{
	int x,y;

	CDDSurface* pDestSurf;
	if(m_bLoginMode)
	{
		if(m_Gender == 1) { x = tmp_x; y = tmp_y - 110; } //여자...
		else { x = tmp_x + 8; y = tmp_y - 91; }

		pDestSurf = m_pTargetSurface[m_AniCount]; //우석. 9월 24일.
	}
	else
	{
		if(m_Gender == 1) {	x = 95 + tmp_x;	y = 250 + tmp_y; } // 여자라면..
		else { x = 103 + tmp_x; y = 269 + tmp_y; }
		pDestSurf = CChildView::DirectXMng.m_pDrawSurface; //우석. 9월 24일.
	}

	INV_SPR_DATA *spr_data;
	_SPR_LSP	 *spr;
	PWORD pV;
	int frame;

	spr_data = m_arrInv[INV_KIND_ARMOR];
	if(spr_data == NULL)
	{
		spr = NULL;
		pV = NULL;
		TRACE("spl 데이타가 널이다...\n");
		return;
	}

	if(m_AniCount < 0 || m_AniCount >= m_aniInv.m_HAni.nDirQt * m_aniInv.m_AniData.nMaxFrm)
	{
		TRACE("애니메이션 범위를 벗어났다..\n");
		return;
	}
	frame = m_aniInv.m_AniData.pnFrms[m_AniCount];
	if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
	{
		TRACE("스프라이트 범위를 벗어났다..\n");
		return;
	}

	spr = &spr_data->m_arrSprData[frame];
	pV = spr_data->m_arrVData[frame];

	RECT rcV = spr->rcV;

	BltLSPNormal(pDestSurf, x, y, &rcV, pV);	
}

void CItem_Inv::Draw_Jean(int tmp_x, int tmp_y)
{
	int x,y;

	CDDSurface* pDestSurf;
	if(m_bLoginMode) 
	{
		if(m_Gender == 1) { x = tmp_x ; y = tmp_y; } //여자..
		else { x = tmp_x + 9 ; y = tmp_y - 60; }

		pDestSurf = m_pTargetSurface[m_AniCount]; //우석. 9월 24일.
	}
	else
	{
		if(m_Gender == 1) { x = 95 + tmp_x; y = 360 + tmp_y; } // 여자라면..
		else { x = 104 + tmp_x; y = 300 + tmp_y; }
		pDestSurf = CChildView::DirectXMng.m_pDrawSurface; //우석. 9월 24일.
	}

	INV_SPR_DATA *spr_data;
	_SPR_LSP	 *spr;
	PWORD pV;
	int frame;

	spr_data = m_arrInv[INV_KIND_JEAN];
	if(spr_data == NULL)
	{
		spr = NULL;
		pV = NULL;
		TRACE("spl 데이타가 널이다...\n");
		return;
	}

	if(m_AniCount < 0 || m_AniCount >= m_aniInv.m_HAni.nDirQt * m_aniInv.m_AniData.nMaxFrm)
	{
		TRACE("애니메이션 범위를 벗어났다..\n");
		return;
	}
	frame = m_aniInv.m_AniData.pnFrms[m_AniCount];
	if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
	{
		TRACE("스프라이트 범위를 벗어났다..\n");
		return;
	}

	spr = &spr_data->m_arrSprData[frame];
	pV = spr_data->m_arrVData[frame];

	RECT rcV = spr->rcV;

	BltLSPNormal(pDestSurf, x, y, &rcV, pV);	
}

void CItem_Inv::Draw_Boot(int tmp_x, int tmp_y)
{
	int x,y;
	
	CDDSurface* pDestSurf;
	if(m_bLoginMode)
	{
		if(m_Gender == 1) {x = tmp_x; y = tmp_y; } //여자..
		else {x = tmp_x; y = tmp_y - 3; } //남자..

		pDestSurf = m_pTargetSurface[m_AniCount]; //우석. 9월 24일.
	}
	else
	{
		if(m_Gender == 1) { x = 95 + tmp_x; y = 360 + tmp_y; } //여자
        else { x = 95 + tmp_x; y = 356 + tmp_y; }
		pDestSurf = CChildView::DirectXMng.m_pDrawSurface; //우석. 9월 24일.
	}

	INV_SPR_DATA *spr_data;
	_SPR_LSP	 *spr;
	PWORD pV;
	int frame;

	spr_data = m_arrInv[INV_KIND_BOOT];
	if(spr_data == NULL)
	{
		spr = NULL;
		pV = NULL;
		TRACE("spl 데이타가 널이다...\n");
		return;
	}

	if(m_AniCount < 0 || m_AniCount >= m_aniInv.m_HAni.nDirQt * m_aniInv.m_AniData.nMaxFrm)
	{
		TRACE("애니메이션 범위를 벗어났다..\n");
		return;
	}
	frame = m_aniInv.m_AniData.pnFrms[m_AniCount];
	if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
	{
		TRACE("스프라이트 범위를 벗어났다..\n");
		return;
	}

	spr = &spr_data->m_arrSprData[frame];
	pV = spr_data->m_arrVData[frame];

	RECT rcV = spr->rcV;

	BltLSPNormal(pDestSurf, x, y, &rcV, pV);	
}

void CItem_Inv::Draw_Arm(int tmp_x, int tmp_y)
{
	int x,y;
	
	CDDSurface* pDestSurf;
	if(m_bLoginMode)
	{
		if(m_Gender == 1) { x = tmp_x; y = tmp_y - 60; } // 여자라면..
		else { x = tmp_x + 33; y = tmp_y - 65; }

		pDestSurf = m_pTargetSurface[m_AniCount]; //우석. 9월 24일.
	}
	else
	{
		if(m_Gender == 1) {  x = 95 + tmp_x; y = 300 + tmp_y;  } // 여자라면..
		else { x = 128 + tmp_x; y = 295 + tmp_y; }

	    
		pDestSurf = CChildView::DirectXMng.m_pDrawSurface; //우석. 9월 24일.
	}

	INV_SPR_DATA *spr_data;
	_SPR_LSP	 *spr;
	PWORD pV;
	int frame;

	spr_data = m_arrInv[INV_KIND_ARMS];
	if(spr_data == NULL)
	{
		spr = NULL;
		pV = NULL;
		TRACE("spl 데이타가 널이다...\n");
		return;
	}

	if(m_AniCount < 0 || m_AniCount >= m_aniInv.m_HAni.nDirQt * m_aniInv.m_AniData.nMaxFrm)
	{
		TRACE("애니메이션 범위를 벗어났다..\n");
		return;
	}
	frame = m_aniInv.m_AniData.pnFrms[m_AniCount];
	if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
	{
		TRACE("스프라이트 범위를 벗어났다..\n");
		return;
	}

	spr = &spr_data->m_arrSprData[frame];
	pV = spr_data->m_arrVData[frame];

	RECT rcV = spr->rcV;

	BltLSPNormal(pDestSurf, x, y, &rcV, pV);		
}

void CItem_Inv::Draw_Shield(int tmp_x, int tmp_y)
{
	int x,y;
	
	CDDSurface* pDestSurf;
	if(m_bLoginMode)
	{
		if(m_Gender == 1) { x = tmp_x; y = tmp_y - 50; } //여자.
		else { x = tmp_x - 17; y = tmp_y - 85; } // 남자.

		pDestSurf = m_pTargetSurface[m_AniCount]; //우석. 9월 24일.
	}
	else
	{
		if(m_Gender == 1) { x = 95 + tmp_x; y = 310 + tmp_y; } //여자.
		else { x = 78 + tmp_x; y = 275 + tmp_y; } //남자
		pDestSurf = CChildView::DirectXMng.m_pDrawSurface; //우석. 9월 24일.
	}

	INV_SPR_DATA *spr_data;
	_SPR_LSP	 *spr;
	PWORD pV;
	int frame;

	spr_data = m_arrInv[INV_KIND_SHIELD];
	if(spr_data == NULL)
	{
		spr = NULL;
		pV = NULL;
		TRACE("spl 데이타가 널이다...\n");
		return;
	}

	if(m_AniCount < 0 || m_AniCount >= m_aniInv.m_HAni.nDirQt * m_aniInv.m_AniData.nMaxFrm)
	{
		TRACE("애니메이션 범위를 벗어났다..\n");
		return;
	}
	frame = m_aniInv.m_AniData.pnFrms[m_AniCount];
	if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
	{
		TRACE("스프라이트 범위를 벗어났다..\n");
		return;
	}

	spr = &spr_data->m_arrSprData[frame];
	pV = spr_data->m_arrVData[frame];

	RECT rcV = spr->rcV;

	BltLSPNormal(pDestSurf, x, y, &rcV, pV);	
}

void CItem_Inv::Draw_Man(int tmp_x, int tmp_y)
{
	int x,y;
	
	CDDSurface* pDestSurf;
	if(m_bLoginMode)
	{
		x = tmp_x; y = tmp_y;
		pDestSurf = m_pTargetSurface[m_AniCount]; //우석. 9월 24일.
	}
	else
	{
		x = 95 + tmp_x; y = 360 + tmp_y;
		pDestSurf = CChildView::DirectXMng.m_pDrawSurface; //우석. 9월 24일.
	}


	INV_SPR_DATA *spr_data;
	_SPR_LSP	 *spr;
	PWORD pV;
	int frame;

	spr_data = m_arrInv[INV_KIND_BODY];
	if(spr_data == NULL)
	{
		spr = NULL;
		pV = NULL;
		TRACE("spl 데이타가 널이다...\n");
		return;
	}

	if(m_AniCount < 0 || m_AniCount >= m_aniInv.m_HAni.nDirQt * m_aniInv.m_AniData.nMaxFrm)
	{
		TRACE("애니메이션 범위를 벗어났다..\n");
		return;
	}
	frame = m_aniInv.m_AniData.pnFrms[m_AniCount];
	if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
	{
		TRACE("스프라이트 범위를 벗어났다..\n");
		return;
	}

	spr = &spr_data->m_arrSprData[frame];
	pV = spr_data->m_arrVData[frame];

	RECT rcV = spr->rcV;

	BltLSPNormal(pDestSurf, x, y, &rcV, pV);	
}

void CItem_Inv::BltLSPNormal(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = 800;
	int ScreenY = 600;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = FALSE;
	rcClip = rc;

	if(rc.left < 0)
	{ 
		rcClip.left = 0;
		bUseXClip = TRUE;
	}
	else if(rc.right > ScreenX)
	{ 
		rcClip.right = ScreenX;
		bUseXClip = TRUE;
	}
	if(rc.top < 0)
	{
		rcClip.top = 0;
	}
	else if(rc.bottom > ScreenY)
	{
		rcClip.bottom = ScreenY;
	}

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이

	const WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	if( NULL == pSurface->Lock() ) return;	
	DDSURFACEDESC2& ddsd = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsd.lpSurface+rcClip.left*2+rcClip.top*ddsd.lPitch);
	int pitchDest = ddsd.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left; 
	int height = rcClip.bottom - rcClip.top;

	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				CUser::PutPixelNormal(pDest, pSrc, nCopyCount);

			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				CUser::PutPixelNormal(pDest, pSrc, nCopyCount);
				// }

			}
		}
	}
	pSurface->Unlock();
}
