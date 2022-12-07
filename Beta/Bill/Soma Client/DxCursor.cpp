// DxCursor.cpp: implementation of the CDxCursor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "DxCursor.h"
#include "DManage/BB_Direct.h"
#include "ChildView.h"
#include "User.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDxCursor::CDxCursor()
{
	m_dwTick = 0;
	m_iCursorIndex = 0;
	m_iSpriteIndex = 0;
	m_ptHotSpot = CPoint(0,0);
	for(int i = 0; i < CURSOR_COUNT; i++) m_arrCursor[i] = NULL;
}

CDxCursor::~CDxCursor()
{
	RemoveAllData();
}

void CDxCursor::RemoveAllData()
{
	for (int i = 0; i < CURSOR_COUNT; i++)
	{
		delete m_arrCursor[i];
		m_arrCursor[i] = NULL;
	}
}

void CDxCursor::SetCursorIndex(int index)
{
	if(m_iCursorIndex == index) return;

	m_iCursorIndex = index;
	m_iSpriteIndex = 0;
}

void CDxCursor::Draw(CDDSurface *pSurface, CPoint ptPos)
{
//	ptPos.x -= m_ptHotSpot.x;
//	ptPos.y -= m_ptHotSpot.y;

	SPR_DATA *spr_data;
	_SPR_LSP* spr;
	PWORD pV;

	spr_data = m_arrCursor[m_iCursorIndex];
	if(spr_data == NULL)
	{
		spr = NULL;
		pV = NULL;
		return;
	}

	if(m_iSpriteIndex == spr_data->m_AniData.nMaxFrm) m_iSpriteIndex = 0;

	int frame;
	DWORD dwTick = GetTickCount();
	BOOL bFrameChange = FALSE;
	// 시간이 지나면 다음 프레임 돌려줌..
	if (dwTick > m_dwTick + 1000 / spr_data->m_AniData.fSpd)
	{
		m_dwTick = dwTick;
		if(m_iSpriteIndex < 0 || m_iSpriteIndex >= spr_data->m_HAni.nDirQt * spr_data->m_AniData.nMaxFrm)
		{
			return;
		}
		frame = spr_data->m_AniData.pnFrms[m_iSpriteIndex];
		if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
		{
			return;
		}
		m_iSpriteIndex++;
	}
	else frame = spr_data->m_AniData.pnFrms[m_iSpriteIndex];

	spr = &spr_data->m_arrSprData[frame];
	pV = spr_data->m_arrVData[frame];

	RECT rcV = spr->rcV;

	BltLSPNormal(pSurface, ptPos.x, ptPos.y, &rcV, pV);
}

static char CURSOR_NAME[CURSOR_COUNT][7] = {"normal", "attack", "pickup"};

void CDxCursor::InitCursor()
{
	const CString szExePath = CDirectDraw::GetExecutionPath();
	CString szSplFileName, szAniFileName;
	int i=0;

	int Count = 0;
	UINT count;
	CFile spl_file, ani_file;
	WORD *word;
	int index = 0;
	SPR_DATA *spr_data;

	for(int j = 0; j < CURSOR_COUNT; j++)
	{
		szSplFileName.Format("%sNRes\\%s.spl", (LPCTSTR)szExePath, (LPCTSTR)CURSOR_NAME[j]);
		szAniFileName.Format("%sNRes\\%s.ani", (LPCTSTR)szExePath, (LPCTSTR)CURSOR_NAME[j]);
		
		if(spl_file.Open(szSplFileName, CFile::modeRead) == FALSE)
		{
			continue;
		}
		if(ani_file.Open(szAniFileName, CFile::modeRead) == FALSE)
		{
			spl_file.Close();
			continue;
		}
		//Spl 파일 읽기 (포맷은 Spr과 다름)
		//우석.. 10월 15일..
		
		spr_data = new SPR_DATA;
		
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
		
		// 애니메이션
		ani_file.Read((PVOID)&spr_data->m_HAni, sizeof(_ANI_HEADER));
		ani_file.Read((PVOID)&spr_data->m_AniData, sizeof(_ANI_DATA));
		spr_data->m_AniData.pnFrms = NULL;
		if(spr_data->m_AniData.nMaxFrm > 0)
		{
			int nQt = spr_data->m_HAni.nDirQt * spr_data->m_AniData.nMaxFrm; // 갯수 계산 = 방향 * 프레임갯수
			spr_data->m_AniData.pnFrms = new short int[nQt]; // 갯수 만큼 생성
			ani_file.Read(spr_data->m_AniData.pnFrms, nQt*2); // 갯수 만큼 읽는다.
		}
		
		m_arrCursor[j] = spr_data;
		
		spl_file.Close();
		ani_file.Close();
	}
}

void CDxCursor::BltLSPNormal(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data)
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
