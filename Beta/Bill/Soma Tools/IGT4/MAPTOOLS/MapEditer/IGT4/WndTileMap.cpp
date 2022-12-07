// WndMapTile.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "WndTileMap.h"

#include "GlobalVar.h"
#include "math.h"
#include "HouseNumberDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndTileMap

POINT PosToCell(POINT p);
POINT CellToPos(POINT p);

static _TILE_MAP_DATA_TOOL* m_pMDataClip; // 클립 보드용...

CWndTileMap::CWndTileMap()
{
	m_ptLBPrev.x = m_ptLBPrev.y = 0;
	m_ptScrollPrev.x = m_ptScrollPrev.y = 0;

	m_rcNeedUpdate.SetRect(0,0,0,0);
	m_rcSelect.SetRect(0,0,0,0);
	m_rcCopy.SetRect(0,0,0,0);
	m_ptMouse.x = 0; m_ptMouse.y = 0;
	m_ptRBDown.x = 0; m_ptRBDown.y = 0;
	m_pMDataClip = NULL;
	m_rcSetSel.SetRect(0,0,0,0);
	
	m_hCursorDrag = NULL;
	m_hCursorOrg = NULL;

	m_bAniMode = FALSE;

	MAKE_ZERO(m_UndoData);

	m_MSM.SetViewPtr(this);
}

CWndTileMap::~CWndTileMap()
{
	DELETE_ARRAY_SAFE(m_pMDataClip); // 클립보드 데이터 없애기
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		DELETE_ARRAY_SAFE(m_UndoData.pDatas[i]);
	}
}


BEGIN_MESSAGE_MAP(CWndTileMap, CWnd)
	//{{AFX_MSG_MAP(CWndTileMap)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZING()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndTileMap message handlers
void CWndTileMap::OnPaint() 
{
	if(m_bAniMode == TRUE) return;
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

	CPaintDC dc(this); // device context for painting
	CRect rcPaint = dc.m_ps.rcPaint;

	g_pTEng->SurfaceFillRect(NULL, &rcPaint);
	if(m_Dlg.GetSafeHwnd() == NULL)
	{
		g_pTEng->Flip(&rcPaint);
		return;
	}

	int nTS = m_Dlg.m_ListTS.GetCurSel();
	int nTSS = m_Dlg.m_ListTSS.GetCurSel();

	int nWork = m_Dlg.m_CBWork.GetCurSel();
	int edit_info = m_Dlg.GetCheckedRadioButton(IDC_RADIO_NO_EDIT, IDC_RADIO_EVENT_EDIT);

	if(nWork == 0) // 타일셋 작업일 경우
	{
		if(nTS >= 0 && nTS < MAX_TILE_SET) // 타일셋 노드가 선택되어 있음...
		{
			if(g_pTSet->m_lpDDSs[nTS] != NULL)
			{
				CRect rcSrc;
				g_pTEng->SurfaceGetSize(g_pTSet->m_lpDDSs[nTS], &rcSrc);
				g_pTEng->BltNormal(g_pTSet->m_lpDDSs[nTS], -ptScroll.x*TILE_SIZE, -ptScroll.y*TILE_SIZE, rcSrc, FALSE);

				CRect rcBox((m_rcSetSel.left - ptScroll.x) * TILE_SIZE - 1,
							(m_rcSetSel.top - ptScroll.y) * TILE_SIZE - 1,
							(m_rcSetSel.right - ptScroll.x) * TILE_SIZE,
							(m_rcSetSel.bottom - ptScroll.y) * TILE_SIZE);
				g_pTEng->DrawBox(NULL, &rcBox, RGB(255,255,0));
			}
		}
	}
	else if(nWork == 1 || nWork == 2) // 타일 맵 작업, 오브젝트 작업일 경우...
	{
		CRect rcD(	rcPaint.left/TILE_SIZE + ptScroll.x,
					rcPaint.top/TILE_SIZE + ptScroll.y,
					rcPaint.right/TILE_SIZE + 1 + ptScroll.x,
					rcPaint.bottom/TILE_SIZE + 1 + ptScroll.y);
		if(rcD.right > g_pTMap->m_HTMap.nWidth) rcD.right = g_pTMap->m_HTMap.nWidth;
		if(rcD.bottom > g_pTMap->m_HTMap.nHeight) rcD.bottom = g_pTMap->m_HTMap.nHeight;

		int nCurLayer = m_Dlg.m_LBLayer.GetCurSel();
		if(nCurLayer < 0 || nCurLayer >= MAX_TILE_MAP_LAYER) return; // 레이어...

		BOOL bUCK = FALSE; // 투명찍기인가?
		int nRenderType = RENDER_NORMAL;
			
		LPDIRECTDRAWSURFACE lpDDS;
		for(int nLayer = 0; nLayer < MAX_TILE_MAP_LAYER; nLayer++)
		{
			if(m_Dlg.m_LBLayer.GetItemData(nLayer) == TRUE) continue; // 레이어 숨기기면 보이지 않는다.

			int x, y;
			for(y = rcD.top; y < rcD.bottom; y++) // 바닥에 깔리는 맵
			{
				for(x = rcD.left; x < rcD.right; x++)
				{
					const _TILE_MAP_DATA_TOOL* pTMData = g_pTMap->ToolDataGet(nLayer, x, y);
					if(pTMData == NULL) continue;
					if(pTMData->snTS < 0 || pTMData->snTS >= MAX_TILE_SET) continue;
					lpDDS = g_pTSet->m_lpDDSs[pTMData->snTS];
					if(lpDDS == NULL) continue;
					POINT *t_pos = g_pTSet->DataGetPos(pTMData->snTS, pTMData->snTSS, pTMData->snNum);

					if(t_pos == NULL ||
						t_pos->x < 0 ||
						t_pos->x >= MAX_TILE_SET_WIDTH ||
						t_pos->y < 0 ||
						t_pos->y >= MAX_TILE_SET_HEIGHT) continue; // 쓰지 않는 타일이면 지나간다..

					CRect rcSrc;
					rcSrc.left = t_pos->x * TILE_SIZE;
					rcSrc.top = t_pos->y * TILE_SIZE;
					rcSrc.right = rcSrc.left + TILE_SIZE;
					rcSrc.bottom = rcSrc.top + TILE_SIZE;
					
					bUCK = FALSE;
					nRenderType = RENDER_NORMAL;

					if(nLayer == 0) // 기본 깔리는 맵
						bUCK = FALSE;
					else if(nLayer == 1) // 겹치는 맵
						bUCK = TRUE;
					else if(nLayer == 2) // 그림자 레이어
					{
						bUCK = TRUE;
						nRenderType = RENDER_SHADOW;
					}
					else if(nLayer == 3 || nLayer == 4)
					{
						if(pTMData->snFlags & 2) bUCK = TRUE; // 투명 찍기면
						else bUCK = FALSE;
					}
					
					g_pTEng->BltNormal(lpDDS, (x-ptScroll.x)*TILE_SIZE, (y-ptScroll.y)*TILE_SIZE, rcSrc, bUCK, nRenderType, 16);
				}
			}
			// 속성 표시..
			if(m_Dlg.IsDlgButtonChecked(IDC_CHECK_VIEW_ATTR))
			{
				CString szAttr;
				for(y = rcD.top; y < rcD.bottom; y++) // 바닥에 깔리는 맵
				{
					for(x = rcD.left; x < rcD.right; x++)
					{
						const _TILE_MAP_DATA_TOOL* pTMData = g_pTMap->ToolDataGet(nLayer, x, y);
						if(pTMData == NULL) continue;
						if(pTMData->snFlags & 1)
							g_pTEng->TextOut(NULL, (x-ptScroll.x)*TILE_SIZE, (y-ptScroll.y)*TILE_SIZE, "충돌", RGB(255,255,0));
						if(pTMData->snFlags & 2)
							g_pTEng->TextOut(NULL, (x-ptScroll.x)*TILE_SIZE, (y-ptScroll.y)*TILE_SIZE + TILE_SIZE/2, "투명", RGB(255,255,0));
					}
				}
			} // end of if(m_Dlg.IsDlgButtonCheck....
		} // end of for(int nLayer = ....

		// 오브젝트 배치 표시
		int x, y;
		for(y = rcD.top; y < rcD.bottom; y++) // 바닥에 깔리는 맵
		{
			for(x = rcD.left; x < rcD.right; x++)
			{
				int nObj = m_DlgObj.m_MapObj.DataGet(x, y);
				if(nObj < 0) continue;

				int nSet = nObj >> 8; // 오브젝트 세트
				if(nSet < 0 || nSet >= MAX_MAP_OBJ_REG) continue;
				int nSubSet = nObj & 0xff; // 오브젝트 서브 세트
				if(nSubSet < 0) continue;

				CAniObjLSP* pAniObj = &m_DlgObj.m_MapObj.m_Objs[nSet];
				const _SPR_LSP* pSpr = NULL;
				int nSpr = pAniObj->FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
				pAniObj->SprGet(nSpr, &pSpr);
				if(pSpr == NULL) continue;

				g_pTEng->BackBufferLock();
				g_pTEng->BltLSP(NULL, (x-ptScroll.x)*TILE_SIZE + TILE_SIZE/2, (y-ptScroll.y)*TILE_SIZE + TILE_SIZE/2, pSpr);
				g_pTEng->BackBufferUnlock();
			}
		}

	}

	if(nWork == 1 && edit_info == IDC_RADIO_NO_EDIT)
	{
		if(	nTS >= 0 && nTS < MAX_TILE_SET &&
			nTSS >= 0 && nTSS < MAX_TILE_SUB_SET &&
			m_Dlg.m_CBrushErase.GetCheck() == FALSE) // 타일 맵일때 선택된 타일 보내기
		{
			_TILE_SUB_SET_DATA_TOOL* pTSData = NULL;
			
			pTSData = g_pTSet->DataGetSafe(nTS, nTSS);
			if(pTSData != NULL)
			{
				int nBrush = m_Dlg.m_CBBrush.GetCurSel();
				CPoint pt(m_ptMouse.x/TILE_SIZE + ptScroll.x, m_ptMouse.y/TILE_SIZE + ptScroll.y);

				if(pTSData->nWidth > 0 && pTSData->nHeight > 0)
				{

					int width = (nBrush*2 + 1)*pTSData->nWidth;
					int height = (nBrush*2 + 1)*pTSData->nHeight;
					CRect rcTmp;
					rcTmp.left = pt.x - width/2;
					rcTmp.top = pt.y - height/2;
					rcTmp.right = rcTmp.left + width;
					rcTmp.bottom = rcTmp.top + height;

					POINT* pPtTmp, t_p;
					for(int y = rcTmp.top, y2 = 0; y < rcTmp.bottom; y++, y2++)
					{
						for(int x = rcTmp.left, x2 =0; x < rcTmp.right; x++, x2++)
						{
							t_p.x = x2%pTSData->nWidth; t_p.y = y2%pTSData->nHeight;
							pPtTmp = pTSData->GetPos(t_p);
							if(pPtTmp == NULL) continue;

							CRect rcSrc;
							rcSrc.left = pPtTmp->x*TILE_SIZE;		rcSrc.top = pPtTmp->y*TILE_SIZE;
							rcSrc.right = rcSrc.left + TILE_SIZE;	rcSrc.bottom = rcSrc.top + TILE_SIZE;
							g_pTEng->BltNormal(g_pTSet->m_lpDDSs[nTS], (x - ptScroll.x)*TILE_SIZE, (y - ptScroll.y)*TILE_SIZE, rcSrc, TRUE);
						}
					}

					CRect rcBox = rcTmp; // 타일셋 박스 그리기.
					rcBox.left = (rcTmp.left - ptScroll.x)*TILE_SIZE;
					rcBox.top = (rcTmp.top - ptScroll.y)*TILE_SIZE;
					rcBox.right = rcBox.left + width*TILE_SIZE;
					rcBox.bottom = rcBox.top + height*TILE_SIZE;
					g_pTEng->DrawBox(NULL, &rcBox, RGB(255,255,0));
				}
			}
		}
		else if(m_Dlg.m_CBrushErase.GetCheck() == TRUE) // 지우개 박스 그리기..
		{
			int nBrush = m_Dlg.m_CBBrush.GetCurSel();
			CRect rcBox;
			rcBox.left = (m_ptMouse.x/TILE_SIZE-nBrush)*TILE_SIZE;
			rcBox.top = (m_ptMouse.y/TILE_SIZE-nBrush)*TILE_SIZE;
			rcBox.right = (m_ptMouse.x/TILE_SIZE+nBrush + 1)*TILE_SIZE;
			rcBox.bottom = (m_ptMouse.y/TILE_SIZE+nBrush + 1)*TILE_SIZE;
			g_pTEng->DrawBox(NULL, &rcBox, RGB(255,0,0));
		}

		// 클립 보드 타일 그리기.. - 반투명
		if(m_pMDataClip)
		{
			const _TILE_MAP_DATA_TOOL* pTMData;
			int x, y, x2, y2;
			for(y2 = 0, y = m_rcCopy.top; y < m_rcCopy.bottom; y++, y2++)
			{
				for(x2 = 0, x = m_rcCopy.left; x < m_rcCopy.right; x++, x2++)
				{
					CRect rcSrc;
					pTMData = &m_pMDataClip[y2*m_rcCopy.Width() + x2];
					POINT *t_pos = g_pTSet->DataGetPos(pTMData->snTS, pTMData->snTSS, pTMData->snNum);

					if(t_pos == NULL) continue;
					rcSrc.left = t_pos->x*TILE_SIZE;
					rcSrc.top = t_pos->y*TILE_SIZE;
					rcSrc.right = rcSrc.left + TILE_SIZE;
					rcSrc.bottom = rcSrc.top + TILE_SIZE;
					
					LPDIRECTDRAWSURFACE lpDDS = g_pTSet->m_lpDDSs[pTMData->snTS];

					g_pTEng->BltNormal(lpDDS,
						(m_rcSelect.left + x2 - ptScroll.x)*TILE_SIZE,
						(m_rcSelect.top + y2 - ptScroll.y)*TILE_SIZE,
						&rcSrc, TRUE);
				}
			}
		}
		// 선택된 영역 박스 그리기..
		CRect rcSelectBox = m_rcSelect - ptScroll;
		rcSelectBox.left *= TILE_SIZE;
		rcSelectBox.top *= TILE_SIZE;
		rcSelectBox.right *= TILE_SIZE;
		rcSelectBox.bottom *= TILE_SIZE;
		g_pTEng->DrawBox(NULL, rcSelectBox, RGB(0,0,255));
	}

	// 그리드 그리기..
	if(nWork == 1 && edit_info != IDC_RADIO_NO_EDIT)
	{
		int start_x, start_y;
		int x, y, i, j;
		RECT client_rect;
		CPoint scroll_pos, move_start, move_end;

		GetClientRect(&client_rect);
		if(ptScroll.x % 2) start_x = -32;
		else start_x = 0;
		start_y = 0;

		for(y = start_y+16; y <= client_rect.bottom+32; y += 32)
		{
			for(x = start_x+32; x <= client_rect.right+64; x += 64)
			{
				g_pTEng->DrawRhombus(NULL, x, y, 0x00000000);			// 그리드 그리기
			}
		}

		if(g_pTMove->m_pMoveInfo)
		{
			scroll_pos.x = ptScroll.x * TILE_SIZE;
			scroll_pos.y = ptScroll.y * TILE_SIZE;
			move_end.x = client_rect.right + scroll_pos.x;
			move_end.y = client_rect.bottom + scroll_pos.y;
			move_start = PosToCell(scroll_pos);
			move_end = PosToCell(move_end);

			if(move_start.x < 0) move_start.x = 0;
			if(move_start.y < 0) move_start.y = 0;
			if(move_end.x > g_pTMove->m_vDim.cx-2) move_end.x = g_pTMove->m_vDim.cx - 2;
			if(move_end.y > g_pTMove->m_vDim.cy-2) move_end.y = g_pTMove->m_vDim.cy - 2;
			if(edit_info == IDC_RADIO_MOVE_EDIT)
			{
				for(i = move_start.y; i <= move_end.y; i++)
				{
					for(j = move_start.x; j <= move_end.x; j++)
					{
						if((i+j)%2==0)
						{
							if(g_pTMove->GetMovable(j, i))
							{
								CPoint pos(j,i);
								pos = CellToPos(pos);
								g_pTEng->TextOut(NULL, pos.x-scroll_pos.x, pos.y-scroll_pos.y-7, "M", RGB(255,255,255), TA_CENTER);
							}
						}
					}
				}
			}
			else if(edit_info == IDC_RADIO_HOUSE_EDIT)
			{
				short house;
				CString msg;
				for(i = move_start.y; i <= move_end.y; i++)
				{
					for(j = move_start.x; j <= move_end.x; j++)
					{
						if((i+j)%2==0)
						{
							house = g_pTMove->GetHouse(j, i);
							if(house != -1)
							{
								msg.Format("%d", house);
								CPoint pos(j,i);
								pos = CellToPos(pos);
								g_pTEng->TextOut(NULL, pos.x-scroll_pos.x, pos.y-scroll_pos.y-7, msg, RGB(255,255,255), TA_CENTER);
							}
						}
					}
				}
			}
			else if(edit_info == IDC_RADIO_EVENT_EDIT)
			{
				short event;
				CString msg;
				for(i = move_start.y; i <= move_end.y; i++)
				{
					for(j = move_start.x; j <= move_end.x; j++)
					{
						if((i+j)%2==0)
						{
							event = g_pTMove->GetEvent(j, i);
							if(event != -1)
							{
								msg.Format("%d", event);
								CPoint pos(j,i);
								pos = CellToPos(pos);
								g_pTEng->TextOut(NULL, pos.x-scroll_pos.x, pos.y-scroll_pos.y-7, msg, RGB(255,255,255), TA_CENTER);
							}
						}
					}
				}
			}
		}
	}
	else if(m_Dlg.IsDlgButtonChecked(IDC_CHECK_VIEW_GRID))
	{
		int x1 = rcPaint.left/TILE_SIZE;
		int y1 = rcPaint.top/TILE_SIZE;
		int x2 = rcPaint.right/TILE_SIZE;
		int y2 = rcPaint.bottom/TILE_SIZE;

		for(int x = x1; x <= x2; x++)
		{
			g_pTEng->DrawLine(NULL, x*TILE_SIZE, rcPaint.top, x*TILE_SIZE, rcPaint.bottom, 0x0000ff00);
		}
		for(int y = y1; y <= y2; y++)
		{
			g_pTEng->DrawLine(NULL, rcPaint.left, y*TILE_SIZE, rcPaint.right, y*TILE_SIZE, 0x0000ff00);
		}
	}

	if(m_MSM.IsUsing())
	{
		g_pTEng->DrawPolygon(NULL, m_MSM.m_vCurPos, 5, 0x00ffffff);
	}

	g_pTEng->HWndSet(this->m_hWnd);
	g_pTEng->Flip(&rcPaint);

	// Do not call CWnd::OnPaint() for painting messages
}

void CWndTileMap::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	
	int nWork = m_Dlg.m_CBWork.GetCurSel();

	if(nWork == 0) // 타일 셋 작업이면..
	{
		m_rcSetSel.left = point.x/TILE_SIZE + ptScroll.x;
		m_rcSetSel.top = point.y/TILE_SIZE + ptScroll.y;
	}	
	else if((nWork == 1 || nWork == 2) && _IsKeyDown(VK_SPACE)) // 스크롤 시작
	{
		m_ptLBPrev = point;
		m_ptScrollPrev = ptScroll;
	}
	else if(nWork == 1) // 타일 맵 작업이면..
	{
		if(m_Dlg.GetCheckedRadioButton(IDC_RADIO_NO_EDIT, IDC_RADIO_EVENT_EDIT) != IDC_RADIO_NO_EDIT)
		{
			point.Offset(ptScroll.x*TILE_SIZE, ptScroll.y*TILE_SIZE);
			m_MSM.SetStartPos(point);
			SetCapture();
		}
		else
		{
			CPoint pt(point.x/TILE_SIZE + ptScroll.x, point.y/TILE_SIZE + ptScroll.y);
			if(m_Dlg.m_CBrushErase.GetCheck()) // 지우기 상태면
			{
				int nBrush = m_Dlg.m_CBBrush.GetCurSel();
				TileMapDelete(pt.x, pt.y, nBrush);
				InvalidateRect(NULL, FALSE);
			}
			else
			{
				m_ptLBDown = pt;
				int nBrush = m_Dlg.m_CBBrush.GetCurSel();
				TileMapSet(pt.x, pt.y, nBrush);
				InvalidateRect(NULL, FALSE);
			}
		}
	}
	else if(nWork == 2) // 맵에 객체 배치 작업..
	{
		CPoint pt(point.x/TILE_SIZE + ptScroll.x, point.y/TILE_SIZE + ptScroll.y);
		int nData = m_DlgObj.GetSelectedObjData();
		if(nData >= 0) // 선택된 값이 잘못되어 있으면 -1 리턴.
		{
			m_DlgObj.m_MapObj.DataSet(pt.x, pt.y, nData);
			this->InvalidateRect(NULL, FALSE);
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CWndTileMap::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int nWork = m_Dlg.m_CBWork.GetCurSel();
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	int edit_info = m_Dlg.GetCheckedRadioButton(IDC_RADIO_NO_EDIT, IDC_RADIO_EVENT_EDIT);
	
	if(nWork == 0)
	{
		m_rcSetSel.NormalizeRect();
		
		int nTS = m_Dlg.m_ListTS.GetCurSel();
		int nTSS = m_Dlg.m_ListTSS.GetCurSel();
		int width = m_rcSetSel.Width();
		int height = m_rcSetSel.Height();
		if(width > 0 && height > 0)
		{
			_TILE_SUB_SET_DATA_TOOL* pData = g_pTSet->DataGetSafe(nTS, nTSS);

			if(pData != NULL)
			{
				if(!(nFlags & MK_CONTROL)) pData->Release();
				int index = pData->Alloc(width, height);
				if(index < 0)
				{
					if(index == -1) AfxMessageBox("가로 크기와 세로 크기가 0이상이어야 합니다.", MB_ICONINFORMATION);
					else if(index == -2) AfxMessageBox("애니메이션의 경우 크기가 서로 같아야 합니다.", MB_ICONINFORMATION);
					else if(index == -3) AfxMessageBox("애니메이션 개수는 20개이상 할 수 없습니다.", MB_ICONINFORMATION);
					return;
				}
				for(int y = m_rcSetSel.top, y2 = 0; y < m_rcSetSel.bottom; y++, y2++)
				{
					for(int x = m_rcSetSel.left, x2 = 0; x < m_rcSetSel.right; x++, x2++)
					{
						pData->pnFlags[index][y2*pData->nWidth + x2] = 0;
						pData->pptSrcs[index][y2*pData->nWidth + x2].x = x;
						pData->pptSrcs[index][y2*pData->nWidth + x2].y = y;
					}
				}
			}
		}
		InvalidateRect(NULL, FALSE);
		m_Dlg.UpdateTileSubSetInfo();
		m_Dlg.UpdateTileSetPreviewWindow();
	}
	else if(nWork == 1 && edit_info != IDC_RADIO_NO_EDIT)
	{
		point.Offset(ptScroll.x*TILE_SIZE, ptScroll.y*TILE_SIZE);
		m_MSM.SetEndPos(point);
		ReleaseCapture();
		POINT *p = m_MSM.GetSelectRgn();
		if(edit_info == IDC_RADIO_MOVE_EDIT)
		{
			g_pTMove->SetMovable(p);
			Invalidate(FALSE);
		}
		else if(edit_info == IDC_RADIO_HOUSE_EDIT)
		{
			CHouseNumberDialog dialog(this);

			dialog.SetCaption("집정보 편집");
			if(dialog.DoModal() == IDOK)
			{
				g_pTMove->SetHouse(p, dialog.m_vHouseNumber);
				Invalidate(FALSE);
			}
		}
		else if(edit_info == IDC_RADIO_EVENT_EDIT)
		{
			CHouseNumberDialog dialog(this);

			dialog.SetCaption("이벤트정보 편집");
			if(dialog.DoModal() == IDOK)
			{
				g_pTMove->SetEvent(p, dialog.m_vHouseNumber);
				Invalidate(FALSE);
			}
		}
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CWndTileMap::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	
	// TODO: Add your message handler code here and/or call default
	int nWork = m_Dlg.m_CBWork.GetCurSel();

	int bMoveGrid = (point.x/TILE_SIZE != m_ptMouse.x/TILE_SIZE || point.y/TILE_SIZE != m_ptMouse.y/TILE_SIZE); // 그리드 단위로 움직였는지...

	CRect rcUpdate;
	if(nWork == 0 && (nFlags & MK_LBUTTON))
	{
		m_rcSetSel.right = point.x/TILE_SIZE + ptScroll.x + 1;
		m_rcSetSel.bottom = point.y/TILE_SIZE + ptScroll.y + 1;
		InvalidateRect(NULL, FALSE);
	}
	else if((nWork == 1 || nWork == 2) && _IsKeyDown(VK_SPACE) && (nFlags & MK_LBUTTON)) // 타일맵 작업.. 스페이스 바를 누르고 있으면 스크롤...
	{
		if(m_ptLBPrev.x == 0 && m_ptLBPrev.y == 0)
		{
			m_ptLBPrev = point;
		}
		else
		{
			int nDX = (m_ptLBPrev.x - point.x)/TILE_SIZE;
			int nDY = (m_ptLBPrev.y - point.y)/TILE_SIZE;
			this->OnHScroll(SB_THUMBPOSITION, m_ptScrollPrev.x + nDX, NULL);
			this->OnVScroll(SB_THUMBPOSITION, m_ptScrollPrev.y + nDY, NULL);

			if(nDX || nDY)
			{
				int uu = 0;
			}
		}
	}
	else if(nWork == 1 && (nFlags & MK_LBUTTON) && m_Dlg.GetCheckedRadioButton(IDC_RADIO_NO_EDIT, IDC_RADIO_HOUSE_EDIT) != IDC_RADIO_NO_EDIT)
	{
		point.Offset(ptScroll.x*TILE_SIZE, ptScroll.y*TILE_SIZE);
		m_MSM.MouseMoving(point);
	}
	else if(nWork == 1 && bMoveGrid == TRUE) // 타일맵 작업..
	{
		this->InvalidateRect(&m_rcNeedUpdate, FALSE); // 전의 영역을 다시 그려준다...

		if(m_Dlg.m_CBrushErase.GetCheck() == TRUE) // 지우기 상태면
		{
			int nBrush = m_Dlg.m_CBBrush.GetCurSel();
			CPoint pt(point.x/TILE_SIZE + ptScroll.x, point.y/TILE_SIZE + ptScroll.y);
			if(nFlags & MK_LBUTTON) TileMapDelete(pt.x, pt.y, nBrush);

			rcUpdate.SetRect(	(point.x/TILE_SIZE - nBrush - 1)*TILE_SIZE, 
								(point.y/TILE_SIZE - nBrush - 1)*TILE_SIZE, 
								(point.x/TILE_SIZE + nBrush + 1)*TILE_SIZE,
								(point.y/TILE_SIZE + nBrush + 1)*TILE_SIZE);
		}
		else if(m_Dlg.m_CBrushErase.GetCheck() == FALSE)// 지우기 상태가 아니면
		{
			int nTS = m_Dlg.m_ListTS.GetCurSel();
			int nTSS = m_Dlg.m_ListTSS.GetCurSel();

			_TILE_SUB_SET_DATA_TOOL* pTSSData = g_pTSet->DataGetSafe(nTS, nTSS);
			if(pTSSData)
			{
				if(pTSSData->nWidth > 0 && pTSSData->nHeight > 0) // 타일 셋 데이터의 격자대로 움직임...
				{
					CPoint pt(point.x/TILE_SIZE + ptScroll.x, point.y/TILE_SIZE + ptScroll.y);

					int nBrush = m_Dlg.m_CBBrush.GetCurSel();
					if(nFlags & MK_LBUTTON)
					{
						if((pt.x - m_ptLBDown.x)%pTSSData->nWidth == 0 && (pt.y - m_ptLBDown.y)%pTSSData->nHeight == 0)
							TileMapSet(pt.x, pt.y, nBrush);
					}
					
					int width = (nBrush*2 + 1)*pTSSData->nWidth;
					int height = (nBrush*2 + 1)*pTSSData->nHeight;
					int left = pt.x - ptScroll.x - width/2;
					int top = pt.y - ptScroll.y - height/2;
					rcUpdate.SetRect(	(left - 1)*TILE_SIZE,
										(top - 1)*TILE_SIZE,
										(left + width + 1)*TILE_SIZE,
										(top + height + 1)*TILE_SIZE);
				}
			}
		}

		this->InvalidateRect(&rcUpdate, FALSE);
		m_rcNeedUpdate = rcUpdate; // 담에 업데이트할 영역 저장
	}
	
	if(nFlags & MK_RBUTTON)
	{
		if(nWork == 1 && bMoveGrid == TRUE) // 타일 맵 작업
		{
			CPoint pt(point.x/TILE_SIZE + ptScroll.x, point.y/TILE_SIZE + ptScroll.y);
			static CPoint ptPrev = pt;
			if(m_rcSelect.PtInRect(pt))
			{
				CPoint ptDelta(pt.x - ptPrev.x, pt.y - ptPrev.y);
				m_rcSelect += ptDelta;
			}
			else
			{
				m_rcSelect.right = pt.x;
				m_rcSelect.bottom = pt.y;
			}
			ptPrev = pt;
			InvalidateRect(NULL, FALSE);
		}
		
		SetCursor(m_hCursorDrag);
	}
	
	m_ptMouse = point; // 현재 마우스 위치 기록

	CWnd::OnMouseMove(nFlags, point);
}

void CWndTileMap::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	CPoint pt(point.x/TILE_SIZE + ptScroll.x, point.y/TILE_SIZE + ptScroll.y);
	
	int nWork = m_Dlg.m_CBWork.GetCurSel();
	if(nWork == 1) // 맵 작업일 경우
	{
		if(m_rcSelect.PtInRect(pt))
		{
			int ttt = 0;
		}
		else
		{
			DELETE_ARRAY_SAFE(m_pMDataClip); // 클립 보드 지우고..
			m_rcSelect.left = pt.x;
			m_rcSelect.top = pt.y;
			m_rcSelect.right = pt.x;
			m_rcSelect.bottom = pt.y;
			m_rcCopy = m_rcSelect;
		}
	}
	else if(nWork == 2)
	{
		m_DlgObj.m_MapObj.DataSet(pt.x, pt.y, -1);
	}
	InvalidateRect(NULL, FALSE);
	CWnd::OnRButtonDown(nFlags, point);
}

void CWndTileMap::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	CPoint pt(point.x/TILE_SIZE + ptScroll.x, point.y/TILE_SIZE + ptScroll.y);

	if(m_Dlg.m_CBWork.GetCurSel() == 1) // 타일 맵 작업이면 // 클립 보드에 복사..
	{
		DELETE_ARRAY_SAFE(m_pMDataClip); // 클립 보드 지우고..

//		if(m_rcSelect.PtInRect(pt) == FALSE)
//		{
			m_rcSelect.right = point.x/TILE_SIZE + ptScroll.x;
			m_rcSelect.bottom = point.y/TILE_SIZE + ptScroll.y;
			m_rcSelect.NormalizeRect();

			CRect rcMap(0, 0, g_pTMap->m_HTMap.nWidth, g_pTMap->m_HTMap.nHeight);
			m_rcCopy.IntersectRect(&m_rcSelect, &rcMap); // 겹치는 영역을 찾는다..

			if(m_rcCopy.Width() > 0 && m_rcCopy.Height() > 0)
			{
				m_rcSelect = m_rcCopy;
				int nLayer = m_Dlg.m_LBLayer.GetCurSel();
				m_pMDataClip = new _TILE_MAP_DATA_TOOL[m_rcSelect.Width()*m_rcSelect.Height()];
				for(int y = m_rcCopy.top; y < m_rcCopy.bottom; y++)
				{
					for(int x = m_rcCopy.left; x < m_rcCopy.right; x++)
					{
						_TILE_MAP_DATA_TOOL* pTMData = g_pTMap->ToolDataGet(nLayer, x, y);
						if(pTMData != NULL)	m_pMDataClip[(y - m_rcCopy.top)*m_rcCopy.Width() + x - m_rcCopy.left] = *pTMData;
						else m_pMDataClip[(y - m_rcCopy.top)*m_rcCopy.Width() + x - m_rcCopy.left].snTS = -1;
					}
				}
//			}
//			else
//			{
//				m_rcSelect.SetRect(0,0,0,0);
//				m_rcCopy.SetRect(0,0,0,0);
//			}
		}
	}
	InvalidateRect(NULL, FALSE);
	CWnd::OnRButtonUp(nFlags, point);
}

// 지정된 위치에 선택된 타일과 타일셋을 갖고 세팅... 
BOOL CWndTileMap::TileMapSet(int x, int y, int nBrush)
{
	int nLayer = m_Dlg.m_LBLayer.GetCurSel();
	if(nLayer < 0 || nLayer >= MAX_TILE_MAP_LAYER) return FALSE;

	// 타일셋 및 서브셋 번호 가져오기..
	int nTS = m_Dlg.m_ListTS.GetCurSel();
	int nTSS = m_Dlg.m_ListTSS.GetCurSel();

	if(nTS < 0 || nTS >= MAX_TILE_SET) return FALSE;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return FALSE;

	_TILE_SUB_SET_DATA_TOOL* pTSData = g_pTSet->DataGetSafe(nTS, nTSS);
	if(pTSData == NULL) return FALSE;
	
	CRect rcUpdate;
	rcUpdate.left =	x - pTSData->nWidth/2 - pTSData->nWidth*nBrush - 3;
	rcUpdate.top = y - pTSData->nHeight/2 - pTSData->nWidth*nBrush - 3;
	rcUpdate.right = rcUpdate.left + pTSData->nWidth*(nBrush*2+1) + 3;
	rcUpdate.bottom = rcUpdate.top + pTSData->nHeight*(nBrush*2+1) + 3;
	g_WndTMapPreview.UpdateData(&rcUpdate); // 미니 맵 업데이트
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);

	// 실행 취소용 버퍼 생성
	this->UndoPrepareData(rcUpdate);
	_TILE_MAP_DATA_TOOL* pMData;

	// 브러쉬 크기..
	if(nBrush < 0) nBrush = 0;
	if(nBrush > 4) nBrush = 3;
	for(int ny = -nBrush; ny <= nBrush; ny++)
	{
		for(int nx = -nBrush; nx <= nBrush; nx++)
		{
			CRect rcDest;
			rcDest.left = nx*pTSData->nWidth + x - pTSData->nWidth/2; // 너비에 따른 오프셋(??)
			rcDest.top = ny*pTSData->nHeight + y - pTSData->nHeight/2;
			rcDest.right = rcDest.left + pTSData->nWidth;
			rcDest.bottom = rcDest.top + pTSData->nHeight;

			int sx, sy, dx, dy;
			int t_int;
			for(sy = 0, dy = rcDest.top; dy < rcDest.bottom; dy++, sy++)
			{
				for(sx = 0, dx = rcDest.left; dx < rcDest.right; dx++, sx++)
				{
					pMData = g_pTMap->ToolDataGet(nLayer, dx, dy);
					if(pMData == NULL) continue;

					t_int = pTSData->GetSafe(sx, sy);
					if(t_int == -1) continue;
					
					pMData->snTS = nTS;
					pMData->snTSS = nTSS;
					pMData->snNum = t_int;
					pMData->snFlags = pTSData->pnFlags[0][sy*pTSData->nWidth+sx];
				}
			}
		}
	}

	return TRUE;
}

BOOL CWndTileMap::TileMapDelete(int x, int y, int nBrush)
{
	int nLayer = m_Dlg.m_LBLayer.GetCurSel();
	if(nLayer < 0 || nLayer >= MAX_TILE_MAP_LAYER) return FALSE;

	// 브러쉬 크기..
	if(nBrush < 0) nBrush = 0;
	if(nBrush > 4) nBrush = 3;

	CRect rcUpdate(	x - nBrush - 3,
					y  - nBrush - 3,
					x + nBrush + 3,
					y  + nBrush + 3);
	// 실행 취소용 버퍼 생성
	this->UndoPrepareData(rcUpdate);

	_TILE_MAP_DATA_TOOL* pMData;
	for(int dy = y - nBrush; dy <= y + nBrush; dy++)
	{
		for(int dx = x - nBrush; dx <= x + nBrush; dx++)
		{
			pMData = g_pTMap->ToolDataGet(nLayer, dx, dy);
			if(pMData == NULL) continue;
			
			memset(pMData, 0, sizeof(_TILE_MAP_DATA_TOOL));
			pMData->snNum = -1;
		}
	}
	
	g_WndTMapPreview.UpdateData(&rcUpdate); // 미니 맵 업데이트
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);

	return TRUE;
}

void CWndTileMap::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int nWork = m_Dlg.m_CBWork.GetCurSel();

	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	CRect rc;
	GetClientRect(&rc);
	int nScrDeltaX = rc.Width()/TILE_SIZE/3;
	int nScrDeltaY = rc.Height()/TILE_SIZE/3;

	switch(nChar)
	{
	case VK_DELETE:
		break;
	case VK_LEFT:
		if(nWork == 1 || nWork == 2) SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ptScroll.x - nScrDeltaX), NULL);
		break;
	case VK_RIGHT:
		if(nWork == 1 || nWork == 2) SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ptScroll.x + nScrDeltaX), NULL);
		break;
	case VK_UP:
		if(nWork == 1 || nWork == 2) SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ptScroll.y - nScrDeltaY), NULL);
		break;
	case VK_DOWN:
		if(nWork == 1 || nWork == 2) SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ptScroll.y + nScrDeltaY), NULL);
		break;
	case 'C':
		if(nFlags & MK_CONTROL)
		{
			// 클립 보드에 복사..
		}
		break;
	case 'z':
	case 'Z':
		if(_IsKeyDown(VK_CONTROL))
		{
			this->Undo();
		}
		break;
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CWndTileMap::RecalcScrollBar()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int w = 0, h = 0;

	int nWork = m_Dlg.m_CBWork.GetCurSel();
	if(nWork == 0) // 타일 셋 작업
	{
		int nTS = m_Dlg.m_ListTS.GetCurSel();
		if(nTS >= 0 && nTS < MAX_TILE_SET)
		{
			if(g_pTSet->m_lpDDSs[nTS])
			{
				CRect rc;
				g_pTEng->SurfaceGetSize(g_pTSet->m_lpDDSs[nTS], &rc);
				w = (rc.Width() - rcClient.Width())/TILE_SIZE;
				h = (rc.Height() - rcClient.Height())/TILE_SIZE;
			}
		}
	}
	else if(nWork == 1 || nWork == 2) // 타일 맵 작업 || 오브젝트 작업
	{
		w = g_pTMap->m_HTMap.nWidth - rcClient.Width()/TILE_SIZE;
		h = g_pTMap->m_HTMap.nHeight - rcClient.Height()/TILE_SIZE;
	}
	if(w > 0)
	{
		int nh = GetScrollPos(SB_HORZ);
		EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);
		SetScrollRange(SB_HORZ, 0, w);
		if(w > nh) w = nh;
		SetScrollPos(SB_HORZ, nh);
	}
	else
	{
		EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
		SetScrollRange(SB_HORZ, 0, 0);
		SetScrollPos(SB_HORZ, 0);
	}

	if(h > 0)
	{
		int nv = GetScrollPos(SB_VERT);
		EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);
		SetScrollRange(SB_VERT, 0, h);
		if(h > nv) h = nv;
		SetScrollPos(SB_VERT, nv);
	}
	else
	{
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
		SetScrollRange(SB_VERT, 0, 0);
		SetScrollPos(SB_VERT, 0);
	}
}

void CWndTileMap::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	RecalcScrollBar();

	CRect rcClient, rcWnd;
	GetWindowRect(&rcWnd);
	GetClientRect(&rcClient);

	int w = rcWnd.Width() - rcClient.Width();
	int h = rcWnd.Height() - rcClient.Height();

	BOOL bResize = FALSE;
	if(rcClient.Width() && TILE_SIZE != 0)
	{
		cx = rcClient.Width()/TILE_SIZE*TILE_SIZE + w;
		bResize = TRUE;
	}
	if(rcClient.Height() && TILE_SIZE != 0)
	{
		cy = rcClient.Height()/TILE_SIZE*TILE_SIZE + h;
		bResize = TRUE;
	}

	if(bResize == TRUE)
	{
		SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOOWNERZORDER);
		if(g_WndTMapPreview.GetSafeHwnd() != NULL) g_WndTMapPreview.InvalidateRect(NULL, FALSE);
	}
}

void CWndTileMap::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int min, max;
	GetScrollRange(SB_HORZ, &min, &max);
	int pos = GetScrollPos(SB_HORZ);

	int delta = (max - min)/10;

	switch(nSBCode)
	{
	case SB_LINEUP:
		pos -= 1;
		break;
	case SB_LINEDOWN:
		pos += 1;
		break;
	case SB_PAGEUP:
		pos -= delta;
		break;
	case SB_PAGEDOWN:
		pos += delta;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		pos = nPos;
		break;
	}

	// 스크롤 범위 검사후 세팅
	if(pos < min) pos = min;
	if(pos > max) pos = max;
	SetScrollPos(SB_HORZ, pos);

	InvalidateRect(NULL, FALSE);
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CWndTileMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int min, max;
	GetScrollRange(SB_VERT, &min, &max);
	int pos = GetScrollPos(SB_VERT);

	int delta = (max - min) / 10;

	switch(nSBCode)
	{
	case SB_LINEUP:
		pos -= 1;
		break;
	case SB_LINEDOWN:
		pos += 1;
		break;
	case SB_PAGEUP:
		pos -= delta;
		break;
	case SB_PAGEDOWN:
		pos += delta;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		pos = nPos;
		break;
	}

	// 스크롤 범위 검사후 세팅
	if(pos < min) pos = min;
	if(pos > max) pos = max;
	SetScrollPos(SB_VERT, pos);

	InvalidateRect(NULL, FALSE);
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

int CWndTileMap::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	DELETE_SAFE(g_pTSet); g_pTSet = new CTileSetTool();
	DELETE_SAFE(g_pTMap); g_pTMap = new CTileMapTool();
	DELETE_SAFE(g_pTMove); g_pTMove = new MoveCell();

	m_pMDataClip = NULL;
	
	if(m_Dlg.GetSafeHwnd() == NULL) // 제어 대화상자..
	{
		m_Dlg.Create(IDD_TILE_MAP, this);
	}
	m_Dlg.ShowWindow(SW_SHOW);
	CRect rcWnd;
	this->GetWindowRect(&rcWnd);
	m_Dlg.SetWindowPos(NULL, rcWnd.right, rcWnd.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	
	if(m_DlgObj.GetSafeHwnd() == NULL) // 제어 대화상자..
	{
		m_DlgObj.Create(IDD_TILE_MAP_OBJ, this);
	}
	m_DlgObj.ShowWindow(SW_SHOW);
	m_DlgObj.SetWindowPos(NULL, rcWnd.right + 10, rcWnd.top + 10, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	
	
	if(g_WndTMapPreview.GetSafeHwnd() == NULL) // 맵 프리뷰 윈도우를 만든다.. - 아직 보이지는 않고 툴바에서 호출해야만 보인다.
	{
		HCURSOR hCur = LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_C_NORMAL));
		CString szClass;
		szClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, hCur, NULL, 0);
		
		g_WndTMapPreview.CreateEx(	NULL,
									szClass,
									"Tile Map Editor Window",
									WS_OVERLAPPEDWINDOW,
									CW_USEDEFAULT,
									CW_USEDEFAULT,
									256,
									256,
									this->m_hWnd,
									NULL,
									NULL);
		ShowCursor(TRUE);
		CRect rcAdjust(0, 0, 256, 256);
		AdjustWindowRect(rcAdjust, g_WndTMapPreview.GetStyle(), FALSE);
		g_WndTMapPreview.SetWindowPos(NULL, 0, 0, rcAdjust.Width(), rcAdjust.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	g_WndTMapPreview.ShowWindow(SW_SHOW);
	g_WndTMapPreview.EnableToolTips(TRUE); // 툴팁 가능..
	g_WndTMapPreview.SetFocus();

//	m_hCursorDrag = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_DRAG));

	return 0;
}

void CWndTileMap::OnDestroy() 
{
	CWnd::OnDestroy();

	DELETE_SAFE(g_pTSet);
	DELETE_SAFE(g_pTMap);
	DELETE_SAFE(g_pTMove);

	DestroyCursor(m_hCursorDrag);
	// TODO: Add your message handler code here
}

BOOL CWndTileMap::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	int nWork = m_Dlg.m_CBWork.GetCurSel();
	if(nWork == 0 || nWork == 1 || nWork == 2) // 타일 맵 작업
	{
		if(nFlags & MK_SHIFT) 
		{
			int nPos = this->GetScrollPos(SB_HORZ);
			this->OnHScroll(SB_THUMBPOSITION, nPos - zDelta/WHEEL_DELTA*5, NULL);
		}
		else
		{
			int nPos = this->GetScrollPos(SB_VERT);
			this->OnVScroll(SB_THUMBPOSITION, nPos - zDelta/WHEEL_DELTA*5, NULL);
		}
	}
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CWndTileMap::UndoPrepareData(CRect rc)
{
	if(rc.left < 0) rc.left = 0;
	if(rc.top < 0) rc.top = 0;
	if(rc.right > g_pTMap->m_HTMap.nWidth) rc.right = g_pTMap->m_HTMap.nWidth;
	if(rc.bottom > g_pTMap->m_HTMap.nHeight) rc.bottom = g_pTMap->m_HTMap.nHeight;

	if(rc.Width() <= 0 && rc.Height() <= 0) return;

	m_UndoData.rc = rc;
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		_TILE_MAP_DATA_TOOL* pMData;

		DELETE_ARRAY_SAFE(m_UndoData.pDatas[i]);
		int nAlloc = rc.Width() * rc.Height();
		m_UndoData.pDatas[i] = new _TILE_MAP_DATA_TOOL[nAlloc];
		memset(m_UndoData.pDatas[i], 0, sizeof(_TILE_MAP_DATA)*nAlloc);
		for(int y = rc.top, y2 = 0; y < rc.bottom; y++, y2++)
		{
			for(int x = rc.left, x2 = 0; x < rc.right; x++, x2++)
			{
				pMData = g_pTMap->ToolDataGet(i, x, y);
				if(pMData == NULL) continue;

				m_UndoData.pDatas[i][y2*rc.Width() + x2] = *pMData;
			}
		}
	}
}

void CWndTileMap::Undo()
{
	CRect rc = m_UndoData.rc;
	if(rc.Width() <= 0 && rc.Height() <= 0) return;

	_TILE_MAP_DATA_TOOL* pMData;
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		for(int y = rc.top, y2 = 0; y < rc.bottom; y++, y2++)
		{
			for(int x = rc.left, x2 = 0; x < rc.right; x++, x2++)
			{
				pMData = g_pTMap->ToolDataGet(i, x, y);
				if(pMData == NULL) continue;
				*pMData = m_UndoData.pDatas[i][y2*rc.Width() + x2];
			}
		}
	}
	this->InvalidateRect(NULL, FALSE);
}

void CWndTileMap::UnoClearData()
{
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		DELETE_ARRAY_SAFE(m_UndoData.pDatas[i]);
	}
	MAKE_ZERO(m_UndoData);
}

void CWndTileMap::ActiveAni()
{
	m_bAniMode = TRUE;
	SetTimer(0, 300, NULL);
}

void CWndTileMap::DeactiveAni()
{
	m_bAniMode = FALSE;
	g_pTSet->ResetAni();
	KillTimer(0);
	Invalidate(FALSE);
}

POINT PosToCell(POINT p)
{
	int k1, k2;
	k1 = (int)ceil((.5*p.x+p.y-16)/32);
	k2 = (int)ceil((-.5*p.x+p.y+16)/32);
	
	k1 = 32*k1 + 16;
	k2 = 32*k2 - 16;
	
	p.x = k1 - k2;
	p.y = (k1+k2)/2-16;
	
	p.x = int(p.x/16.+.5)*16;
	p.y = int(p.y/16.+.5)*16;
	
	p.x /= 16;
	p.y /= 16;
	p.x /= 2;
	p.x--;
	p.y--;

	return p;
}

POINT CellToPos(POINT p)
{
	p.x = p.x * 32 + 32;
	p.y = p.y * 16 + 16;

	return p;
}

void CWndTileMap::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0)
	{
		AniPaint();
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CWndTileMap::AniPaint()
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));

	CClientDC dc(this); // device context for painting
	CRect rcPaint;
	GetClientRect(&rcPaint);

	g_pTEng->SurfaceFillRect(NULL, &rcPaint);
	if(m_Dlg.GetSafeHwnd() == NULL)
	{
		g_pTEng->Flip(&rcPaint);
		return;
	}

	int nWork = m_Dlg.m_CBWork.GetCurSel();
	if(nWork == 1 || nWork == 2) // 타일 맵 작업, 오브젝트 작업일 경우...
	{
		CRect rcD(	rcPaint.left/TILE_SIZE + ptScroll.x,
					rcPaint.top/TILE_SIZE + ptScroll.y,
					rcPaint.right/TILE_SIZE + 1 + ptScroll.x,
					rcPaint.bottom/TILE_SIZE + 1 + ptScroll.y);
		if(rcD.right > g_pTMap->m_HTMap.nWidth) rcD.right = g_pTMap->m_HTMap.nWidth;
		if(rcD.bottom > g_pTMap->m_HTMap.nHeight) rcD.bottom = g_pTMap->m_HTMap.nHeight;

		int nCurLayer = m_Dlg.m_LBLayer.GetCurSel();
		if(nCurLayer < 0 || nCurLayer >= MAX_TILE_MAP_LAYER) return; // 레이어...

		BOOL bUCK = FALSE; // 투명찍기인가?
		int nRenderType = RENDER_NORMAL;
			
		LPDIRECTDRAWSURFACE lpDDS;
		g_pTSet->Increase();
		for(int nLayer = 0; nLayer < MAX_TILE_MAP_LAYER; nLayer++)
		{
			if(m_Dlg.m_LBLayer.GetItemData(nLayer) == TRUE) continue; // 레이어 숨기기면 보이지 않는다.

			int x, y;
			for(y = rcD.top; y < rcD.bottom; y++) // 바닥에 깔리는 맵
			{
				for(x = rcD.left; x < rcD.right; x++)
				{
					const _TILE_MAP_DATA_TOOL* pTMData = g_pTMap->ToolDataGet(nLayer, x, y);
					if(pTMData == NULL) continue;
					if(pTMData->snTS < 0 || pTMData->snTS >= MAX_TILE_SET) continue;
					lpDDS = g_pTSet->m_lpDDSs[pTMData->snTS];
					if(lpDDS == NULL) continue;
					POINT *t_pos = g_pTSet->DataGetPosCur(pTMData->snTS, pTMData->snTSS, pTMData->snNum);
					int *t_flag = g_pTSet->DataGetFlagCur(pTMData->snTS, pTMData->snTSS, pTMData->snNum);

					if(t_pos == NULL || t_flag == NULL ||
						t_pos->x < 0 ||
						t_pos->x >= MAX_TILE_SET_WIDTH ||
						t_pos->y < 0 ||
						t_pos->y >= MAX_TILE_SET_HEIGHT) continue; // 쓰지 않는 타일이면 지나간다..

					CRect rcSrc;
					rcSrc.left = t_pos->x * TILE_SIZE;
					rcSrc.top = t_pos->y * TILE_SIZE;
					rcSrc.right = rcSrc.left + TILE_SIZE;
					rcSrc.bottom = rcSrc.top + TILE_SIZE;
					
					bUCK = FALSE;
					nRenderType = RENDER_NORMAL;

					if(nLayer == 0) // 기본 깔리는 맵
						bUCK = FALSE;
					else if(nLayer == 1) // 겹치는 맵
						bUCK = TRUE;
					else if(nLayer == 2) // 그림자 레이어
					{
						bUCK = TRUE;
						nRenderType = RENDER_SHADOW;
					}
					else if(nLayer == 3 || nLayer == 4)
					{
						if(*t_flag & 2) bUCK = TRUE; // 투명 찍기면
						else bUCK = FALSE;
					}
					
					g_pTEng->BltNormal(lpDDS, (x-ptScroll.x)*TILE_SIZE, (y-ptScroll.y)*TILE_SIZE, rcSrc, bUCK, nRenderType, 16);
				}
			}
		}
	}

	// 그리드 그리기..
	if(m_Dlg.IsDlgButtonChecked(IDC_CHECK_VIEW_GRID))
	{
		int x1 = rcPaint.left/TILE_SIZE;
		int y1 = rcPaint.top/TILE_SIZE;
		int x2 = rcPaint.right/TILE_SIZE;
		int y2 = rcPaint.bottom/TILE_SIZE;

		for(int x = x1; x <= x2; x++)
		{
			g_pTEng->DrawLine(NULL, x*TILE_SIZE, rcPaint.top, x*TILE_SIZE, rcPaint.bottom, 0x0000ff00);
		}
		for(int y = y1; y <= y2; y++)
		{
			g_pTEng->DrawLine(NULL, rcPaint.left, y*TILE_SIZE, rcPaint.right, y*TILE_SIZE, 0x0000ff00);
		}
	}

	g_pTEng->HWndSet(this->m_hWnd);
	g_pTEng->Flip(&rcPaint);
}