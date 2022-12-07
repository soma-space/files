// TileMapTool.cpp: implementation of the CTileMapTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileMapTool.h"
#include "..\\IGObject\\Dib16.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

typedef struct {
	DWORD tsn	:	10;		// 1023개
	DWORD tssn	:	6;		// 63개
	DWORD num	:	12;		// 4096개(64X64)
	DWORD flag	:	4;
} DARK_TMD_FORMAT;

//////////////////////////////////////////////////////////////////////
// ConstrsnSrction/DestrsnSrction
//////////////////////////////////////////////////////////////////////

CTileMapTool::CTileMapTool()
{
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++) // 왕창 할당..
	{
		m_hDatasTool[i] = NULL;
		m_pDatasTool[i] = NULL;
	}
}

CTileMapTool::~CTileMapTool()
{
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++) // 몽땅 해제..
	{
		GlobalUnlock(m_hDatasTool[i]);
		GlobalFree(m_hDatasTool[i]);
		m_hDatasTool[i] = NULL;
		m_pDatasTool[i] = NULL;
	}
}

_TILE_MAP_DATA_TOOL* CTileMapTool::ToolDataGet(int nLayer, int x, int y)
{
	if(nLayer < 0 || nLayer >= MAX_TILE_MAP_LAYER) return NULL;
	if(x < 0 || x >= m_HTMap.nWidth) return NULL;
	if(y < 0 || y >= m_HTMap.nHeight) return NULL;

	return &m_pDatasTool[nLayer][y*m_HTMap.nWidth + x];
}

void CTileMapTool::New()
{
	this->Release();
	this->ReleaseEx();
}

void CTileMapTool::LoadFromToolData(const char* szFileName)
{
	this->Release();
	this->ReleaseEx();

	CFile file1, file2;
	CString szSprFileName;
	DARK_TMD_FORMAT _tmd;
	int map_size;
	DWORD table_count, core_size;
	DWORD offset, count;
	DWORD *st_buf;
	DWORD i, j, k, l;

	szSprFileName = szFileName;
	szSprFileName = szSprFileName.Left(szSprFileName.GetLength()-1);
	szSprFileName += '1';
	if(file1.Open(szFileName, CFile::modeRead) == FALSE) return;
	if(file2.Open(szSprFileName, CFile::modeRead) == FALSE)
	{
		file1.Close();
		return;
	}

	file1.Read(&m_HTMap, sizeof(TILE_MAP_HEADER));

	this->AllocTool(m_HTMap.nWidth, m_HTMap.nHeight); // 할당

	map_size = m_HTMap.nHeight*m_HTMap.nWidth;
	file1.Read(&core_size, sizeof(DWORD));
	st_buf = new DWORD[core_size];

	file1.Read(st_buf, sizeof(DWORD)*core_size);

	offset = 0;
	for(i = 0; i < 2; i++) // 툴에서 쓴 데이터 저장...
	{
		table_count = st_buf[offset++];
		k = 0;
		for(j = 0; j < table_count; j++)
		{
			count = st_buf[offset++];
			for(l = 0; l < count; l++, k++)
			{
				m_pDatasTool[i][k].snTS = -1;
				m_pDatasTool[i][k].snTSS = -1;
				m_pDatasTool[i][k].snNum = -1;
				m_pDatasTool[i][k].snFlags = 0;
			}
			count = st_buf[offset++];
			for(l = 0; l < count; l++, k++)
			{
				CopyMemory(&_tmd, &st_buf[offset++], 4);
				m_pDatasTool[i][k].snTS = _tmd.tsn;
				m_pDatasTool[i][k].snTSS = _tmd.tssn;
				m_pDatasTool[i][k].snNum = _tmd.num;
				m_pDatasTool[i][k].snFlags = _tmd.flag;
			}
		}
	}
	delete[] st_buf;
	file1.Close();

	file2.Read(&core_size, sizeof(DWORD));
	st_buf = new DWORD[core_size];

	file2.Read(st_buf, sizeof(DWORD)*core_size);

	offset = 0;
	for(i = 2; i < MAX_TILE_MAP_LAYER; i++) // 툴에서 쓴 데이터 저장...
	{
		table_count = st_buf[offset++];
		k = 0;
		for(j = 0; j < table_count; j++)
		{
			count = st_buf[offset++];
			for(l = 0; l < count; l++, k++)
			{
				m_pDatasTool[i][k].snTS = -1;
				m_pDatasTool[i][k].snTSS = -1;
				m_pDatasTool[i][k].snNum = -1;
				m_pDatasTool[i][k].snFlags = 0;
			}
			count = st_buf[offset++];
			for(l = 0; l < count; l++, k++)
			{
				CopyMemory(&_tmd, &st_buf[offset++], 4);
				m_pDatasTool[i][k].snTS = _tmd.tsn;
				m_pDatasTool[i][k].snTSS = _tmd.tssn;
				m_pDatasTool[i][k].snNum = _tmd.num;
				m_pDatasTool[i][k].snFlags = _tmd.flag;
			}
		}
	}
	delete[] st_buf;
	file2.Close();
}

void CTileMapTool::SaveToToolData(const char *szFileName)
{
	CFile file1, file2;
	CString szSprFileName;
	DARK_TMD_FORMAT _tmd;
	DWORD *st_buf;
	int map_size;
	DWORD table_count, count, *t_buf;
	DWORD offset;
	DWORD y, old_y, i, j, t_src;

	szSprFileName = szFileName;
	szSprFileName = szSprFileName.Left(szSprFileName.GetLength()-1);
	szSprFileName += '1';
	if(file1.Open(szFileName, CFile::modeCreate | CFile::modeWrite) == FALSE) return;
	if(file2.Open((LPCTSTR)szSprFileName, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		file1.Close();
		return;
	}

	file1.Write(&m_HTMap, sizeof(TILE_MAP_HEADER));

	map_size = m_HTMap.nHeight*m_HTMap.nWidth;
	st_buf = new DWORD[MAX_TILE_MAP_LAYER*map_size*2];

	offset = 0;
	for(i = 0; i < 2; i++) // 툴에서 쓴 데이터 저장...
	{
		t_buf = st_buf+offset;
		offset++;								// table count를 적기 위한 공간
		y = 0;
		table_count = 0;
		while(1)
		{
			table_count++;
			count = 0;
			for(; y < (DWORD)map_size; y++)
			{
				if(m_pDatasTool[i][y].snTS != -1) break;
				else count++;
			}
			st_buf[offset++] = count;
			old_y = y;

			count = 0;
			for(; y < (DWORD)map_size; y++)
			{
				if(m_pDatasTool[i][y].snTS == -1) break;
				else count++;
			}
			st_buf[offset++] = count;
			for(j = old_y; j < count+old_y; j++)
			{
				t_src = i*map_size+j;
				_tmd.tsn = m_pDatasTool[i][j].snTS;
				_tmd.tssn = m_pDatasTool[i][j].snTSS;
				_tmd.num = m_pDatasTool[i][j].snNum;
				_tmd.flag = m_pDatasTool[i][j].snFlags;
				CopyMemory(&st_buf[offset++], &_tmd, sizeof(DWORD));
			}
			if(y >= (DWORD)map_size) break;
		}
		*t_buf = table_count;
	}
	file1.Write(&offset, sizeof(DWORD));
	if(offset > 0)
	{
		file1.Write(st_buf, sizeof(DWORD)*offset);
	}
	file1.Close();

	offset = 0;
	for(i = 2; i < MAX_TILE_MAP_LAYER; i++) // 툴에서 쓴 데이터 저장...
	{
		t_buf = st_buf+offset;
		offset++;								// table count를 적기 위한 공간
		y = 0;
		table_count = 0;
		while(1)
		{
			table_count++;
			count = 0;
			for(; y < (DWORD)map_size; y++)
			{
				if(m_pDatasTool[i][y].snTS != -1) break;
				else count++;
			}
			st_buf[offset++] = count;
			old_y = y;

			count = 0;
			for(; y < (DWORD)map_size; y++)
			{
				if(m_pDatasTool[i][y].snTS == -1) break;
				else count++;
			}
			st_buf[offset++] = count;
			for(j = old_y; j < count+old_y; j++)
			{
				t_src = i*map_size+j;
				_tmd.tsn = m_pDatasTool[i][j].snTS;
				_tmd.tssn = m_pDatasTool[i][j].snTSS;
				_tmd.num = m_pDatasTool[i][j].snNum;
				_tmd.flag = m_pDatasTool[i][j].snFlags;
				CopyMemory(&st_buf[offset++], &_tmd, sizeof(DWORD));
			}
			if(y >= (DWORD)map_size) break;
		}
		*t_buf = table_count;
	}
	file2.Write(&offset, sizeof(DWORD));
	if(offset > 0)
	{
		file2.Write(st_buf, sizeof(DWORD)*offset);
	}
	delete[] st_buf;
	file2.Close();
}

void CTileMapTool::SaveToGameFormat(const char* szFileName, COLORREF crCK)
{
	g_pTEng->PathSetCur("MAP");
	CFile file;
	if(file.Open(szFileName, CFile::modeCreate | CFile::modeWrite) == FALSE) return;

	TILE_MAP_HEADER HMapOld = m_HTMap; // 헤더 저장...
	this->Alloc(m_HTMap.nWidth, m_HTMap.nHeight); // 게임용 데이터 준비...
	m_HTMap = HMapOld;
	int nTS, nTSS, nIndex;
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++) // 툴에서 쓴 데이터 저장...
	{
		HMapOld.nResrcQts[i] = 0;
		m_HTMap.nResrcQts[i] = 0;

		// 증복되는 타일셋 기록..
		int nEncodeQt = 0;
		_TILE_MAP_DATA_TOOL* pEncodeBuff = new _TILE_MAP_DATA_TOOL[10240];
		memset(pEncodeBuff, 0, sizeof(_TILE_MAP_DATA_TOOL)*10240);

		for(int y = 0; y < m_HTMap.nHeight; y++)
		{
			for(int x = 0; x < m_HTMap.nWidth; x++)
			{
				nTS = m_pDatasTool[i][y*m_HTMap.nWidth+x].snTS;
				nTSS = m_pDatasTool[i][y*m_HTMap.nWidth+x].snTSS;
				nIndex = m_pDatasTool[i][y*m_HTMap.nWidth+x].snNum;

				m_pnDatas[i][y*m_HTMap.nWidth+x] = -1;
				if(nTS < 0 || nTS >= MAX_TILE_SET || nIndex < 0) continue;

				m_pnDatas[i][y*m_HTMap.nWidth+x] = 0;
				m_pnDatas[i][y*m_HTMap.nWidth+x] |= m_pDatasTool[i][y*m_HTMap.nWidth+x].snFlags << 12; // 플래그로 상위 4바이트 만을 쓴다.. 

				BOOL bSame = FALSE;
				for(int z = 0; z < m_HTMap.nResrcQts[i]; z++)
				{
					// 데이터가 같다면..
					if(	m_pDatasTool[i][y*m_HTMap.nWidth+x].snTS == pEncodeBuff[z].snTS &&
						m_pDatasTool[i][y*m_HTMap.nWidth+x].snTSS == pEncodeBuff[z].snTSS &&
						m_pDatasTool[i][y*m_HTMap.nWidth+x].snNum == pEncodeBuff[z].snNum)
					{
						bSame = TRUE;
						break;
					}
				}

				if(bSame == FALSE) // 인덱스 테이블에 없다면.. 인덱스 테이블에 추가..
				{
					pEncodeBuff[m_HTMap.nResrcQts[i]] = m_pDatasTool[i][y*m_HTMap.nWidth+x];
					m_pnDatas[i][y*m_HTMap.nWidth+x] |= m_HTMap.nResrcQts[i]; // 하위 12 바이트가 실제 데이터
//					m_pnDatas[i][y*m_HTMap.nWidth+x].snFlags = ???
					m_HTMap.nResrcQts[i]++;
				}
				else // 인덱스 테이블에 있다면..
				{
					m_pnDatas[i][y*m_HTMap.nWidth+x] |= z;
//					m_pnDatas[i][y*m_HTMap.nWidth+x].snFlags = ???
				}
			}
		}

		// 맵 헤더 저장..
		m_HTMap.crCKs[i] = crCK; // 컬러키 저장
		CString szResrc = szFileName;
		szResrc = szResrc.Left(szResrc.GetLength() - 4);
		szResrc += '1' + i;
		szResrc += ".obm";
		lstrcpy(m_HTMap.szBMPFNs[i], szResrc); // 비트맵 파일 이름 저장..

		// 리소스 저장.. 32 개가 한줄..
		CDib16 dib; 
		int w = TILE_SIZE*32, h = (m_HTMap.nResrcQts[i]/32 + 1)*TILE_SIZE;
		dib.New(w, h);
		WORD* pwDest;

		DDSURFACEDESC ddsd;
		int nPitch;
		WORD* pwSrc;
		RECT rcSrc;
		POINT *t_pos;
		for(int j = 0; j < m_HTMap.nResrcQts[i]; j++)
		{
			nTS = pEncodeBuff[j].snTS;
			if(g_pTSet->m_lpDDSs[nTS] == NULL) continue; // 서피스가 없으면 통과...

			t_pos = g_pTSet->DataGetPos(pEncodeBuff[j].snTS, pEncodeBuff[j].snTSS, pEncodeBuff[j].snNum);
			if(t_pos == NULL) continue;
			SetRect(&rcSrc, t_pos->x*TILE_SIZE,
							t_pos->y*TILE_SIZE,
							t_pos->x*TILE_SIZE + TILE_SIZE,
							t_pos->y*TILE_SIZE + TILE_SIZE);
			CRect rcBMP;
			CPoint ptSrc(t_pos->x*TILE_SIZE, t_pos->y*TILE_SIZE);
			g_pTEng->SurfaceGetSize(g_pTSet->m_lpDDSs[nTS], &rcBMP);
			if(rcBMP.PtInRect(ptSrc) == FALSE)
				continue;

			g_pTEng->SurfaceLock(g_pTSet->m_lpDDSs[nTS], &ddsd, &rcSrc);
			pwSrc = (WORD*)ddsd.lpSurface;
			nPitch = ddsd.lPitch/2;
			
			if(pwSrc == NULL) continue;

			pwDest = dib.Lock();
			WORD* pwDest2;
			for(int y2 = 0; y2 < TILE_SIZE; y2++)
			{
				pwDest2 = pwDest + ((j/32)*TILE_SIZE + y2)*w + (j%32)*TILE_SIZE;
				if(g_pTEng->PixelFormatGet() == 555) g_pTEng->Conv555PixelTo565Pixel(pwDest2, pwSrc + y2*nPitch, TILE_SIZE);
				else memcpy(pwDest2, pwSrc + y2*nPitch, TILE_SIZE*2);
			}
			dib.Unlock();

			g_pTEng->SurfaceUnlock(g_pTSet->m_lpDDSs[nTS], &rcSrc);
		}
		g_pTEng->PathSetCur("BMP");
		dib.Save(m_HTMap.szBMPFNs[i]);

		DELETE_ARRAY_SAFE(pEncodeBuff);
	}
	
	m_HTMap.szID[0] = 'G'; m_HTMap.szID[1] = 'M'; m_HTMap.szID[2] = 'D'; m_HTMap.szID[3] = MAX_TILE_MAP_LAYER;
	file.Write(&m_HTMap, sizeof(TILE_MAP_HEADER));
	for(i = 0; i < MAX_TILE_MAP_LAYER; i++) // 게임 데이터 저장..
	{
		for(int y = 0; y < m_HTMap.nHeight; y++)
		{
			file.Write(&m_pnDatas[i][y*m_HTMap.nWidth], sizeof(short int)*m_HTMap.nWidth);
		}
	}
	file.Close();

	this->Release();
	m_HTMap = HMapOld;
}

void CTileMapTool::ReAlloc(int nWidth, int nHeight)
{
	if(nWidth <= 0 || nWidth > MAX_TILE_MAP_WIDTH || nHeight <= 0 || nHeight > MAX_TILE_MAP_HEIGHT ) return;

	HGLOBAL					hTmpOld[MAX_TILE_MAP_LAYER];
	_TILE_MAP_DATA_TOOL*	pTmpOld[MAX_TILE_MAP_LAYER];
	int nOldW = m_HTMap.nWidth;
	int nOldH = m_HTMap.nHeight;

	int i = 0;

	if(nOldW > 0 && nOldH > 0) // 전의 데이터 포인터를 저장해 놓는다.
	{
		for(i = 0; i < MAX_TILE_MAP_LAYER; i++)
		{
			hTmpOld[i] = m_hDatasTool[i];
			pTmpOld[i] = m_pDatasTool[i];
		}
	}

	this->AllocTool(nWidth, nHeight);

	int w = 0, h = 0;
	if(nOldW < nWidth) w = nOldW; else w = nWidth;
	if(nOldH < nHeight) h = nOldH; else h = nHeight;

	if(h > 0 && w > 0) // 전의 맵 데이터 복사
	{
		for(i = 0; i < MAX_TILE_MAP_LAYER; i++)
		{
			for(int y = 0; y < h; y++)
			{
				memcpy(&m_pDatasTool[i][y*nWidth], &pTmpOld[i][y*nOldW], w*sizeof(_TILE_MAP_DATA_TOOL));
			}

			GlobalUnlock(hTmpOld[i]); // 해제
			GlobalFree(hTmpOld[i]);
			hTmpOld[i] = NULL;
			pTmpOld[i] = NULL;
		}
	}
}

void CTileMapTool::ReleaseEx()
{
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++) // 왕창 할당..
	{
		GlobalUnlock(m_hDatasTool[i]);
		GlobalFree(m_hDatasTool[i]);
		m_hDatasTool[i] = NULL;
		m_pDatasTool[i] = NULL;
	}

	MAKE_ZERO(m_HTMap);
}

void CTileMapTool::AllocTool(int nWidth, int nHeight)
{
	if(nWidth <= 0 || nHeight <= 0) return;
	
	m_HTMap.nWidth = nWidth;
	m_HTMap.nHeight = nHeight;
	
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++) // 할당
	{
		int nAllocSize = nWidth * nHeight * sizeof(_TILE_MAP_DATA_TOOL);
		m_hDatasTool[i] = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, nAllocSize);
		m_pDatasTool[i] = (_TILE_MAP_DATA_TOOL*) GlobalLock(m_hDatasTool[i]);
		memset(m_pDatasTool[i], 0, nAllocSize);
		for(int y = 0; y < nHeight; y++)
		{
			for(int x = 0; x < nWidth; x++)
			{
				m_pDatasTool[i][y*nWidth+x].snFlags = 0;
				m_pDatasTool[i][y*nWidth+x].snTS = -1;
				m_pDatasTool[i][y*nWidth+x].snTSS = -1;
				m_pDatasTool[i][y*nWidth+x].snNum = -1;
			}
		}
	}
}
