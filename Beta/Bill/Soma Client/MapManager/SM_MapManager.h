// SM_MapManager.h: interface for the SM_MapManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SM_MAPMANAGER_H__0CAC0DD9_11ED_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_SM_MAPMANAGER_H__0CAC0DD9_11ED_11D3_8422_0020AF9F40BD__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSgrRes;
typedef CTypedPtrArray <CPtrArray, CSgrRes*> AP_SGRRES;

class CMapCell;
typedef CTypedPtrArray <CPtrArray, CMapCell*> APA_MAPCELL;

class CSpriteUnit;
typedef CTypedPtrArray <CPtrArray, CSpriteUnit*> AP_SPRITEUNIT;

typedef CTypedPtrArray <CPtrArray, CPoint*> AP_USERPOS;

class CDirectDraw;
class CDDSurface;


//전체 맵 관리자
class AFX_EXT_CLASS SM_MapManager  
{
public:
	CPoint					PixelToCell(CPoint ptPixel);	//Pixel좌표를 32X32의 셀 좌표로...
	CPoint					CellToPixel(CPoint ptCell);		//32X32의 셀좌표를 pixel좌표로

	//새로운 메모리를 할당하게 되므로 반드시 부른곳에서 제거해 주어야함.
	BYTE**					GetMoveableTable(int &dimx, int &dimy);	//이동정보로만 구성된 새로운 Set 만들기
	short					**GetHouseTable(int &dimx, int &dimy);	//건물정보로만 구성된 새로운 Set 만들기

	int						SetDarkLevel(int iPercent, BOOL bAbsolute=TRUE);	//맵의 어둡기 정도 선택: bAbsolute이면 절대 밝기, 아니면 현재 밝기의 상대적 밝기
	CPoint					SetCenterPos(CPoint pos);	//Map의 시작위치를 Pixel단위로 return함 : 맵그리기중 중심이 될 위치 설정(자신의 아이디 위치가 중심이 됨)
	CPoint					GetMapStartPos();			//Map의 시작위치를 Pixel단위로 return함
	void					DrawPreSprite(CDDSurface* pDestSurf);	//캐릭터보다 먼저 그릴 Sprite그리기
	void					DrawPostSprite(CDDSurface* pDestSurf);	//캐릭터보다 나중에 그릴 Sprite그리기
	void					DrawAllSprite(CDDSurface* pDestSurf, int Number);	//0509 새롭게 구조를 바꾸는 함수 위의 함수 두개를 대체..

	void					DecidePrePostSprite();					//먼저, 혹은 나중에 그릴것 선택
	BOOL					IsPreCondition(CRect r, CSpriteUnit *pSpriteUnit);	//먼저 그릴 조건인가? (pSpriteUnit)
	BOOL					IsPostCondition(CRect r, CSpriteUnit *pSpriteUnit); //나중에 그릴 조건이가? (pSpriteUnit)

	CPoint					DrawTile(CDDSurface* pDestSurf);		//타일 그리기 : 좌표의 기준은 SetCenterPos에 의해 미리 설정된 상태
	BOOL					LoadMap(CString szFileName, BOOL bFileNameContainPath=FALSE); //Map파일을 Load함 (mmf파일 로드: 자동으로 필요한 Sgr파일도 로드함)
	BOOL					GetMapStatus() {return m_bLoadStatusOk;}	//Load가 정상적으로 이루어 졌는가?
	void					InitManager(CDirectDraw* pDD, HWND hwnd);	//MapManager초기화 (DirectDraw와 MainWindow의 Handle
	void					EndManager();								//MapManager사용 종료

	//Sgr관련루틴
	CString					GetSgrFileName(int index);					//지정한 Index에 해당하는 Sgr파일의 이름은?
	BOOL					LoadSgr(int index);							//지정한 Index의 Sgr파일 로드
	CSgrRes*				GetSgrRes(int index);						//Index를 이용한 CSgrRes클래스 가져오기

	SM_MapManager();
	virtual ~SM_MapManager();

protected:
	BOOL					IsAnimationTime(int iAnimationMode);		//맵을 Animation시킬 시간인가?
	BOOL					IsInSgrList(CList<int, int> &SgrList, int iSgrIdx); //iSgrIdx를 SgrList에 가지고 있는가?
	BOOL					InitializeSgrs(CList<int, int> &SgrList);	//주어진 SgrList를 이용해서 SgrResource를 정리(없으면 Load하고, 있는데 List에 없으면 Unload)...
	BOOL					LoadSpriteUnit(CArchive &ar);				//SpriteUnit 을 Load함
	void					RemoveSpriteUnit();							//SpriteUnit 을 제거함
	void					RemoveMapCellData();						//MapCell데이터 제거
	void					SetMapDimension(CSize dimMap);				//맵의 Dimension설정하고 그만큼 MapCell생성


public:
	BYTE GetAreaNumber(CPoint posCell);
	int  IsHouse(CPoint PosCell);
	void ShowMapType(int type);
	void SetUserPos(CPoint UserPos, int index);
	BOOL IsHidden(CPoint PosCell);
	CSize GetMapDimension();
	BOOL LoadSmf(CString szFileName, BOOL bFileNameContainPath);// 
	BOOL LoadImf(CString szFileName, BOOL bFileNameContainPath);//imf..맵데이타를 읽어 들인다.

	static	CDirectDraw*	sm_bpDirectDraw;							//DiretDraw 객체의 백업 포인터 보관장소
	static	HWND			sm_bRelatedHwnd;							//연결된 MainWindow Handle

	PBYTE*                  m_ppTable;
	PBYTE*                  m_ppHiddenTable;
	PBYTE*                  m_ppEventTable;
	PBYTE*					m_ppAreaNumber;
	short					**m_ppHouseTable;
	int						m_cell_width;								// 실제 맵 사이즈..
	int						m_cell_height;

	int                     m_View_Flag;// 풀화면인지 작은 화면인지 플레그 세팅을 하게한다. 0 - 풀화면 1 - 왼쪽 작은화면 2 - 오른쪽 작은화면
	AP_USERPOS				m_apUsersPos;//Pixel 단위					// 화면상에 있는 유저들의 위치들 스프라이트에 가리는 문제 해결
	CPoint                  m_UsersPos[500];// 한 화면상에 나오는 캐릭터 위치 저장..스프라이트 가리는 문제때문에..
	int                     m_UserSize;// 위의 배열에 몇명이 들어 있는지 체크..

	int                     m_PreSprCount;
	int                     m_PostSprCount;
	int                     m_Spr_Y[500];
	int                     m_Spr_Sky[500];
//	int						m_PreSprNumber[500];// 임의로 500
//	int						m_PostSprNumber[500];


protected:
public:
	CString					m_szExePath;								//실행 디렉토리
	AP_SGRRES				m_apSgrRes;									//SgrRes의 보관장소
	APA_MAPCELL				m_apaMapCell;								//MapCell의 보관장소(Dimension크기)
	AP_SPRITEUNIT			m_apSpriteUnit;								//SpriteUnit의 보관장소(Sgr의 Sprite와는 다름, 맵에디터에서 스프라이트를 찍은 위치등을 기록한 Sprite정보)
	AP_SPRITEUNIT			m_apPreDrawSpriteUnit;						//Backup용 포인터 담을 장소(먼저 그리기)
	AP_SPRITEUNIT			m_apPostDrawSpriteUnit;						//Backup용 포인터 담을 장소(나중 그리기)
	CSize					m_dimMap;									//맵의 크기// 다이아몬드 맵 크기 같음..음냐..이상타.
	CString					m_szMapName;								//맵의 이름(*.mmf)
	BOOL					m_bLoadStatusOk;							//Load후의 상태
	CPoint					m_posCenter;//Pixel 단위					//그리기 중심이 될 위치(캐릭터의 Center위치)
	CPoint					m_MapStartIndex;							//m_posCenter를 기준으로 했을때 LeftTop에 그려질 Cell의 Index
	CPoint					m_offsetMap;								//도트 상으로 MapStartIndex를 얼마나 움직여서 찍어야 하는가? (맵은 cell단위로 찍히는 것이 아니라 Pixel단위로 찍힘: m_posCenter기준)
	DWORD					m_dwTickAniTileStart;						//Tile Animation 시간 척도
	DWORD					m_dwTickAniSpriteStart;						//Sprite Animation 시간 척도
	int						m_iAbsDarkLevel;							//절대적인 밝기(100: 원색 50: 밝기 반으로 줄임)
};

#endif // !defined(AFX_SM_MAPMANAGER_H__0CAC0DD9_11ED_11D3_8422_0020AF9F40BD__INCLUDED_)
