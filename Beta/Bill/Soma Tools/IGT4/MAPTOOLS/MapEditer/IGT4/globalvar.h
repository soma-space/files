#if !defined(AFX_GLOBALVAR_H__)
#define AFX_GLOBALVAR_H__

#include "DDEngineTool4.h"
#include "AniObjTool.h"
#include "IntfTool.h"
#include "TileMapTool.h"
#include "MoveCell.h"
#include "TileSetTool.h"
#include "WndAniObj.h"
#include "WndTileMap.h"
#include "WndTileMapPreview.h"
#include "WndIntf.h"
#include "DlgScript.h"
#include "DlgSprMerge.h"
#include "DlgFolderSelect.h"
#include "DlgBMPConvert.h"
#include "DlgLook.h"
#include "DlgItem.h"
#include "DlgColorBltTest.h"
#include "DlgResrc.h"

extern CDDEngineTool4*		g_pTEng;
extern CAniObjTool*			g_pAniObj;
extern CTileMapTool*		g_pTMap;
extern MoveCell*			g_pTMove;
extern CTileSetTool*		g_pTSet;
extern CWndAniObj			g_WndAni;
extern CWndTileMap			g_WndTMap;
extern CWndTileMapPreview	g_WndTMapPreview;
extern CWndIntf				g_WndIntf;
extern CDlgScript			g_DlgScript;
extern CDlgLook				g_DlgLook;
extern CDlgItem				g_DlgItem;
extern CIntfTool*			g_pIntf;
extern CDlgColorBltTest		g_DlgCBT;
extern CDlgResrc			g_DlgResrc;

#define MOVE_WIDTH			64
#define MOVE_HEIGHT			32

#endif // #define AFX_GLOBALVAR_H__