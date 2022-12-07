// AniObjTool.cpp: implementation of the CAniObjTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AniObjTool.h"
#include "DDEngineTool4.h"

#include "GlobalVar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAniObjTool::CAniObjTool()
{
	m_lpDDS = NULL;
	m_pRGBTbls = NULL;
	MAKE_ZERO(m_SprExs);

	AniCtrlAlloc(MAX_ANI_CTRL/2);
}

CAniObjTool::~CAniObjTool()
{
	RELEASE_SAFE(m_lpDDS);
	if(m_pRGBTbls) { delete [] m_pRGBTbls; m_pRGBTbls = NULL; }
}

void CAniObjTool::SprExGetData(int index, _SPR_TOOL *pSprDataEx)
{
	if(index < 0 || index >= m_HSpr.nQt || index >= MAX_SPR || pSprDataEx == NULL)
	{
		MAKE_ZERO(*pSprDataEx);
		return;
	}
	*pSprDataEx = m_SprExs[index];
}

void CAniObjTool::SprExSetData(int index, _SPR_TOOL *pSprDataEx)
{
	if(index < 0 || index >= m_HSpr.nQt || pSprDataEx == NULL) return;
	m_SprExs[index] = *pSprDataEx;
}

void CAniObjTool::SprNew()
{
	SprRelease();
	MAKE_ZERO(m_SprExs);
	RELEASE_SAFE(m_lpDDS);
	if(m_pRGBTbls) { delete [] m_pRGBTbls; m_pRGBTbls = NULL; }

	m_nSprWorkQt = 0;
}

void CAniObjTool::AniNew()
{
	AniRelease();
	m_nCurAnis[0] = 0;
	m_nCurDirs[0] = 0;
	m_nCurFrms[0] = 0;
}
/*
void CAniObjTool::Load2(const char* szFileName, int nType)
{
	if(nType != SAVE_ANI) if(m_pRGBTbls) { delete [] m_pRGBTbls; m_pRGBTbls = NULL; }

	////////////////////////////////////////////////////////////////////////////////
	// 스프라이트...................................................................
	if(nType == SAVE_SPR_NORMAL) // 단순 구조
	{
		int i;
		CFile file; if(file.Open(szFileName, CFile::modeRead) == FALSE) { MessageBox(NULL, szFileName, "파일 열기 실패", MB_OK); return; }
		file.Read(&m_HSpr, sizeof(_SPR_HEADER)); // 헤더 읽기
		
		// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
		for(i = 0; i < m_HSpr.nQt; i++)
		{
			file.Read(&m_SprExs[i], sizeof(_SPR_TOOL));
		}

		_SPR_NORMAL spr;
		for(i = 0; i < m_HSpr.nQt; i++)
		{
			file.Read(&spr, sizeof(_SPR_NORMAL));
			ConvSprNormalToSprToolEx(&spr, &m_SprExs[i]);
		}

		RELEASE_SAFE(m_lpdds);
		g_pTEng->ChangePath("BMP");
		m_lpdds = g_pTEng->LoadBMP(m_HSpr.szBMPFN, FALSE, m_HSpr.SrcCK); // 서피스 로딩...
		RECT rcT = g_pTEng->GetSurfaceSize(m_lpdds); // 헤더 세팅
		m_HSpr.nWidth = rcT.right; m_HSpr.nHeight = rcT.bottom;
		file.Close();
	}
	else if(nType == SAVE_SPR_LSP)
	{
		CFile file; if(file.Open(szFileName, CFile::modeRead) == FALSE) { MessageBox(NULL, szFileName, "파일 열기 실패", MB_OK); return; }

		file.Read(&m_HSpr, sizeof(_SPR_HEADER)); // 스프라이트 헤더 읽기.
		
		RELEASE_SAFE(m_lpdds);
		g_pTEng->ChangePath("BMP");
		m_lpdds = g_pTEng->LoadBMP(m_HSpr.szBMPFN, FALSE, m_HSpr.SrcCK); // 서피스 로딩...
		if(m_lpdds)
		{
			int i;
			// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Read(&m_SprExs[i], sizeof(_SPR_TOOL));
			}

			RECT rcT = g_pTEng->GetSurfaceSize(m_lpdds); // 헤더 세팅
			m_HSpr.nWidth = rcT.right; m_HSpr.nHeight = rcT.bottom;
		}
		else // 비트맵로딩에 실패하면 자동 추출된 파일이다..
		{
			int i = 0;
			
			_SPR_HEADER m_HSpr = m_HSpr; // 해제 하기전에 헤더만 기억해 놓는다.
			SprRelease();
			m_HSpr = m_HSpr; // 헤더를 원래대로 돌려 놓는다.

			file.Seek(sizeof(_SPR_TOOL)*m_HSpr.nQt, CFile::current); // 툴 데이터 만큼 건너뛰고..

			SprExAlloc(m_HSpr.nQt);
			_SPR_LSP spr;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Read(&spr, sizeof(_SPR_LSP));
				m_pSprExs[i]->rcV1 = spr.rcV;
			}
			
			int nPF = g_pTEng->GetPixelFormat();
			for(i = 0; i < m_HSpr.nQt; i++) 	// 압축 스프라이트 데이터
			{
				file.Read(&m_pLSPExs[i]->nV1, 4); // 캐릭터 영역 - Part 1
				if(m_pLSPExs[i]->nV1 > 640000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
				if(m_pLSPExs[i]->nV1 > 0)
				{
					m_pLSPExs[i]->pV1 = new WORD[m_pLSPExs[i]->nV1/2];
					file.Read(m_pLSPExs[i]->pV1, m_pLSPExs[i]->nV1);
					if(nPF == 555) Decode2_555(m_pLSPExs[i]->pV1);
				}
			}
		}
		file.Close();
	}
	else if(nType == SAVE_SPR_LSP_EX)
	{
		CFile file; if(file.Open(szFileName, CFile::modeRead) == FALSE) { MessageBox(NULL, szFileName, "파일 열기 실패", MB_OK); return; }

		file.Read(&m_HSpr, sizeof(_SPR_HEADER)); // 스프라이트 헤더 읽기.

		RELEASE_SAFE(m_lpdds);
		g_pTEng->ChangePath("BMP");
		m_lpdds = g_pTEng->LoadBMP(m_HSpr.szBMPFN, FALSE, m_HSpr.SrcCK); // 서피스 로딩...
		
		if(m_lpdds)
		{
			// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
			int i;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Read(&m_SprExs[i], sizeof(_SPR_TOOL));
			}
			RECT rcT = g_pTEng->GetSurfaceSize(m_lpdds); // 헤더 세팅
			m_HSpr.nWidth = rcT.right; m_HSpr.nHeight = rcT.bottom;

		}
		else // 비트맵 파일 이름이 없으면 자동 추출된 스프라이트이다..
		{
			int i = 0;

			_SPR_HEADER m_HSpr = m_HSpr; // 해제 하기전에 헤더만 기억해 놓는다.
			SprRelease();
			m_HSpr = m_HSpr; // 헤더를 원래대로 돌려 놓는다.
			
			file.Seek(sizeof(_SPR_TOOL)*m_HSpr.nQt, CFile::current); // 툴 데이터 만큼 건너뛰고..

			SprExAlloc(m_HSpr.nQt);
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Read(m_pSprExs[i], sizeof(_SPR_LSP_EX));
			}
			
			int nPF = g_pTEng->GetPixelFormat();
			for(i = 0; i < m_HSpr.nQt; i++) 	// 압축 스프라이트 데이터
			{
				file.Read(&m_pLSPExs[i]->nV1, 4); // 캐릭터 영역 - Part 1
				if(m_pLSPExs[i]->nV1 > 640000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
				if(m_pLSPExs[i]->nV1 > 0)
				{
					m_pLSPExs[i]->pV1 = new WORD[m_pLSPExs[i]->nV1/2];
					file.Read(m_pLSPExs[i]->pV1, m_pLSPExs[i]->nV1);
					if(nPF == 555) Decode2_555(m_pLSPExs[i]->pV1);
				}
				file.Read(&m_pLSPExs[i]->nV2, 4); // 캐릭터 영역 - Part 2
				if(m_pLSPExs[i]->nV2 > 640000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
				if(m_pLSPExs[i]->nV2 > 0)
				{
					m_pLSPExs[i]->pV2 = new WORD[m_pLSPExs[i]->nV2/2];
					file.Read(m_pLSPExs[i]->pV2, m_pLSPExs[i]->nV2);
					if(nPF == 555) Decode2_555(m_pLSPExs[i]->pV2);
				}
				file.Read(&m_pLSPExs[i]->nSH, 4); // 그림자 영역
				if(m_pLSPExs[i]->nSH > 640000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
				if(m_pLSPExs[i]->nSH > 0)
				{
					m_pLSPExs[i]->pSH = new WORD[m_pLSPExs[i]->nSH/2];
					file.Read(m_pLSPExs[i]->pSH, m_pLSPExs[i]->nSH);
					if(nPF == 555) Decode2_555(m_pLSPExs[i]->pSH);
				}
				file.Read(&m_pLSPExs[i]->nFX, 4); // 공격 영역
				if(m_pLSPExs[i]->nFX > 640000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
				if(m_pLSPExs[i]->nFX > 0)
				{
					m_pLSPExs[i]->pFX = new WORD[m_pLSPExs[i]->nFX/2];
					file.Read(m_pLSPExs[i]->pFX, m_pLSPExs[i]->nFX);
					if(nPF == 555) Decode2_555(m_pLSPExs[i]->pFX);
				}
				for(int j = 0; j < MAX_PART; j++) // 파트 영역
				{
					file.Read(&m_pLSPExs[i]->nPs[j], 4);
					if(m_pLSPExs[i]->nPs[j] > 640000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
					if(m_pLSPExs[i]->nPs[j] > 0)
					{
						m_pLSPExs[i]->pPs[j] = new WORD[m_pLSPExs[i]->nPs[j]/2];
						file.Read(m_pLSPExs[i]->pPs[j], m_pLSPExs[i]->nPs[j]);
						if(nPF == 555) Decode2_555(m_pLSPExs[i]->pPs[j]);
					}
				}
			}
		}
		file.Close();
	}
	else if(nType == SAVE_SPR_LSP_INDEXED) // 8비트 인덱스 스프라이트
	{
		CFile file; if(file.Open(szFileName, CFile::modeRead) == FALSE) { MessageBox(NULL, szFileName, "파일 열기 실패", MB_OK); return; }

		file.Read(&m_HSpr, sizeof(_SPR_HEADER)); // 스프라이트 헤더 읽기.
		
		RELEASE_SAFE(m_lpdds);
		g_pTEng->ChangePath("BMP");
		m_lpdds = g_pTEng->LoadBMP(m_HSpr.szBMPFN, FALSE, m_HSpr.SrcCK); // 서피스 로딩...
		if(m_lpdds)
		{
			int i;
			// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Read(&m_SprExs[i], sizeof(_SPR_TOOL));
			}

			RECT rcT = g_pTEng->GetSurfaceSize(m_lpdds); // 헤더 세팅
			m_HSpr.nWidth = rcT.right; m_HSpr.nHeight = rcT.bottom;
		}
		else // 비트맵로딩에 실패하면 자동 추출된 파일이다..
		{
			int i = 0;
			
			_SPR_HEADER m_HSpr = m_HSpr; // 해제 하기전에 헤더만 기억해 놓는다.
			SprRelease();
			m_HSpr = m_HSpr; // 헤더를 원래대로 돌려 놓는다.

			SprExAlloc(m_HSpr.nQt);

			file.Seek(sizeof(_SPR_TOOL)*m_HSpr.nQt, CFile::current); // 툴 데이터 만큼 건너뛰고..

			// 스프라이트 팔레트 영역
			if(m_pRGBTbls == NULL) m_pRGBTbls = new RGBQUAD[256];
			file.Read(m_pRGBTbls, sizeof(RGBQUAD)*256); // 컬러 테이블 영역

			_SPR_LSP spr;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Read(&spr, sizeof(_SPR_LSP));
				m_pSprExs[i]->rcV1 = spr.rcV;
			}
			
			int nPF = g_pTEng->GetPixelFormat();

			for(i = 0; i < m_HSpr.nQt; i++) 	// 압축 스프라이트 데이터
			{
				file.Read(&m_pLSPExs[i]->nV1, 4); // 캐릭터 영역 - Part 1
				if(m_pLSPExs[i]->nV1 > 640000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
				if(m_pLSPExs[i]->nV1 > 0)
				{
					m_pLSPExs[i]->pV1 = new WORD[m_pLSPExs[i]->nV1/2];
					WORD* pTemp = m_pLSPExs[i]->pV1;
					WORD wLineCount;
					file.Read(&wLineCount, 2); *pTemp = wLineCount; pTemp++;// 라인 카운트..
					for(int j = 0; j < wLineCount; j++)
					{
						WORD wNodeCount;
						file.Read(&wNodeCount, 2); *pTemp = wNodeCount; *pTemp++;// 노드 카운트..
						for(int k =0; k < wNodeCount; k++)
						{
							WORD wSkip, wPixelCount;
							file.Read(&wSkip, 2); *pTemp = wSkip; pTemp++;// 건너뛰는 픽셀 카운트..
							file.Read(&wPixelCount, 2); *pTemp = wPixelCount; pTemp++;// 유효 픽셀 카운트..
							BYTE* pbyTemp = new BYTE[wPixelCount];
							file.Read(pbyTemp, wPixelCount); // 8비트 픽셀을 읽어온다...
							Convert8PixelTo16Pixel(pbyTemp, pTemp, m_pRGBTbls, wPixelCount); pTemp+= wPixelCount;
							delete [] pbyTemp; pbyTemp = NULL;
						}
					}
					if(nPF == 555) Decode2_555(m_pLSPExs[i]->pV1);
				}
			}
		}
		file.Close();
	}
	// 스프라이트...................................................................
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// 에니메이션...................................................................
	else if(nType == SAVE_ANI)
	{
		CFile file; if(file.Open(szFileName, CFile::modeRead) == FALSE) { MessageBox(NULL, szFileName, "파일 열기 실패", MB_OK); return; }

		file.Read(&m_HAni, sizeof(_ANI_HEADER)); // 에니메이션 헤더 읽기
		AniAlloc(m_HAni.nQt); // 할당..
		int i;
		for(i = 0; i < m_HAni.nQt; i++) // 에니메이션 데이터 읽기
		{
			file.Read(m_pAnis[i], sizeof(_ANI_DATA));
		}

		file.Close();
	}
	// 에니메이션...................................................................
	////////////////////////////////////////////////////////////////////////////////
}
*/
/*
void CAniObjTool::Save(const char* szFileName, int nType)
{
////////////////////////////////////////////////////////////////////////////////
// 스프라이트...................................................................
	// 스프라이트 헤더 저장.
	if(nType == SAVE_SPR_NORMAL) // 보통 스프라이트
	{
		CFile file; file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);

		int i;
		wsprintf(m_HSpr.szID, "단순 Sprite : %s | Sprite - %d", szFileName, m_HSpr.nQt); // 스프라이트 저장
		file.Write(&m_HSpr, sizeof(_SPR_HEADER));
		
		// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
		for(i = 0; i < m_HSpr.nQt; i++)
		{
			file.Write(&m_SprExs[i], sizeof(_SPR_TOOL));
		}
		
		// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
		_SPR_NORMAL spr;
		for(i = 0; i < m_HSpr.nQt; i++)
		{
			ConvSprToolExToSprNormal(&m_SprExs[i], &spr);
			file.Write(&spr, sizeof(_SPR_NORMAL));
		}
		file.Close();
	}
	else if(nType == SAVE_SPR_LSP) // 단순구조 LSP 스프라이트
	{
		CFile file; file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);
	
		int i;
		wsprintf(m_HSpr.szID, "단순구조 LSP Sprite : %s | Sprite - %d", szFileName, m_HSpr.nQt); // 스프라이트 저장
		file.Write(&m_HSpr, sizeof(_SPR_HEADER));
		
		// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
		for(i = 0; i < m_HSpr.nQt; i++)
		{
			file.Write(&m_SprExs[i], sizeof(_SPR_TOOL));
		}
		
		if(m_lpdds)
		{
			// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
			_SPR_LSP_EX spr;
			_SPR_LSP spr2;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				ConvSprToolExToSprLSPEx(&m_SprExs[i], &spr); // 툴에서 쓰는 확장 스프라이트를 게임에서 쓰는 스프라이트로 변환
				spr2.rcV = spr.rcV1;
				file.Write(&spr2, sizeof(_SPR_LSP));
			}

			// 압축 스프라이트 데이터
			WORD* pwBuff = NULL;
			CRect rc;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				_SPR_TOOL sprex = m_SprExs[i];

				m_pLSPExs[i]->nV1 = 0;
				rc = sprex.rcV1; // 캐릭터 보이는 영역 - Part 1
				if((rc.right-rc.left)*(rc.bottom-rc.top) > 0) // 영역이 있어야만 처리한다.
				{
					g_pTEng->EncodeToLSP(m_lpdds, m_HSpr.SrcCK, rc, &m_pLSPExs[i]->nV1, &pwBuff);
					file.Write(&m_pLSPExs[i]->nV1, 4);
					file.Write(pwBuff, m_pLSPExs[i]->nV1);
					if(pwBuff) delete [] pwBuff; pwBuff = NULL;
				}
				else // 영역이 없으면 0 만 써놓는다.
				{
					file.Write(&m_pLSPExs[i]->nV1, 4);
				}
			}
		}
		else
		{
			// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
			_SPR_LSP spr;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				spr.rcV = m_pSprExs[i]->rcV1;
				file.Write(&spr, sizeof(_SPR_LSP));
			}

			// 압축 스프라이트 데이터
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Write(&m_pLSPExs[i]->nV1, 4);
				if(m_pLSPExs[i]->nV1 > 0) file.Write(m_pLSPExs[i]->pV1, m_pLSPExs[i]->nV1);
			}
		}
		file.Close();
	}
	else if(nType == SAVE_SPR_LSP_EX) // 복합구조 LSP 스프라이트
	{
		CFile file; file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);
	
		int i;
		wsprintf(m_HSpr.szID, "소마신화전기 8방향 복합 Sprite : %s | Sprite - %d", szFileName, m_HSpr.nQt); // 스프라이트 저장
		file.Write(&m_HSpr, sizeof(_SPR_HEADER));
		
		// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
		for(i = 0; i < m_HSpr.nQt; i++)
		{
			file.Write(&m_SprExs[i], sizeof(_SPR_TOOL));
		}
		
		if(m_lpdds)
		{
			// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
			_SPR_LSP_EX spr;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				// 툴에서 쓰는 확장 스프라이트를 게임에서 쓰는 스프라이트로 변환
				ConvSprToolExToSprLSPEx(&m_SprExs[i], &spr);
				file.Write(&spr, sizeof(_SPR_LSP_EX));
			}

			// 압축 스프라이트 데이터
			WORD* pwBuff = NULL;
			CRect rc;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				_SPR_TOOL sprex = m_SprExs[i];

				m_pLSPExs[i]->nV1 = 0;
				rc = sprex.rcV1; // 캐릭터 보이는 영역 - Part 1
				if((rc.right-rc.left)*(rc.bottom-rc.top) > 0) // 영역이 있어야만 처리한다.
				{
					g_pTEng->EncodeToLSP(m_lpdds, m_HSpr.SrcCK, rc, &m_pLSPExs[i]->nV1, &pwBuff);
					file.Write(&m_pLSPExs[i]->nV1, 4);
					file.Write(pwBuff, m_pLSPExs[i]->nV1);
					if(pwBuff) delete [] pwBuff; pwBuff = NULL;
				}
				else // 영역이 없으면 0 만 써놓는다.
				{
					file.Write(&m_pLSPExs[i]->nV1, 4);
				}

				m_pLSPExs[i]->nV2 = 0;
				rc = sprex.rcV2; // 캐릭터 보이는 영역 - Part 2
				if((rc.right-rc.left)*(rc.bottom-rc.top) > 0) // 영역이 있어야만 처리한다.
				{
					g_pTEng->EncodeToLSP(m_lpdds, m_HSpr.SrcCK, rc, &m_pLSPExs[i]->nV2, &pwBuff);
					file.Write(&m_pLSPExs[i]->nV2, 4);
					file.Write(pwBuff, m_pLSPExs[i]->nV2);
					if(pwBuff) delete [] pwBuff; pwBuff = NULL;
				}
				else // 영역이 없으면 0 만 써놓는다.
				{
					file.Write(&m_pLSPExs[i]->nV2, 4);
				}

				m_pLSPExs[i]->nSH = 0;
				rc = sprex.rcSH; // 그림자 영역
				if((rc.right-rc.left)*(rc.bottom-rc.top) > 0) // 영역이 있어야만 처리한다.
				{
					g_pTEng->EncodeToLSP(m_lpdds, m_HSpr.SrcCK, rc, &m_pLSPExs[i]->nSH, &pwBuff);
					file.Write(&m_pLSPExs[i]->nSH, 4);
					file.Write(pwBuff, m_pLSPExs[i]->nSH);
					if(pwBuff) delete [] pwBuff; pwBuff = NULL;
				}
				else // 영역이 없으면 0 만 써놓는다.
				{
					file.Write(&m_pLSPExs[i]->nSH, 4);
				}

				m_pLSPExs[i]->nFX = 0;
				rc = sprex.rcFX; // 공격 효과 영역
				if((rc.right-rc.left)*(rc.bottom-rc.top) > 0) // 영역이 있어야만 처리한다.
				{
					g_pTEng->EncodeToLSP(m_lpdds, m_HSpr.SrcCK, rc, &m_pLSPExs[i]->nFX, &pwBuff);
					file.Write(&m_pLSPExs[i]->nFX, 4);
					file.Write(pwBuff, m_pLSPExs[i]->nFX);
					if(pwBuff) delete [] pwBuff; pwBuff = NULL;
				}
				else // 영역이 없으면 0 만 써놓는다.
				{
					file.Write(&m_pLSPExs[i]->nFX, 4);
				}

				for(int j = 0; j < MAX_PART; j++) // 무기, 아이템등 파트 영역
				{
					m_pLSPExs[i]->nPs[j] = 0;
					rc = sprex.rcParts[j];
					if((rc.right-rc.left)*(rc.bottom-rc.top) > 0) // 영역이 있어야만 처리한다.
					{
						g_pTEng->EncodeToLSP(m_lpdds, m_HSpr.SrcCK, rc, &m_pLSPExs[i]->nPs[j], &pwBuff);
						file.Write(&m_pLSPExs[i]->nPs[j], 4);
						file.Write(pwBuff, m_pLSPExs[i]->nPs[j]);
						delete [] pwBuff; pwBuff = NULL;
					}
					else // 영역이 없으면 0 만 써놓는다.
					{
						file.Write(&m_pLSPExs[i]->nPs[j], 4);
					}
				}
			}
		}	
		else
		{
			// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
			for(i = 0; i < m_HSpr.nQt; i++)
				file.Write(m_pSprExs[i], sizeof(_SPR_LSP_EX));

			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Write(&m_pLSPExs[i]->nV1, 4);
				if(m_pLSPExs[i]->nV1 > 0) file.Write(m_pLSPExs[i]->pV1, m_pLSPExs[i]->nV1);
				file.Write(&m_pLSPExs[i]->nV2, 4);
				if(m_pLSPExs[i]->nV2 > 0) file.Write(m_pLSPExs[i]->pV2, m_pLSPExs[i]->nV2);
				file.Write(&m_pLSPExs[i]->nSH, 4);
				if(m_pLSPExs[i]->nSH > 0) file.Write(m_pLSPExs[i]->pSH, m_pLSPExs[i]->nSH);
				file.Write(&m_pLSPExs[i]->nFX, 4);
				if(m_pLSPExs[i]->nFX > 0) file.Write(m_pLSPExs[i]->pFX, m_pLSPExs[i]->nFX);
				for(int j = 0; j < MAX_PART; j++) // 무기, 아이템등 파트 영역
				{
					file.Write(&m_pLSPExs[i]->nPs[j], 4);
					if(m_pLSPExs[i]->nPs[j] > 0) file.Write(m_pLSPExs[i]->pPs[j], m_pLSPExs[i]->nPs[j]);
				}
			}
		}
		file.Close();
	}
	else if(nType == SAVE_SPR_LSP_INDEXED && m_pRGBTbls)
	{
		CFile file; file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);

		int i;
		wsprintf(m_HSpr.szID, "단순구조 256LSP : %s | Sprite - %d", szFileName, m_HSpr.nQt); // 스프라이트 저장
		file.Write(&m_HSpr, sizeof(_SPR_HEADER));
		
		// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
		for(i = 0; i < m_HSpr.nQt; i++)
		{
			file.Write(&m_SprExs[i], sizeof(_SPR_TOOL));
		}

		// 팔레트 테이블 쓰기..
		file.Write(m_pRGBTbls, sizeof(RGBQUAD)*256);
		
		if(m_lpdds)
		{
			// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
			_SPR_LSP_EX spr;
			_SPR_LSP spr2;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				// 툴에서 쓰는 확장 스프라이트를 게임에서 쓰는 스프라이트로 변환
				ConvSprToolExToSprLSPEx(&m_SprExs[i], &spr);
				spr2.rcV = spr.rcV1;
				file.Write(&spr2, sizeof(_SPR_LSP));
			}

			// 압축 스프라이트 데이터
			WORD* pwBuff = NULL;
			CRect rc;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				_SPR_TOOL sprex = m_SprExs[i];

				m_pLSPExs[i]->nV1 = 0; // Byte 수..
				rc = sprex.rcV1; // 캐릭터 보이는 영역 - Part 1
				if((rc.right-rc.left)*(rc.bottom-rc.top) > 0) // 영역이 있어야만 처리한다.
				{
					g_pTEng->EncodeToLSP(m_lpdds, m_HSpr.SrcCK, rc, &m_pLSPExs[i]->nV1, &pwBuff);
					file.Write(&m_pLSPExs[i]->nV1, 4);
					WORD* pTemp = pwBuff;
					WORD wLine = *pTemp; pTemp++;
					file.Write(&wLine, 2); // 라인 카운트
					for(int j = 0; j < wLine; j++)
					{
						WORD wNode = *pTemp; pTemp++;
						file.Write(&wNode, 2); // 노드 카운트
						for(int k = 0; k < wNode; k++)
						{
							WORD wSkip = *pTemp; pTemp++;
							WORD wPixelCount = *pTemp; pTemp++;
							file.Write(&wSkip, 2); // 건너뛰는 픽셀 카운트
							file.Write(&wPixelCount, 2); // 실제 픽셀 카운트
							// 팔레트 처리...
							BYTE* p8Pixels = new BYTE[wPixelCount];
							g_pTEng->Convert16PixelTo8Pixel(pTemp, p8Pixels, m_pRGBTbls, wPixelCount); pTemp += wPixelCount;
							file.Write(p8Pixels, wPixelCount);
							delete [] p8Pixels; p8Pixels = NULL;
						}
					}
					if(pwBuff) delete [] pwBuff; pwBuff = NULL;
				}
				else
				{
					file.Write(&m_pLSPExs[i]->nV1, 4);
				}
			}
		}
		else
		{
			// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
			_SPR_LSP spr;
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				spr.rcV = m_pSprExs[i]->rcV1;
				file.Write(&spr, sizeof(_SPR_LSP));
			}

			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Write(&m_pLSPExs[i]->nV1, 4);
				if(m_pLSPExs[i]->nV1 > 0)
				{
					WORD* pTemp = m_pLSPExs[i]->pV1;
					WORD wLine = *pTemp; pTemp++;
					file.Write(&wLine, 2); // 라인 카운트
					for(int j = 0; j < wLine; j++)
					{
						WORD wNode = *pTemp; pTemp++;
						file.Write(&wNode, 2); // 노드 카운트
						for(int k = 0; k < wNode; k++)
						{
							WORD wSkip = *pTemp; pTemp++;
							WORD wPixelCount = *pTemp; pTemp++;
							file.Write(&wSkip, 2); // 건너뛰는 픽셀 카운트
							file.Write(&wPixelCount, 2); // 실제 픽셀 카운트
							// 팔레트 처리...
							BYTE* p8Pixels = new BYTE[wPixelCount];
							g_pTEng->Convert16PixelTo8Pixel(pTemp, p8Pixels, m_pRGBTbls, wPixelCount); pTemp += wPixelCount;
							file.Write(p8Pixels, wPixelCount);
							delete [] p8Pixels; p8Pixels = NULL;
						}
					}
				}
			}
		}
		file.Close();
	}
	// 스프라이트...................................................................
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// 에니메이션...................................................................
	else if(nType == SAVE_ANI)
	{
		// 에니메이션 저장
		CFile file; file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);
		int tmp = m_HAni.nQt;
		m_HAni.nQt = 0;
		int i = 0;
		for(i = 0; i < tmp; i++) // 유효한 숫자 세기..
		{
			if(m_pAnis[i]) m_HAni.nQt++;
		}

		wsprintf(m_HAni.szID, "소마신화전기AniFile : %s | %d 개", szFileName, m_HAni.nQt);
		file.Write(&m_HAni, sizeof(_ANI_HEADER));
		
		for(i = 0; i < tmp; i++)
		{
			if(m_pAnis[i]) file.Write(m_pAnis[i], sizeof(_ANI_DATA));
		}
		file.Close();
	}
	// 에니메이션...................................................................
	////////////////////////////////////////////////////////////////////////////////
}
*/

BOOL CAniObjTool::AniAdd()
{
	if(m_HAni.nQt >= MAX_ANI) return FALSE;
	if(!m_pAnis[m_HAni.nQt]) m_pAnis[m_HAni.nQt] = new _ANI_DATA;
	MAKE_ZERO(*m_pAnis[m_HAni.nQt]);
	lstrcpy(m_pAnis[m_HAni.nQt]->szName, "추가 에니메이션");
	m_HAni.nQt++;
	return TRUE;
}

BOOL CAniObjTool::AniInsert(int index) // 삽입
{
	if(index < 0 || index >= MAX_ANI-1) return FALSE;

	if(!m_pAnis[m_HAni.nQt]) m_pAnis[m_HAni.nQt] = new _ANI_DATA; // 끝에 하나 만들고

	m_HAni.nQt++;
	for(int i = m_HAni.nQt-1; i > index; i--) // 뒤로 하나씩 밀려서 복사.
	{
		*m_pAnis[i] = *m_pAnis[i-1];
	}
	MAKE_ZERO(*m_pAnis[index]);
	return TRUE;
}

BOOL CAniObjTool::AniDelete(int index) // 중간 삭제.
{
	if(index < 0 || index >= MAX_ANI) return FALSE;
	if(m_HAni.nQt <= 0) return FALSE;
	
	for(int i = index; i < m_HAni.nQt-2; i++) // 앞으로 하나씩 밀려 복사..
	{
		*m_pAnis[i] = *m_pAnis[i+1];
	}

	if(m_pAnis[m_HAni.nQt])
	{
		delete m_pAnis[m_HAni.nQt];
		m_pAnis[m_HAni.nQt] = NULL; // 끝에 하나 삭제
	}
	m_HAni.nQt--;

	return TRUE;
}

/*
void CAniObjTool::ConvSprToolExToSprLSPEx(_SPR_TOOL *pSprEx, _SPR_LSP_EX *pSpr)
{
	if(pSprEx == NULL || pSpr == NULL) return;

	MAKE_ZERO(*pSpr); // 일단 깨끗이!

	CRect rc;
	CPoint pt;

	rc = pSprEx->rcV1;
	if(rc.Width() && rc.Height()) // View Part1
	{
		rc -= pSprEx->ptCV1;
		pSpr->rcV1 = rc;
	}

	rc = pSprEx->rcV2;
	if(rc.Width() && rc.Height()) // View Part2
	{
		rc -= pSprEx->ptCV2;
		pSpr->rcV2 = rc;
	}

	rc = pSprEx->rcSH;
	if(rc.Width() && rc.Height())
	{
		rc -= pSprEx->ptCSH;
		pSpr->rcSH = rc;
	}

	rc = pSprEx->rcFX;
	if(rc.Width() && rc.Height())
	{
		rc -= pSprEx->ptCFX;
		pSpr->rcFX = rc;
	}
	
	for(int i = 0; i < MAX_PART; i++)
	{
		rc = pSprEx->rcParts[i];
		if(rc.Width() && rc.Height())
		{
			rc -= pSprEx->ptParts[i];
			pSpr->rcPs[i] = rc;
		}
	}

	rc = pSprEx->rcD;
	if(rc.Width() && rc.Height())
	{
		rc -= pSprEx->ptCV1;
		pSpr->rcD = rc;
	}
	
	rc = pSprEx->rcA;
	if(rc.Width() && rc.Height())
	{
		rc -= pSprEx->ptCV1;
		pSpr->rcA = rc;
	}
}

void CAniObjTool::ConvSprToolExToSprNormal(_SPR_TOOL *pSprEx, _SPR_NORMAL *pSpr)
{
	if(pSprEx == NULL || pSpr == NULL) return;

	pSpr->ptCV = pSprEx->ptCV1;
	pSpr->ptCSH = pSprEx->ptCSH;
	pSpr->ptCFX = pSprEx->ptCFX;
	pSpr->rcV = pSprEx->rcV1;
	pSpr->rcSH = pSprEx->rcSH;
	pSpr->rcFX = pSprEx->rcFX;
}

void CAniObjTool::ConvSprNormalToSprToolEx(_SPR_NORMAL *pSpr, _SPR_TOOL *pSprEx)
{
	if(pSprEx == NULL || pSpr == NULL) return;

	pSprEx->ptCV1 = pSpr->ptCV;
	pSprEx->ptCSH = pSpr->ptCSH;
	pSprEx->ptCFX = pSpr->ptCFX;
	pSprEx->rcV1 = pSpr->rcV;
	pSprEx->rcSH = pSpr->rcSH;
	pSprEx->rcFX = pSpr->rcFX;
}
*/
int CAniObjTool::HitTest(CPoint point, _SPR_TOOL** ppSprEx)
{
	for(int i = 0; i < m_HSpr.nQt; i++)
	{
		if(PtInRect(&m_SprExs[i].rcV, point) == TRUE)
		{
			*ppSprEx = &m_SprExs[i];
			return i;
		}
	}
	*ppSprEx = NULL;
	return -1;
}

void CAniObjTool::SetColorKey(COLORREF ColorKey)
{
//	if(m_lpdds == NULL) return; // 컬러키 지정
//	g_pTEng->SetColorKey(m_lpdds, ColorKey);
//	m_HSpr.SrcCK = ColorKey;
}

void CAniObjTool::GetStandLSP(RECT *prc, const WORD **pwStand)
{
	if(prc == NULL || pwStand == NULL) return;
//	if(m_pLSPExs[0] == NULL || m_pSprExs[0] == NULL)
//	{
//		MAKE_ZERO(*prc); *pwStand = NULL; return;
//	}
//	
//	*prc = m_pSprExs[0]->rcV1;
//	*pwStand = m_pLSPExs[0]->pV1;
}

void CAniObjTool::AniSave(const char *szFileName)
{
	CFile file;
	if(file.Open(szFileName, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
#ifdef _DEBUG
		OutputDebugString("에니메이션 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
#endif
		return;
	}
	m_HAni.szID[0] = 'A';
	m_HAni.szID[1] = 'N';
	m_HAni.szID[2] = 'I';
	m_HAni.szID[3] = NULL;
	file.Write(&m_HAni, sizeof(_ANI_HEADER)); // 에니메이션 헤더 읽기

	for(int i = 0; i < m_HAni.nQt; i++)
	{
		if(m_pAnis[i]->fSpd == 0.0f) m_pAnis[i]->fSpd = 10.0f; // 기본값으로 세팅..
		file.Write(m_pAnis[i], sizeof(_ANI_DATA)); // 에니메이션 데이터 쓰기
		if(m_pAnis[i]->nMaxFrm > 0)
		{
			file.Write(m_pAnis[i]->pnFrms, m_pAnis[i]->nMaxFrm*m_HAni.nDirQt*2); // 에니메이션 프레임 데이터 쓰기
		}
	}
	file.Close();
}

void CAniObjTool::SprAlloc(int nQt)
{
	CAniObjLSP::SprAlloc(nQt);
}

void CAniObjTool::SprSave(const char *szFileName)
{
	if(szFileName == NULL) return;
	int n = lstrlen(szFileName);
	if(n < 4) return;
	char szExt[3] = { szFileName[n-3], szFileName[n-2], szFileName[n-1] };
	
	int nPF = g_pTEng->PixelFormatGet(); // 픽셀 포맷

	if(	(szExt[0] == 'S' || szExt[0] == 's') && 
		(szExt[1] == 'P' || szExt[1] == 'p') && 
		(szExt[2] == 'L' || szExt[2] == 'l') )
	{
		if(m_lpDDS) // 수동으로 지정되어 있다면..
		{
			CFile file;
			file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);
			
			m_HSpr.szID[0] = 'S';
			m_HSpr.szID[1] = 'P';
			m_HSpr.szID[2] = 'L';
			m_HSpr.szID[3] = 'N';
			wsprintf(m_HSpr.szRemark, "단순 LSP Sprite | Sprite - %d", m_HSpr.nQt);
			file.Write(&m_HSpr, sizeof(_SPR_HEADER));

			// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터
			for(int i = 0; i < m_HSpr.nQt; i++)
				file.Write(&m_SprExs[i], sizeof(_SPR_TOOL));
				
			// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
			this->SprAlloc(m_HSpr.nQt);
			m_HSpr = m_HSpr;

			for(i = 0; i < m_HSpr.nQt; i++)
			{
				g_pTEng->EncodeToLSPData(m_lpDDS, m_HSpr.SrcCK, m_SprExs[i].rcV, m_SprExs[i].ptCV, m_pSprs[i]);
				if(g_pTEng->PixelFormatGet() == PF_555) this->Decode2_565(m_pSprs[i]); // 픽셀 포맷이 555 라면 565 바꾸어 준다..
				file.Write(m_pSprs[i], sizeof(_SPR_LSP));
				if(g_pTEng->PixelFormatGet() == PF_555) this->Decode2_555(m_pSprs[i]); // 픽셀 포맷이 555 라면 565 바꾸어 준다..
			}

			for(i = 0; i < m_HSpr.nQt; i++)
			{
				if(m_pSprs[i]->nDataSize)
				{
					if(nPF == PF_555) this->Decode2_565(m_pSprs[i]);
					file.Write(m_pSprs[i]->pwV, m_pSprs[i]->nDataSize);
					if(nPF == PF_555) this->Decode2_555(m_pSprs[i]);
				}
			}

			file.Close();
		}
		else // 자동 추출된 거면...
		{
			CFile file;
			file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);

			m_HSpr.szID[0] = 'S';
			m_HSpr.szID[1] = 'P';
			m_HSpr.szID[2] = 'L';
			m_HSpr.szID[3] = 'N';
			wsprintf(m_HSpr.szRemark, "LSP Sprite | Sprite - %d", m_HSpr.nQt);
			file.Write(&m_HSpr, sizeof(_SPR_HEADER));
			
			// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터. - 0 으로 채운다..
			_SPR_TOOL sprTemp; MAKE_ZERO(sprTemp);
			for(int i = 0; i < m_HSpr.nQt; i++)
				file.Write(&sprTemp, sizeof(_SPR_TOOL));
				
			// 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				file.Write(m_pSprs[i], sizeof(_SPR_LSP));
			}
			for(i = 0; i < m_HSpr.nQt; i++)
			{
				if(m_pSprs[i]->nDataSize)
				{
					if(nPF == PF_555) this->Decode2_565(m_pSprs[i]);
					file.Write(m_pSprs[i]->pwV, m_pSprs[i]->nDataSize);
					if(nPF == PF_555) this->Decode2_555(m_pSprs[i]);
				}
			}

			file.Close();
		}
	}
	else if(	(szExt[0] == 'S' || szExt[0] == 's') && 
				(szExt[1] == 'P' || szExt[1] == 'p') && 
				(szExt[2] == 'R' || szExt[2] == 'r') )
	{
		CFile file;
		file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);
		
		m_HSpr.szID[0] = 'S';
		m_HSpr.szID[1] = 'P';
		m_HSpr.szID[2] = 'R';
		m_HSpr.szID[3] = 'N';
		wsprintf(m_HSpr.szRemark, "단순 Sprite : | Sprite - %d", m_HSpr.nQt);
		
		file.Write(&m_HSpr, sizeof(_SPR_HEADER));
		
		// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터. - 0 으로 채운다..
		for(int i = 0; i < m_HSpr.nQt; i++)
			file.Write(&m_SprExs[i], sizeof(_SPR_TOOL));
			
		file.Close();
	}
}

void CAniObjTool::SprLoad(const char *szFileName, int nPF)
{
	CFile file;
	
	if(file.Open(szFileName, CFile::modeRead) == FALSE)
	{
		MessageBox(NULL, szFileName, "LSP 스프라이트 파일 열기 실패 : ", MB_OK);
		return;
	}

	SprNew(); // 해제...

	file.Read(&m_HSpr, sizeof(_SPR_HEADER)); // 스프라이트 헤더 읽기.
	
	for(int i = 0; i < m_HSpr.nQt; i++) // 툴에서 쓰는 데이터 읽기..
	{
		file.Read(&m_SprExs[i], sizeof(_SPR_TOOL));
	}
	file.Close();

	if(lstrlen(m_HSpr.szBMPFN) > 0) // 비트맵이 지정되어 있다면..
	{
		RELEASE_SAFE(m_lpDDS);
		CString szBMPFN = g_pTEng->PathGetCur("BMP");
		szBMPFN += '\\'; // 경로 지정
		szBMPFN += m_HSpr.szBMPFN; // 경로 지정
		m_lpDDS = g_pTEng->SurfaceCreateWithBMP(szBMPFN, FALSE, m_HSpr.SrcCK);
	}

	if(m_HSpr.szID[0] == 'S' && m_HSpr.szID[1] == 'P' && m_HSpr.szID[2] == 'L' && m_HSpr.szID[3] == 'N' && m_lpDDS == NULL) // LSP
	{
		CAniObjLSP::SprLoad(szFileName, nPF);
		return;
	}
}

void CAniObjTool::SprLoadOldFormat(const char *szFileName)
{
/*	this->SprNew();

	CFile file;
	if(file.Open(szFileName, CFile::modeRead) == FALSE)
	{
		MessageBox(NULL, szFileName, "파일 열기 실패", MB_OK);
		return;
	}


// LSP Data 구조
//      0000 00
//	4바이트 - 밑의 데이터 크기... 바이트수. // 새 포맷에서는 안쓴다.
//	2바이트	- 라인수 // 새 포맷에서는 안쓴다..
//      |
//      |
//		+--- 2바이트 - 줄에 들어가는 노드수
//      |       |
//      |       +-----+--- 2바이트 - 노드 투명색수(찍을때 건너뛰는길이)
//      |       |     |
//      |       |     +--- 2바이트 - 노드 픽셀수
//      |       |              |
//      |       |              +--- 2바이트 - 실제 픽셀
//      |       |              +---        .
//      |       |              +---        . 노드 픽셀수 만큼 반복
//      |       |              +---        .
//      |       +--------
//      |            .
//      |            . 노드수만큼 반복
//      |            .
//      +-------- 
//		    .
//		    . // 줄 수만큼 반복
//		    .

	struct _SPR_HEADER_OLD
	{
		char	szID[64];
		char	szBMPFN[64];	// 비트맵 파일 이름 - 데이터를 갖고 있으면 안 쓴다.
		int		nWidth;				// 너비
		int		nHeight; 			// 높이
		COLORREF SrcCK;				// 소스 컬러키
		int		nQt;				// 스프라이트 수
	};

	const int MAX_PART = 10; 	// 최대 10 개의 파트 스프라이트 처리
	struct _SPR_TOOL_OLD
	{
		RECT	rcV1;			// 캐릭터 영역 - 파트 1
		RECT	rcV2;			// 캐릭터 영역 - 파트 2
		RECT	rcSH;			// 그림자 영역
		RECT	rcFX;			// 공격 효과 영역
		RECT	rcD;			// 피해 영역 - 캐릭터 센터점 기준으로 한다.
		RECT	rcA;			// 공격 영역 - 캐릭터 센터점 기준으로 한다.
		RECT	rcParts[MAX_PART];	// 무기, 기타 아이템 붙이는 영역..
		
		POINT	ptCV1;			// 센터점 - 파트 1.
		POINT	ptCV2;			// 센터점 - 파트 2.
		POINT	ptCSH;			// 센터점 - 그림자
		POINT	ptCFX;			// 센터점 - 공격 효과
		POINT	ptParts[MAX_PART];	// 무기, 기타 아이템 붙이는 센터점...
	};
//	struct _SPR_TOOL_OLD
//	{
//		POINT	ptCV1;			// 센터점 - 파트 1.
//		POINT	ptCSH;			// 센터점 - 그림자
//		POINT	ptCFX;			// 센터점 - 공격 효과
//		RECT	rcV1;			// 캐릭터 영역 - 파트 1
//		RECT	rcSH;			// 그림자 영역
//		RECT	rcFX;			// 공격 효과 영역
//	};
	
	struct _SPR_LSP_OLD
	{
		RECT	rcV;			// 캐릭터 영역 - Part1
	};

	_SPR_HEADER_OLD m_HSpr;
	file.Read(&m_HSpr, sizeof(_SPR_HEADER_OLD)); // 스프라이트 헤더 읽기.
	this->SprAlloc(m_HSpr.nQt);

	m_HSpr.nQt = m_HSpr.nQt; // 헤더 정보 저장..
	m_HSpr.nWidth = m_HSpr.nWidth;
	m_HSpr.nHeight = m_HSpr.nHeight;
	lstrcpy(m_HSpr.szBMPFN, m_HSpr.szBMPFN);
	m_HSpr.SrcCK = m_HSpr.SrcCK;
	
	int i = 0;

	_SPR_TOOL_OLD sprtool;
	for(i = 0; i < m_HSpr.nQt; i++)
	{
		file.Read(&sprtool, sizeof(_SPR_TOOL_OLD)); // 툴 데이터 읽고..
		m_SprExs[i].ptCV = sprtool.ptCV1; // 툴 데이터 변환??
		m_SprExs[i].rcV = sprtool.rcV1;
	}

	_SPR_LSP_OLD spr;
	for(i = 0; i < m_HSpr.nQt; i++)
	{
		file.Read(&spr, sizeof(_SPR_LSP_OLD));
		m_pSprs[i]->rcV = spr.rcV;
	}
	
	int nPF = g_pTEng->PixelFormatGet();
	for(i = 0; i < m_HSpr.nQt; i++) 	// 압축 스프라이트 데이터
	{
		file.Read(&m_pSprs[i]->nDataSize, 4); // 
		if(m_pSprs[i]->nDataSize > 640000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
		if(m_pSprs[i]->nDataSize > 0)
		{
			file.Read(&m_pSprs[i]->nLine, 2);
			m_pSprs[i]->nDataSize -= 2; // 음 .. 라인수는 LSP 구조체에 있다...
			m_pSprs[i]->pwV = new WORD[m_pSprs[i]->nDataSize/2];
			file.Read(m_pSprs[i]->pwV, m_pSprs[i]->nDataSize);
			if(nPF == 555) this->Decode2_555(m_pSprs[i]);
		}
	}
	file.Close();
*/
}

void CAniObjTool::AniLoadOldFormat(const char *szFileName)
{
/*	this->AniRelease();

	struct _ANI_HEADER_OLD
	{
		char	szID[64];		// 'A' 'N' 'I' 방향 숫자(1,8) 등등
		int		nQt;			// Animation 숫자
	};
	struct _ANI_DATA_OLD
	{
		char		szName[64];	// 에니메이션 이름
		int 	nMaxFrm;
		int 	nFrms[8][32];	// 프레임
		float		fSpd;		// 스피드.. 단위는 Frame/Sec
		int nAttr;
		char szSndFN[64];
		int nSndFrm;
	};

	_ANI_HEADER_OLD HAniOld;
	_ANI_DATA_OLD oldani;

	CFile file;
	file.Open(szFileName, CFile::modeRead);

	// 헤더 변환
	file.Read(&HAniOld, sizeof(_ANI_HEADER_OLD));
	m_HAni.nDirQt = 8;
	m_HAni.nQt = HAniOld.nQt;
	m_HAni.szID[0] = 'A'; m_HAni.szID[1] = 'N'; m_HAni.szID[2] = 'I'; m_HAni.szID[3] = NULL;
	lstrcpy(m_HAni.szRemark, HAniOld.szID);
	
	// 데이터 할당
	this->AniAlloc(m_HAni.nQt);

	for(int i = 0; i < m_HAni.nQt; i++)
	{
		file.Read(&oldani, sizeof(_ANI_DATA_OLD));
		
		// 에니메이션 데이터 변환
		lstrcpy(m_pAnis[i]->szName, oldani.szName);
		m_pAnis[i]->fSpd = oldani.fSpd;
		m_pAnis[i]->nFlag = oldani.nAttr;
		m_pAnis[i]->nMaxFrm = oldani.nMaxFrm;
		if(m_pAnis[i]->nMaxFrm > 0)
		{
			m_pAnis[i]->pnFrms = new short int[m_pAnis[i]->nMaxFrm*m_HAni.nDirQt];
			for(int nd = 0; nd < m_HAni.nDirQt; nd++)
			{
				for(int nf = 0; nf < m_pAnis[i]->nMaxFrm; nf++)
				{
					m_pAnis[i]->pnFrms[nd * m_pAnis[i]->nMaxFrm + nf] = oldani.nFrms[nd][nf];
				}
			}
		}
	}
	file.Close();
	*/
}

void CAniObjTool::AniDirQtSet(int nDirQt)
{
	if(nDirQt <= 0) return;

	int nProcessQt = 0;
	(nDirQt < m_HAni.nDirQt) ? nProcessQt = nDirQt : nProcessQt = m_HAni.nDirQt; // 작은쪽을 가려낸다.
	m_HAni.nDirQt = nDirQt;
	for(int i = 0; i < m_HAni.nQt; i++)
	{
		int nMaxFrm = m_pAnis[i]->nMaxFrm;
		short int* pnFrms = m_pAnis[i]->pnFrms; // 전의 프레임 배열 포인터 기억하고..

		if(nMaxFrm > 0) // 만약 프레임이 있다면..
		{
			m_pAnis[i]->pnFrms = new short int[nDirQt*nMaxFrm];
			memset(m_pAnis[i]->pnFrms, 0, nDirQt*m_pAnis[i]->nMaxFrm*2);
			for(int j = 0; j < nProcessQt; j++)
			{
				memcpy(&m_pAnis[i]->pnFrms[j*nMaxFrm], &pnFrms[j*nMaxFrm], nMaxFrm*2);
			}
		}

		DELETE_ARRAY_SAFE(pnFrms); // 전의 거 해제..
	}
}
