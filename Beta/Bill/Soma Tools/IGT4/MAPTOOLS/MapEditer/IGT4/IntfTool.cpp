// IntfTool.cpp: implementation of the CIntfTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IntfTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntfTool::CIntfTool()
{
	MAKE_ZERO(m_pIntfClips);
	New();
}

CIntfTool::~CIntfTool()
{
	ClipBoardClear();
}

void CIntfTool::New()
{
	this->IntfRelease();
	MAKE_ZERO(m_Header);
	
	DELETE_SAFE(m_pAniObj);
	m_pAniObj = new CAniObjLSP();
}

void CIntfTool::Save(const char* szFileName)
{
	CFile file;
	file.Open(szFileName, CFile::modeWrite | CFile::modeCreate);
	
	_INTF_HEADER Header = m_Header; // 임시로 있는 것만 찾아서 쓴다..

	Header.nQt = 0;
	for(int i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfs[i]) Header.nQt++;
	}

	file.Write(&Header, sizeof(_INTF_HEADER)); // 인터페이스 헤더 쓰기.

	BOOL bSetRgnDefault = FALSE;
	for(i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfs[i] == NULL) continue;
		if(m_pIntfs[i]->nParentLayer < 0 || m_pIntfs[i]->nParentLayer >= MAX_LAYER) 
		{
			MessageBox(NULL, "레이어 번호가 잘못되어 있습니다. 다시 지정해 주세요", "파일 저장 실패", MB_OK);
			file.Close();
			return;
		}
		
		// 버튼 영역 체크..
		if(m_pIntfs[i]->nType != INTF_TYPE_NORMAL) // 기능을 갖고 있는 인터페이스이면...
		{
			if(	(m_pIntfs[i]->rcRgn.right - m_pIntfs[i]->rcRgn.left < 3 &&
				m_pIntfs[i]->rcRgn.bottom - m_pIntfs[i]->rcRgn.top < 3) || m_pIntfs[i]->rcRgn.left < 0 || m_pIntfs[i]->rcRgn.top < 0)
			{	
				if(bSetRgnDefault == FALSE)
				{
					CString str;
					str.Format("영역값이 너무 작거나 정해지지 않았습니다 : 인터페이스 번호 %d - 모두 기본값으로 저장하시겠습니까?", i);
					int id = MessageBox(NULL, str, "기본값 저장 확인", MB_YESNO);
					if(id == IDYES) bSetRgnDefault = TRUE;
				}
				
				if(bSetRgnDefault == TRUE)
				{
					_ANI_DATA* pAni = m_pAniObj->AniDataGet(m_pIntfs[i]->nAnis[0]);
					if(pAni)
					{
						const _SPR_LSP* pSpr;
						m_pAniObj->SprGet(pAni->pnFrms[0], &pSpr);
						if(pSpr)
						{
							m_pIntfs[i]->rcRgn.left = m_pIntfs[i]->ptView.x + pSpr->rcV.left;
							m_pIntfs[i]->rcRgn.top = m_pIntfs[i]->ptView.y + pSpr->rcV.top;
							m_pIntfs[i]->rcRgn.right = m_pIntfs[i]->ptView.x + pSpr->rcV.right;
							m_pIntfs[i]->rcRgn.bottom = m_pIntfs[i]->ptView.y + pSpr->rcV.bottom;
						}
					}
				}
			}
			// 스크롤바와 다이얼 체크
			if(	(m_pIntfs[i]->nType & INTF_TYPE_DIAL_LR) || 
				(m_pIntfs[i]->nType & INTF_TYPE_DIAL_TB) || 
				(m_pIntfs[i]->nType & INTF_TYPE_SCROLL_LR) || 
				(m_pIntfs[i]->nType & INTF_TYPE_SCROLL_TB) ||
				(m_pIntfs[i]->nType & INTF_TYPE_PROGRESS) ) 
			{
				// 초기값과 스크롤(다이얼) 최소,최대값 점검..
				if(m_pIntfs[i]->nFlags[0] >= m_pIntfs[i]->nFlags[1])
				{
					CString str;
					str.Format("최소, 최대값이 정해지지 않았습니다 : 인터페이스 번호 %d - 기본값(0 ~ 100)으로 하시겠습니까?", i);
					if(MessageBox(NULL, str, "기본값 저장 확인", MB_YESNO) == IDYES)
					{
						m_pIntfs[i]->nFlags[0] = 0;
						m_pIntfs[i]->nFlags[1] = 100;
					}
				}
			}
		}
		file.Write(m_pIntfs[i], sizeof(_INTF_DATA)); // 인터페이스 데이터 읽기.
		if(m_pIntfs[i]->nText > 0) file.Write(m_pIntfs[i]->szText, m_pIntfs[i]->nText); // 텍스트 데이터 쓰기.
	}
	file.Close();
}

void CIntfTool::ImportSpr(const char* szFileName, int nPF)
{
	lstrcpy(m_Header.szSprFN, szFileName);

	m_pAniObj->SprLoad(szFileName, nPF);
}

void CIntfTool::ImportAni(const char* szFileName)
{
	lstrcpy(m_Header.szAniFN, szFileName);

	m_pAniObj->AniLoad(szFileName);
	m_pAniObj->AniCtrlAlloc(MAX_ANI_CTRL); // 최대한 할당하고.. 인터페이스 하나당 하나씩 에니메이션 할당..
}

// 빈데이터 찾아서 할당. 실패하면 -1
int CIntfTool::Add()
{
	for(int i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfs[i] == NULL) // 빈거 찾으면..
		{
			m_pIntfs[i] = new _INTF_DATA;
			MAKE_ZERO(*m_pIntfs[i]);
			return i;
		}
	}
	return -1;
}

void CIntfTool::Delete(int index) // 중간 삭제.
{
	if(index < 0 || index >= MAX_INTF) return;
	
	if(m_pIntfs[index])
	{
		DELETE_ARRAY_SAFE(m_pIntfs[index]->szText);
		DELETE_SAFE(m_pIntfs[index]);
	}
}

void CIntfTool::ClipBoardAdd(int index)
{
	if(index < 0 || index >= MAX_INTF) return;
	if(m_pIntfs[index] == NULL) return;

	for(int i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfClips[i] == NULL)
		{
			m_pIntfClips[i] = new _INTF_DATA;
			MAKE_ZERO(*m_pIntfClips[i]);
			memcpy(m_pIntfClips[i], m_pIntfs[index], sizeof(_INTF_DATA));
			m_pIntfClips[i]->szText = NULL;
			if(m_pIntfClips[i]->nText > 0)
			{
				m_pIntfClips[i]->szText = new char[m_pIntfClips[i]->nText + 1];
				lstrcpy(m_pIntfClips[i]->szText, m_pIntfs[index]->szText);
			}
			return;
		}
	}
}

void CIntfTool::ClipBoardClear()
{
	for(int i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfClips[i])
		{
			DELETE_ARRAY_SAFE(m_pIntfClips[i]->szText);
			DELETE_SAFE(m_pIntfClips[i]);
		}
	}
}

void CIntfTool::ClipBoardPaste(int nDestIntf, int nSeq, int nParentLayer)
{
	if(nParentLayer < 0 || nParentLayer >= MAX_LAYER) return;
	if(nDestIntf < 0 || nDestIntf >= MAX_INTF) return;
	if(nSeq < 0 || nSeq >= MAX_INTF) return;
	if(m_pIntfs[nDestIntf] == NULL) return;
	if(m_pIntfClips[nSeq] == NULL) return;

	DELETE_ARRAY_SAFE(m_pIntfs[nDestIntf]->szText);
	memcpy(m_pIntfs[nDestIntf], m_pIntfClips[nSeq], sizeof(_INTF_DATA));
	m_pIntfs[nDestIntf]->szText = NULL;
	if(m_pIntfs[nDestIntf]->nText > 0)
	{
		m_pIntfs[nDestIntf]->szText = new char[m_pIntfs[nDestIntf]->nText + 1];
		lstrcpy(m_pIntfs[nDestIntf]->szText, m_pIntfClips[nSeq]->szText);
	}
}

int CIntfTool::ClipBoardGetQt()
{
	int nQt = 0;
	for(int i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfClips[i] == NULL) return i;
	}
	
	return 0;
}
