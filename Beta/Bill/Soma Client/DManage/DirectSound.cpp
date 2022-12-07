// DirectSound.cpp: implementation of the CDirectSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirectSound.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectSound::CDirectSound()
{
	m_pDSound = NULL;
	m_hWnd = NULL;
	m_pPrimaryBuffer = NULL;
	m_bFullScreen = TRUE;
	m_bSoundEnabled = FALSE;//dspring_0721현재 사운드를 사용 못하게 하고 초기화에 성공하면 사용할수 있게 해준다.....
	m_vMusicVolume = m_vSoundVolume = DSBVOLUME_MAX;
	m_pDSListener = NULL;
}

CDirectSound::~CDirectSound()
{
	POSITION pos;
	int key;
	CDSoundBuffer *value;

	pos = m_mapMusicBuffer.GetStartPosition();
	while(pos)
	{
		m_mapMusicBuffer.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_mapMusicBuffer.RemoveAll();

	pos = m_mapSoundBuffer.GetStartPosition();
	while(pos)
	{
		m_mapSoundBuffer.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_mapSoundBuffer.RemoveAll();

	if( m_pDSListener ) 
	{
		m_pDSListener->Release();
		m_pDSListener = NULL;
	}
 
	if (m_pPrimaryBuffer)
	{
		m_pPrimaryBuffer->Stop();
		m_pPrimaryBuffer->Release();
		m_pPrimaryBuffer = NULL;
	}

	if (m_pDSound) 
	{
		m_pDSound->Release();
		m_pDSound = NULL;
	}
}

BOOL CDirectSound::InitDirectSound(HWND hwnd, BOOL bFullScreen, FLOAT fDopplerFactor, FLOAT fRolloffFactor  )
{
	HRESULT hr;
	m_hWnd = hwnd;
	m_bFullScreen = bFullScreen;
	if (DS_OK!=DirectSoundCreate(NULL, &m_pDSound, NULL)) return FALSE;
	if (DS_OK!=m_pDSound->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY)) return FALSE;

    DSBUFFERDESC    dsbd;
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;	
	if (DS_OK!=m_pDSound->CreateSoundBuffer(&dsbd, &m_pPrimaryBuffer, NULL))	//Primary Buffer생성 (Mix되는 용)
	{																			//Second의 합성으로 Primary에 적용됨
		return FALSE;
	}
    if( DS_OK != m_pPrimaryBuffer->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)&m_pDSListener  ) )
        return FALSE;
    // Get listener parameters
    m_dsListenerParams.dwSize = sizeof(DS3DLISTENER);
    m_pDSListener->GetAllParameters( &m_dsListenerParams );

    // Set primary buffer format to 22kHz and 16-bit output.
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = 2; 
    wfx.nSamplesPerSec  = 22050; 
    wfx.wBitsPerSample  = 16; 
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if( FAILED( hr = m_pPrimaryBuffer->SetFormat(&wfx) ) )
        return hr;

	SetListenerParameters( fDopplerFactor, fRolloffFactor );

	m_bSoundEnabled = TRUE;//dspring_0721 초기화가 성공해서 사운드를 사용 가능하게 해줌
	return TRUE;
}
VOID CDirectSound::SetListenerParameters( FLOAT fDopplerFactor, FLOAT fRolloffFactor )
{
    // Every change to 3-D sound buffer and listener settings causes 
    // DirectSound to remix, at the expense of CPU cycles. 
    // To minimize the performance impact of changing 3-D settings, 
    // use the DS3D_DEFERRED flag in the dwApply parameter of any of 
    // the IDirectSound3DListener or IDirectSound3DBuffer methods that 
    // change 3-D settings. Then call the IDirectSound3DListener::CommitDeferredSettings 
    // method to execute all of the deferred commands at once.
    DWORD dwApplyFlag = DS3D_IMMEDIATE;

    m_dsListenerParams.flDopplerFactor = fDopplerFactor;
    m_dsListenerParams.flRolloffFactor = fRolloffFactor;

    if( m_pDSListener )
        m_pDSListener->SetAllParameters( &m_dsListenerParams, dwApplyFlag );
}

void CDirectSound::SetScale( CPoint ptScale )
{
	CDSoundBuffer::SetScale( ptScale ); 
}
void CDirectSound::SetMusicVolume(long volume)
{
	if(volume < DSBVOLUME_MIN && volume > DSBVOLUME_MAX) return;

	m_vMusicVolume = volume;

	POSITION pos;
	int key;
	CDSoundBuffer *value;

	pos = m_mapMusicBuffer.GetStartPosition();
	while(pos)
	{
		m_mapMusicBuffer.GetNextAssoc(pos, key, value);
		value->SetVolume(m_vMusicVolume);
	}
}

int CDirectSound::GetMusicVolume()
{
	return m_vMusicVolume;
}

int CDirectSound::MusicPlay(CString file_name, int index, BOOL bLoop)
{
	if( !m_bSoundEnabled ) return -1;//사용 불가능한경우 여기서 제한을 둔다...//dspring_0721

	RemoveUnusedMusic();

	CDSoundBuffer *pSB;
	CString szFileName;

	if(m_mapMusicBuffer.Lookup(index, pSB))
	{
		pSB->Stop();
		return pSB->Play(bLoop);
	}

	pSB = new CDSoundBuffer(this);				//없으면 새로 생성함
	szFileName = ".\\sound\\";
	szFileName += file_name;
	if(pSB->Load(szFileName, m_vMusicVolume))	//Load함
	{
		m_mapMusicBuffer[index] = pSB;
		pSB->SetIndex(index);
		return pSB->Play(bLoop);						//재생
	}
	else
	{
		delete pSB;
		return -1;
	}
}

BOOL CDirectSound::MusicStop(int index, int sub_index, BOOL bDelete)
{
	CDSoundBuffer *pSB;

	if(m_mapMusicBuffer.Lookup(index, pSB))
	{
		pSB->Stop(sub_index);
		if(bDelete)
		{
			delete pSB;
			m_mapMusicBuffer.RemoveKey(index);
		}
		return TRUE;
	}
	else return FALSE;
}

BOOL CDirectSound::IsMusicPlaying(int index, int sub_index)
{
	CDSoundBuffer *pSB;

	if(m_mapMusicBuffer.Lookup(index, pSB))
	{
		return pSB->IsPlaying(sub_index);
	}
	else return FALSE;
}

//SoundPlay가 종료된 후 30초이상 된 데이터를 정리함(CDSoundBuffer 객체를 파괴)
void CDirectSound::RemoveUnusedMusic()
{
	POSITION pos;
	int key;
	CDSoundBuffer *value;
	DWORD dwCurTick;
	LONG gap_time;

	pos = m_mapMusicBuffer.GetStartPosition();
	dwCurTick = GetTickCount();
	while(pos)
	{
		m_mapMusicBuffer.GetNextAssoc(pos, key, value);

		gap_time = dwCurTick - value->m_dwStopTick;
		if(gap_time > 30000)
		{
			delete value;
			m_mapMusicBuffer.RemoveKey(key);
		}
	}
}

void CDirectSound::SetSoundVolume(long volume)
{
	if(volume < DSBVOLUME_MIN && volume > DSBVOLUME_MAX) return;

	m_vSoundVolume = volume;

	POSITION pos;
	int key;
	CDSoundBuffer *value;

	pos = m_mapSoundBuffer.GetStartPosition();
	while(pos)
	{
		m_mapSoundBuffer.GetNextAssoc(pos, key, value);
		value->SetVolume(m_vSoundVolume);
	}
}

int CDirectSound::GetSoundVolume()
{
	return m_vSoundVolume;
}

int CDirectSound::SoundPlay(CString file_name, int index, BOOL bLoop, CPoint ptSoundPos, FLOAT fMinDistance, FLOAT fMaxDistance)
{
	if( !m_bSoundEnabled ) return -1;//사용 불가능한경우 여기서 제한을 둔다...//dspring_0721

	RemoveUnusedSound();

	CDSoundBuffer *pSB;
	CString szFileName;

	if(m_mapSoundBuffer.Lookup(index, pSB))
	{
		pSB->SetObjectProperties( ptSoundPos ); 
		return pSB->Play(bLoop);
	}

	pSB = new CDSoundBuffer(this);				//없으면 새로 생성함
	szFileName = ".\\sound\\";
	szFileName += file_name;
	if (pSB->Load(szFileName, m_vSoundVolume))	//Load함
	{
		pSB->SetAllParameters( fMinDistance, fMaxDistance ); 
		m_mapSoundBuffer[index] = pSB;
		pSB->SetIndex(index);
		pSB->SetObjectProperties( ptSoundPos ); 
		return pSB->Play(bLoop);						//재생
	}
	else
	{
		delete pSB;
		return -1;
	}
}

BOOL CDirectSound::SoundStop(int index, int sub_index)
{
	CDSoundBuffer *pSB;

	if(m_mapSoundBuffer.Lookup(index, pSB))
	{
		pSB->Stop(sub_index);
		return TRUE;
	}
	else return FALSE;
}

BOOL CDirectSound::IsSoundPlaying(int index, int sub_index)
{
	CDSoundBuffer *pSB;

	if(m_mapSoundBuffer.Lookup(index, pSB))
	{
		return pSB->IsPlaying(sub_index);
	}
	else return FALSE;
}

//SoundPlay가 종료된 후 30초이상 된 데이터를 정리함(CDSoundBuffer 객체를 파괴)
void CDirectSound::RemoveUnusedSound()
{
	POSITION pos;
	int key;
	CDSoundBuffer *value;
	DWORD dwCurTick;
	LONG gap_time;

	dwCurTick = GetTickCount();
	pos = m_mapSoundBuffer.GetStartPosition();
	while(pos)
	{
		m_mapSoundBuffer.GetNextAssoc(pos, key, value);

		gap_time = dwCurTick - value->m_dwStopTick;
		if(gap_time > 30000)
		{
			delete value;
			m_mapSoundBuffer.RemoveKey(key);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction  (class CDSoundBuffer)
//////////////////////////////////////////////////////////////////////
CPoint CDSoundBuffer::m_nScale= CPoint( 400, 300);

CDSoundBuffer::CDSoundBuffer(CDirectSound *pDirectSound)
{
	m_bpDirectSound = pDirectSound;
    m_pWaveFormatEx = NULL;          // Pointer to waveformatex structure.
	m_pbData   = NULL;
	m_cbSize = 0;

	m_bSticky = FALSE;
	m_bHardware = FALSE;

	m_bLoadOk = FALSE;

	m_lpDsNotify = NULL;
	m_hNotifyEvent = NULL;
	m_iIndexInDS = -1;
	m_bStartDestroy = FALSE;
	m_pDS3DBuffer     = NULL; 

}

CDSoundBuffer::~CDSoundBuffer()
{
	m_bStartDestroy = TRUE;
	int count, i;

	Stop();
	count = m_arrDSB.GetSize();
	for(i = 0; i < count; i++)
	{
		m_arrDSB[i]->Release();
	}
	m_arrDSB.RemoveAll();

	if (m_lpDsNotify)
	{
		m_lpDsNotify->Release();
		m_lpDsNotify = NULL;
	}

	if (m_hNotifyEvent) 
	{
		::CloseHandle(m_hNotifyEvent);
		m_hNotifyEvent = NULL;
	}

	if (m_pWaveFormatEx != NULL)
	{
		delete [] (PBYTE)m_pWaveFormatEx;
		m_pWaveFormatEx = NULL;			
	}
	if (m_pbData != NULL)
	{
		delete[] m_pbData;
		m_pbData = NULL;
	}
	if( m_pDS3DBuffer ) 
	{
		m_pDS3DBuffer->Release();
		m_pDS3DBuffer = NULL;
	}
}
VOID CDSoundBuffer::SetObjectProperties( CPoint ptSoundPos )
{
    D3DVECTOR vPosition;

    vPosition.x = float(3.5f * (ptSoundPos.x - m_nScale.x )/m_nScale.x * 1.0);//(FLOAT)sin(t);
    vPosition.z = 0.0f;
    vPosition.y = float(3.5f * (ptSoundPos.y - m_nScale.y)/m_nScale.y * 1.0);//(FLOAT)cos(t);

    D3DVECTOR vVelocity;
//    vVelocity.x = ORBIT_MAX_RADIUS * fXScale * (FLOAT)sin(t+0.05f);
//    vVelocity.y = 0.0f;
//    vVelocity.z = ORBIT_MAX_RADIUS * fYScale * (FLOAT)cos(t+0.05f);

    vVelocity.x = 0.0f;
    vVelocity.y = 0.0f;
    vVelocity.z = 0.0f;

    // Every change to 3-D sound buffer and listener settings causes 
    // DirectSound to remix, at the expense of CPU cycles. 
    // To minimize the performance impact of changing 3-D settings, 
    // use the DS3D_DEFERRED flag in the dwApply parameter of any of 
    // the IDirectSound3DListener or IDirectSound3DBuffer methods that 
    // change 3-D settings. Then call the IDirectSound3DListener::CommitDeferredSettings 
    // method to execute all of the deferred commands at once.
    DWORD dwApplyFlag = ( 0 ) ? DS3D_DEFERRED : DS3D_IMMEDIATE;

    memcpy( &m_dsBufferParams.vPosition, &vPosition, sizeof(D3DVECTOR) );
    memcpy( &m_dsBufferParams.vVelocity, &vVelocity, sizeof(D3DVECTOR) );

    if( m_pDS3DBuffer )
        m_pDS3DBuffer->SetAllParameters( &m_dsBufferParams, dwApplyFlag );
}

VOID CDSoundBuffer::SetAllParameters( FLOAT fMinDistance,   FLOAT fMaxDistance )
{
	DWORD dwApplyFlag = DS3D_IMMEDIATE;
    m_dsBufferParams.flMinDistance = fMinDistance;
    m_dsBufferParams.flMaxDistance = fMaxDistance;

    if( m_pDS3DBuffer )
        m_pDS3DBuffer->SetAllParameters( &m_dsBufferParams, dwApplyFlag );

}

int CDSoundBuffer::NewDirectSoundBuffer(long volume)
{
	DSBUFFERDESC        dsbd;
	DSBCAPS         dsbc;
	HRESULT         hr;
//	BYTE            *pbData         = NULL;
//	BYTE            *pbData2        = NULL;
	VOID            *pbData         = NULL;
	VOID            *pbData2        = NULL;

	DWORD           dwLength;
	DWORD           dwLength2;
	BOOL			bOk = TRUE;
	LPDIRECTSOUNDBUFFER pDSB;
	int				index;
	
	// Set up the direct sound buffer. 
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize                 = sizeof(DSBUFFERDESC);
	dsbd.dwFlags                = DSBCAPS_CTRL3D | DSBCAPS_STATIC;// | DSBCAPS_CTRLPOSITIONNOTIFY; Notify사용안함(Event반응 하려면 Multithread로 해야하는 디 동기가 잘 안맞음)
	// Use new GetCurrentPosition() accuracy (DirectX 2 feature)
//	dsbd.dwFlags                |= DSBCAPS_CTRLDEFAULT |DSBCAPS_GETCURRENTPOSITION2;
	// 내컴에서 에러 메닉스
//	dsbd.dwFlags                |= /*DSBCAPS_CTRLDEFAULT |임시 에러가 나서 임시로 막았음*/ DSBCAPS_GETCURRENTPOSITION2;
	dsbd.dwFlags |= DSBCAPS_STATIC | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	if (m_bSticky) dsbd.dwFlags |= DSBCAPS_STICKYFOCUS;
	dsbd.dwBufferBytes			= m_cbSize;
	dsbd.lpwfxFormat            = m_pWaveFormatEx;

	int nResult = 0;
    switch( nResult )
    {
    case -1: // User canceled dialog box
		TRACE0("Load aborted.\n");
       return S_FALSE;

    case 0: // User selected DS3DALG_NO_VIRTUALIZATION  
        dsbd.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
        break;

    case 1: // User selected DS3DALG_HRTF_FULL  
        dsbd.guid3DAlgorithm = DS3DALG_HRTF_FULL;
        break;

    case 2: // User selected DS3DALG_HRTF_LIGHT
        dsbd.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;
        break;
    }
	if ((hr = m_bpDirectSound->GetInterface()->CreateSoundBuffer(&dsbd, &pDSB, NULL)) != DS_OK) bOk = FALSE;

    // Get the 3D buffer from the secondary buffer
    if( FAILED( hr = pDSB->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)&m_pDS3DBuffer ) ) )
        return hr;

    m_dwBufferBytes = dsbd.dwBufferBytes;

    // Get the 3D buffer parameters
    m_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
    m_pDS3DBuffer->GetAllParameters( &m_dsBufferParams );

    // Set new 3D buffer parameters
    m_dsBufferParams.dwMode = DS3DMODE_HEADRELATIVE;
    m_pDS3DBuffer->SetAllParameters( &m_dsBufferParams, DS3D_IMMEDIATE );

	// Ok, lock the sucker down, and copy the memory to it.
	if (bOk && (hr = pDSB->Lock(0, 
							m_cbSize, 
							&pbData,
							&dwLength,
							&pbData2,
							&dwLength2,
							0L)) != DS_OK)//에러가 나서 임시로 막음..
//	if (bOk && (hr = m_pDSB->Lock(0, 
//							m_cbSize, 
//							(LPVOID)&pbData,
//							&dwLength,
//							(LPVOID)&pbData2,
//							&dwLength2,
//							0L)) != DS_OK)
	{
		bOk = FALSE;
	}
	
	if (bOk) 
	{
		ASSERT(pbData != NULL);
		CopyMemory(pbData, m_pbData, m_cbSize);
	}
	
	// Ok, now unlock the buffer, we don't need it anymore.
	if (bOk && (hr = pDSB->Unlock(pbData, m_cbSize, NULL, 0)) != DS_OK) bOk = FALSE;
	
	pbData = NULL;
	
	if (bOk && (hr = pDSB->SetVolume(volume)) != DS_OK) bOk = FALSE;	//볼륨최대
	if (bOk && (hr = pDSB->SetPan(DSBPAN_CENTER)) != DS_OK) bOk = FALSE;		//스피커 균형:중앙
	
	dsbc.dwSize = sizeof(dsbc);
	if (bOk && (hr = pDSB->GetCaps(&dsbc)) != DS_OK) bOk = FALSE;
	
	if (dsbc.dwFlags & DSBCAPS_LOCHARDWARE) {	//하드웨어 사용인지 검사
		m_bHardware = TRUE;						
	} else {
		m_bHardware = FALSE;
	}

	if (FALSE==bOk)
	{
		if (pbData != NULL)
		{
			hr = pDSB->Unlock(m_pbData, m_cbSize, NULL, 0);
			pbData = NULL;
		}
		
		if (pDSB != NULL)
		{
			pDSB->Release();
			pDSB = NULL;
		}
		index = -1;
	}
	else
	{
		index = m_arrDSB.Add(pDSB);
	}
	
	return index; 
}

BOOL CDSoundBuffer::Load(CString szFileName, long volume)
{
	BOOL bOk = TRUE;
	m_szFileName = szFileName;

	DWORD           cSamples;
	int				index = -1;

	if (WaveLoadFile((PCHAR)(LPCTSTR)szFileName, &m_cbSize, 
		&cSamples, &m_pWaveFormatEx, &m_pbData) != 0)
	{
		TRACE("Bad wave file or file too big to fit in memory\n");
		TRACE("Cannot load wave\n");
		bOk = FALSE;
	}
	if (bOk && ((index = NewDirectSoundBuffer(volume)) < 0))
	{
		TRACE("Cannot create new buffer : %s\n", (LPCTSTR)szFileName);
		TRACE("Direct Sound Error\n");
		bOk = FALSE;
	}
	if (bOk) //성공
	{
		m_bLoadOk = TRUE;
		//MiliSecond 단위로 Play시간을 기록
		m_dwDurationTime = m_cbSize*1000/m_pWaveFormatEx->nSamplesPerSec;
		return TRUE;
	}
	
	if (index >= 0)
	{
		m_arrDSB[index]->Release();
		m_arrDSB.RemoveAt(index);
	}
	if (m_pWaveFormatEx != NULL)
	{
		delete [] (PBYTE)m_pWaveFormatEx;
		m_pWaveFormatEx = NULL;			
	}
	if (m_pbData != NULL)
	{
		delete[] m_pbData;
		m_pbData = NULL;
	}
	return FALSE;
}

void CDSoundBuffer::SetVolume(long volume)
{
	if (FALSE==m_bLoadOk) return;

	int i, count;

	count = m_arrDSB.GetSize();
	for(i = 0; i < count; i++)
	{
		m_arrDSB[i]->SetVolume(volume);
	}
	TRACE1("Vol = %d\n", volume);
}

long CDSoundBuffer::GetVolume()
{
	if (FALSE==m_bLoadOk) return FALSE;

	if(m_arrDSB.GetSize() > 0)
	{
		long volume;
		if(m_arrDSB[0]->GetVolume(&volume) == DS_OK) return volume;
		else return 1L;
	}
	else return 1L;
}

int CDSoundBuffer::Play(BOOL bLoop)
{
	if (FALSE==m_bLoadOk) return -1;

	int i, count;
	LPDIRECTSOUNDBUFFER pDSB;

	count = m_arrDSB.GetSize();
	if(count > 0)
	{
		for(i = 0; i < count; i++)
		{
			DWORD status;
			if(m_arrDSB[i]->GetStatus(&status) == DS_OK)
			{
				if(status & DSBSTATUS_PLAYING) continue;
				else
				{
					m_arrDSB[i]->Play(0, 0, bLoop);
					m_dwStopTick = GetTickCount()+m_dwDurationTime;
					return i;
				}
			}
		}
		if(m_bpDirectSound->GetInterface()->DuplicateSoundBuffer(m_arrDSB[0], &pDSB) == DS_OK)
		{
			i = m_arrDSB.Add(pDSB);
			pDSB->Play(0, 0, bLoop);
			m_dwStopTick = GetTickCount()+m_dwDurationTime;
			return i;
		}
		else return -1;
	}
	else return -1;
}

BOOL CDSoundBuffer::Stop(int sub_index)
{
	if (FALSE==m_bLoadOk) return FALSE;

	int i, count;

	count = m_arrDSB.GetSize();
	if(sub_index == -1)
	{
		for(i = 0; i < count; i++)
		{
			m_arrDSB[i]->Stop();
		}
		m_dwStopTick = GetTickCount();
	}
	else if(sub_index < count)
	{
		m_arrDSB[sub_index]->Stop();
		if(IsPlaying() == FALSE)
		{
			m_dwStopTick = GetTickCount();
		}
	}
	return TRUE;
}

BOOL CDSoundBuffer::IsPlaying(int sub_index)
{
	if(m_bLoadOk == FALSE) return FALSE;

	int i, count;
	DWORD status;

	count = m_arrDSB.GetSize();
	if(sub_index == -1)
	{
		for(i = 0; i < count; i++)
		{
			if(m_arrDSB[i]->GetStatus(&status) == DS_OK)
			{
				if(status & DSBSTATUS_PLAYING) return TRUE;
			}
		}
	}
	else if(sub_index < count)
	{
		if(m_arrDSB[sub_index]->GetStatus(&status) == DS_OK)
		{
			if(status & DSBSTATUS_PLAYING) return TRUE;
		}
	}
	return FALSE;
}

void CDSoundBuffer::SoundPlayEndProcess()
{
	if (m_bStartDestroy) return;

	m_dwStopTick = GetTickCount();
}
//사용안하기로 함(Thread동기 문제땜시)
UINT CDSoundBuffer::SoundStopProc(LPVOID lParam)
{
	CDSoundBuffer *pSB = (CDSoundBuffer*) lParam;
	WaitForSingleObject(pSB->GetNotifyHandle(), INFINITE);
	pSB->SoundPlayEndProcess();
	return 0;
}



////////////////////////////////////////////////
// Wave 처리 루틴들
////////////////////////////////////////////////
/* ROUTINES */
/* -------------------------------------------------------*/

/* This function will open a wave input file and prepare it for reading,
* so the data can be easily
* read with WaveReadFile. Returns 0 if successful, the error code if not.
*      pszFileName - Input filename to load.
*      phmmioIn    - Pointer to handle which will be used
*          for further mmio routines.
*      ppwfxInfo   - Ptr to ptr to WaveFormatEx structure
*          with all info about the file.                        
*      
*/
int CDSoundBuffer::WaveOpenFile(
				 char *pszFileName,                              // (IN)
				 HMMIO *phmmioIn,                                // (OUT)
				 WAVEFORMATEX **ppwfxInfo,                       // (OUT)
				 MMCKINFO *pckInRIFF                             // (OUT)
				 )
{
	HMMIO           hmmioIn;
	MMCKINFO        ckIn;           // chunk info. for general use.
	PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.       
	WORD            cbExtraAlloc;   // Extra bytes for waveformatex 
	int             nError;         // Return value.
	
	
	// Initialization...
	*ppwfxInfo = NULL;
	nError = 0;
	hmmioIn = NULL;
	
	if ((hmmioIn = mmioOpen(pszFileName, NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
	{
		nError = ER_CANNOTOPEN;
		goto ERROR_READING_WAVE;
	}
	
	if ((nError = (int)mmioDescend(hmmioIn, pckInRIFF, NULL, 0)) != 0)
	{
		goto ERROR_READING_WAVE;
	}
	
	
	if ((pckInRIFF->ckid != FOURCC_RIFF) || (pckInRIFF->fccType != mmioFOURCC('W', 'A', 'V', 'E')))
	{
		nError = ER_NOTWAVEFILE;
		goto ERROR_READING_WAVE;
	}
	
	/* Search the input file for for the 'fmt ' chunk.     */
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if ((nError = (int)mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
	{
		goto ERROR_READING_WAVE;                
	}
	
	/* Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    * if there are extra parameters at the end, we'll ignore them */
    
    if (ckIn.cksize < (long) sizeof(PCMWAVEFORMAT))
	{
		nError = ER_NOTWAVEFILE;
		goto ERROR_READING_WAVE;
	}
	
	/* Read the 'fmt ' chunk into <pcmWaveFormat>.*/     
    if (mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, (long) sizeof(pcmWaveFormat)) != (long) sizeof(pcmWaveFormat))
	{
		nError = ER_CANNOTREAD;
		goto ERROR_READING_WAVE;
	}
	
	
	// Ok, allocate the waveformatex, but if its not pcm
	// format, read the next word, and thats how many extra
	// bytes to allocate.
	if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
		cbExtraAlloc = 0;                               
	
	else
	{
		// Read in length of extra bytes.
		if (mmioRead(hmmioIn, (LPSTR) &cbExtraAlloc,
			(long) sizeof(cbExtraAlloc)) != (long) sizeof(cbExtraAlloc))
		{
			nError = ER_CANNOTREAD;
			goto ERROR_READING_WAVE;
		}
		
	}
	
	// Ok, now allocate that waveformatex structure.
	if ((*ppwfxInfo = (PWAVEFORMATEX)new BYTE[sizeof(WAVEFORMATEX)+cbExtraAlloc]) == NULL)
	{
		nError = ER_MEM;
		goto ERROR_READING_WAVE;
	}
	
	// Copy the bytes from the pcm structure to the waveformatex structure
	memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
	(*ppwfxInfo)->cbSize = cbExtraAlloc;
	
	// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
	if (cbExtraAlloc != 0)
	{
		if (mmioRead(hmmioIn, (LPSTR) (((BYTE*)&((*ppwfxInfo)->cbSize))+sizeof(cbExtraAlloc)),
			(long) (cbExtraAlloc)) != (long) (cbExtraAlloc))
		{
			nError = ER_NOTWAVEFILE;
			goto ERROR_READING_WAVE;
		}
	}
	
	/* Ascend the input file out of the 'fmt ' chunk. */                                                            
	if ((nError = mmioAscend(hmmioIn, &ckIn, 0)) != 0)
	{
		goto ERROR_READING_WAVE;
		
	}
	
	
	goto TEMPCLEANUP;               
	
ERROR_READING_WAVE:
	if (*ppwfxInfo != NULL)
	{
		delete[] *ppwfxInfo;
		*ppwfxInfo = NULL;
	}               
	
	if (hmmioIn != NULL)
	{
		mmioClose(hmmioIn, 0);
		hmmioIn = NULL;
	}
	
TEMPCLEANUP:
	*phmmioIn = hmmioIn;
	
	return(nError);
	
}

/*      This routine has to be called before WaveReadFile as it searchs for the chunk to descend into for
reading, that is, the 'data' chunk.  For simplicity, this used to be in the open routine, but was
taken out and moved to a separate routine so there was more control on the chunks that are before
the data chunk, such as 'fact', etc... */

int CDSoundBuffer::WaveStartDataRead(
					  HMMIO *phmmioIn,
					  MMCKINFO *pckIn,
					  MMCKINFO *pckInRIFF
					  )
{
	int                     nError;
	
	nError = 0;
	
	// Do a nice little seek...
	if ((nError = mmioSeek(*phmmioIn, pckInRIFF->dwDataOffset + sizeof(FOURCC), SEEK_SET)) == -1)
	{
		ASSERT(FALSE);
	}
	
	nError = 0;
	//      Search the input file for for the 'data' chunk.
	pckIn->ckid = mmioFOURCC('d', 'a', 't', 'a');
	if ((nError = mmioDescend(*phmmioIn, pckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
	{
		goto ERROR_READING_WAVE;
	}
	
	goto CLEANUP;
	
ERROR_READING_WAVE:
	
CLEANUP:        
	return(nError);
}


/*      This will read wave data from the wave file.  Makre sure we're descended into
the data chunk, else this will fail bigtime!
hmmioIn         - Handle to mmio.
cbRead          - # of bytes to read.   
pbDest          - Destination buffer to put bytes.
cbActualRead- # of bytes actually read.

		
		  
*/


int CDSoundBuffer::WaveReadFile(
				 HMMIO hmmioIn,                          // IN
				 UINT cbRead,                            // IN           
				 BYTE *pbDest,                           // IN
				 MMCKINFO *pckIn,                        // IN.
				 UINT *cbActualRead                      // OUT.
				 
				 )
{
	
	MMIOINFO    mmioinfoIn;         // current status of <hmmioIn>
	int                     nError;
	UINT            cT, cbDataIn;
	
	nError = 0;
	
	if (nError = mmioGetInfo(hmmioIn, &mmioinfoIn, 0) != 0)
	{
		goto ERROR_CANNOT_READ;
	}
				
	cbDataIn = cbRead;
	if (cbDataIn > pckIn->cksize) 
		cbDataIn = pckIn->cksize;       
	
	pckIn->cksize -= cbDataIn;
	
	for (cT = 0; cT < cbDataIn; cT++)
	{
		/* Copy the bytes from the io to the buffer. */
		if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
		{
			if ((nError = mmioAdvance(hmmioIn, &mmioinfoIn, MMIO_READ)) != 0)
			{
				goto ERROR_CANNOT_READ;
			} 
			if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
			{
				nError = ER_CORRUPTWAVEFILE;
				goto ERROR_CANNOT_READ;
			}
		}
		
		// Actual copy.
		*((BYTE*)pbDest+cT) = *((BYTE*)mmioinfoIn.pchNext);
		mmioinfoIn.pchNext++;
	}
	
	if ((nError = mmioSetInfo(hmmioIn, &mmioinfoIn, 0)) != 0)
	{
		goto ERROR_CANNOT_READ;
	}
	
	*cbActualRead = cbDataIn;
	goto FINISHED_READING;
	
ERROR_CANNOT_READ:
	*cbActualRead = 0;
	
FINISHED_READING:
	return(nError);
	
}

/*      This will close the wave file openned with WaveOpenFile.  
phmmioIn - Pointer to the handle to input MMIO.
ppwfxSrc - Pointer to pointer to WaveFormatEx structure.

  Returns 0 if successful, non-zero if there was a warning.
  
*/
int CDSoundBuffer::WaveCloseReadFile(
					  HMMIO *phmmio,                                  // IN
					  WAVEFORMATEX **ppwfxSrc                 // IN
					  )
{
	
	if (*ppwfxSrc != NULL)
	{
		delete[] *ppwfxSrc;
		*ppwfxSrc = NULL;
	}
	
	if (*phmmio != NULL)
	{
		mmioClose(*phmmio, 0);
		*phmmio = NULL;
	}
	
	return(0);
	
}

/*      This routine will create a wave file for writing.  This will automatically overwrite any
existing file with the same name, so be careful and check before hand!!!
pszFileName     - Pointer to filename to write.
phmmioOut               - Pointer to HMMIO handle that is used for further writes
pwfxDest                - Valid waveformatex destination structure.
pckOut                  - Pointer to be set with the MMCKINFO.
pckOutRIFF              - Pointer to be set with the RIFF info.

*/
int CDSoundBuffer::WaveCreateFile(
				   char *pszFileName,                              // (IN)
				   HMMIO *phmmioOut,                               // (OUT)
				   WAVEFORMATEX *pwfxDest,                 // (IN)
				   MMCKINFO *pckOut,                               // (OUT)
				   MMCKINFO *pckOutRIFF                    // (OUT)
				   
				   )
{
	
	int             nError;                         // Return value.
	DWORD           dwFactChunk;            // Contains the actual fact chunk. Garbage until WaveCloseWriteFile.
	MMCKINFO        ckOut1;
	
	dwFactChunk = (DWORD)-1;
	nError = 0;
	
	*phmmioOut = mmioOpen(pszFileName, NULL,
		MMIO_ALLOCBUF | MMIO_READWRITE|MMIO_CREATE);
	
    if (*phmmioOut == NULL)
	{
		nError = ER_CANNOTWRITE;
		goto ERROR_CANNOT_WRITE;    // cannot save WAVE file
	}
	
    /* Create the output file RIFF chunk of form type 'WAVE'.
	*/
    pckOutRIFF->fccType = mmioFOURCC('W', 'A', 'V', 'E');       
	pckOutRIFF->cksize = 0; 
    if ((nError = mmioCreateChunk(*phmmioOut, pckOutRIFF, MMIO_CREATERIFF)) != 0)
	{
		goto ERROR_CANNOT_WRITE;    // cannot write file, probably
	}
	
    /* We are now descended into the 'RIFF' chunk we just created.
	* Now create the 'fmt ' chunk. Since we know the size of this chunk,
	* specify it in the MMCKINFO structure so MMIO doesn't have to seek
	* back and set the chunk size after ascending from the chunk.
	*/
    pckOut->ckid = mmioFOURCC('f', 'm', 't', ' ');
    pckOut->cksize = sizeof(PCMWAVEFORMAT);   // we know the size of this ck.
    if ((nError = mmioCreateChunk(*phmmioOut, pckOut, 0)) != 0)
	{
		goto ERROR_CANNOT_WRITE;    // cannot write file, probably
	}
	
    /* Write the PCMWAVEFORMAT structure to the 'fmt ' chunk if its that type. */
    if (pwfxDest->wFormatTag == WAVE_FORMAT_PCM)
	{
		if (mmioWrite(*phmmioOut, (HPSTR) pwfxDest, sizeof(PCMWAVEFORMAT))
			!= sizeof(PCMWAVEFORMAT))
		{
			nError = ER_CANNOTWRITE;
			goto ERROR_CANNOT_WRITE;    // cannot write file, probably
		}
	}
	
	else 
	{
		// Write the variable length size.
		if ((UINT)mmioWrite(*phmmioOut, (HPSTR) pwfxDest, sizeof(*pwfxDest)+pwfxDest->cbSize)
			!= (sizeof(*pwfxDest)+pwfxDest->cbSize))
		{
			nError = ER_CANNOTWRITE;
			goto ERROR_CANNOT_WRITE;    // cannot write file, probably
		} 
		
	}  
	
    /* Ascend out of the 'fmt ' chunk, back into the 'RIFF' chunk.
	*/
    if ((nError = mmioAscend(*phmmioOut, pckOut, 0)) != 0)
	{
		goto ERROR_CANNOT_WRITE;    // cannot write file, probably
	}
	
	// Now create the fact chunk, not required for PCM but nice to have.  This is filled
	// in when the close routine is called.
	ckOut1.ckid = mmioFOURCC('f', 'a', 'c', 't');
	ckOut1.cksize = 0;
    if ((nError = mmioCreateChunk(*phmmioOut, &ckOut1, 0)) != 0)
	{
		goto ERROR_CANNOT_WRITE;    // cannot write file, probably
	}
	
	if (mmioWrite(*phmmioOut, (HPSTR)&dwFactChunk, sizeof(dwFactChunk)) != sizeof(dwFactChunk))
	{
		nError = ER_CANNOTWRITE;
		goto ERROR_CANNOT_WRITE;
	}
	
	// Now ascend out of the fact chunk...
    if ((nError = mmioAscend(*phmmioOut, &ckOut1, 0)) != 0)
	{
		nError = ER_CANNOTWRITE;        // cannot write file, probably
		goto ERROR_CANNOT_WRITE;
	}
	
	
	
	goto DONE_CREATE;
	
ERROR_CANNOT_WRITE:
	// Maybe delete the half-written file?  Ah forget it for now, its good to leave the
	// file there for debugging...
	
DONE_CREATE:
	return(nError);
	
}

/*      This routine has to be called before any data is written to the wave output file, via wavewritefile.  This
sets up the data to write, and creates the data chunk.
*/

int CDSoundBuffer::WaveStartDataWrite(
					   HMMIO *phmmioOut,                               // (IN)
					   MMCKINFO *pckOut,                               // (IN)
					   MMIOINFO *pmmioinfoOut                  // (OUT)
					   )
{
	
	int             nError;
	
	nError = 0;
	/* Create the 'data' chunk that holds the waveform samples.  */
    pckOut->ckid = mmioFOURCC('d', 'a', 't', 'a');
	pckOut->cksize = 0;
    if ((nError = mmioCreateChunk(*phmmioOut, pckOut, 0)) != 0)
	{
		goto ERROR_CANNOT_WRITE;    // cannot write file, probably
	}
	
	if ((nError = mmioGetInfo(*phmmioOut, pmmioinfoOut, 0)) != 0)
	{
		goto ERROR_CANNOT_WRITE;
	}
	
	goto CLEANUP;
ERROR_CANNOT_WRITE:     
	
CLEANUP:
	return(nError);
}

/* This routine will write out data to a wave file. 
hmmioOut                - Handle to hmmioOut filled by WaveCreateFile
cbWrite                 - # of bytes to write out.
pbSrc                   - Pointer to source.
pckOut                  - pointer to ckOut filled by WaveCreateFile
cbActualWrite   - # of actual bytes written.
pmmioinfoOut    - Pointer to mmioinfoOut filled by WaveCreateFile.

  Returns 0 if successful, else the error code.
  
*/


int CDSoundBuffer::WaveWriteFile(
				  HMMIO hmmioOut,                         // (IN)
				  UINT cbWrite,                           // (IN)
				  BYTE *pbSrc,                            // (IN)
				  MMCKINFO *pckOut,                       // (IN)
				  UINT *cbActualWrite,            // (OUT)
				  MMIOINFO *pmmioinfoOut          // (IN)
				  )
{
	
	
	int                     nError;
	UINT            cT;
	
	nError = 0;
	
	*cbActualWrite = 0;
	
	for (cT=0; cT < cbWrite; cT++)
	{       
		if (pmmioinfoOut->pchNext == pmmioinfoOut->pchEndWrite)
		{
			pmmioinfoOut->dwFlags |= MMIO_DIRTY;
			if ((nError = mmioAdvance(hmmioOut, pmmioinfoOut, MMIO_WRITE)) != 0)
			{
				goto ERROR_CANNOT_WRITE;
			}
		}
		*((BYTE*)pmmioinfoOut->pchNext) = *((BYTE*)pbSrc+cT);
		pmmioinfoOut->pchNext++;
		(*cbActualWrite)++;
	}
	
	
ERROR_CANNOT_WRITE:
	// What to do here?  Well, for now, nothing, just return that error.  (maybe delete the
	// file later?
	
	return(nError);
	
}



/*      This routine will close a wave file used for writing.  Returns 0 if successful, else
the error code.
phmmioOut       - Pointer to mmio handle for saving.
pckOut          - Pointer to the MMCKINFO for saving.
pckOutRiff      - Pointer to the riff MMCKINFO for saving.
pmmioinfoOut- Pointer to mmioinfo for saving.
cSamples        - # of samples saved, for the fact chunk.  For PCM, this isn't used but
will be written anyway, so this can be zero as long as programs ignore
this field when they load PCM formats.

  
	
*/
int CDSoundBuffer::WaveCloseWriteFile(
					   HMMIO *phmmioOut,               // (IN)
					   MMCKINFO *pckOut,               // (IN)
					   MMCKINFO *pckOutRIFF,   // (IN)
					   MMIOINFO *pmmioinfoOut, // (IN)
					   DWORD cSamples                  // (IN)
					   )
{
	
	int                     nError;                         
	
	nError = 0;
	
	if (*phmmioOut == NULL)
		return(0);
	
	pmmioinfoOut->dwFlags |= MMIO_DIRTY;
    if ((nError = mmioSetInfo(*phmmioOut, pmmioinfoOut, 0)) != 0)
	{
		// cannot flush, probably...
		goto ERROR_CANNOT_WRITE;                                
	}
	
	/* Ascend the output file out of the 'data' chunk -- this will cause
	* the chunk size of the 'data' chunk to be written.
	*/
    if ((nError = mmioAscend(*phmmioOut, pckOut, 0)) != 0)
		goto ERROR_CANNOT_WRITE;    // cannot write file, probably
	
	
	// Do this here instead...
	if ((nError = mmioAscend(*phmmioOut, pckOutRIFF, 0)) != 0)
		goto ERROR_CANNOT_WRITE;    // cannot write file, probably
	
	
	nError = mmioSeek(*phmmioOut, 0, SEEK_SET);
	if ((nError = (int)mmioDescend(*phmmioOut, pckOutRIFF, NULL, 0)) != 0)
	{
		goto ERROR_CANNOT_WRITE;
	}
	
	nError = 0;
	pckOut->ckid = mmioFOURCC('f', 'a', 'c', 't');
	if ((nError = mmioDescend(*phmmioOut, pckOut, pckOutRIFF, MMIO_FINDCHUNK)) == 0)
	{
		// If it didn't fail, write the fact chunk out, if it failed, not critical, just
		// assert (below).
		nError = mmioWrite(*phmmioOut, (HPSTR)&cSamples, sizeof(DWORD));
		nError = mmioAscend(*phmmioOut, pckOut, 0); 
		nError = 0;
	}
	else 
	{
		nError = 0;
		ASSERT(FALSE);
	}
	
	// CANTWRITEFACT:
    /* Ascend the output file out of the 'RIFF' chunk -- this will cause
	* the chunk size of the 'RIFF' chunk to be written.
	*/
    if ((nError = mmioAscend(*phmmioOut, pckOutRIFF, 0)) != 0)
		goto ERROR_CANNOT_WRITE;    // cannot write file, probably
	
	
	
ERROR_CANNOT_WRITE:
	if (*phmmioOut != NULL)
	{
		mmioClose(*phmmioOut, 0);
		*phmmioOut = NULL;
	}
	
	return(nError);
	
}

/*      This routine will copy from a source wave file to a destination wave file all those useless chunks
(well, the ones useless to conversions, etc --> apparently people use them!).  The source will be
seeked to the begining, but the destination has to be at a current pointer to put the new chunks.
This will also seek     back to the start of the wave riff header at the end of the routine.

  phmmioIn                - Pointer to input mmio file handle.
  pckIn                   - Pointer to a nice ckIn to use.
  pckInRiff               - Pointer to the main riff.
  phmmioOut               - Pointer to output mmio file handle.
  pckOut                  - Pointer to nice ckOut to use.
  pckOutRiff              - Pointer to the main riff.
  
	
	  Returns 0 if successful, else the error code.  If this routine fails, it still attemps to seek back to
	  the start of the wave riff header, though this too could be unsuccessful.
*/

int CDSoundBuffer::WaveCopyUselessChunks(
						  HMMIO *phmmioIn, 
						  MMCKINFO *pckIn, 
						  MMCKINFO *pckInRiff, 
						  HMMIO *phmmioOut, 
						  MMCKINFO *pckOut, 
						  MMCKINFO *pckOutRiff)
{
	
	int                             nError;
	
	nError = 0;
	// First seek to the stinking start of the file, not including the riff header...
	if ((nError = mmioSeek(*phmmioIn, pckInRiff->dwDataOffset + sizeof(FOURCC), SEEK_SET)) == -1)
	{
		nError = ER_CANNOTREAD;
		goto ERROR_IN_PROC;
	}
	
	nError = 0;                     
	
    while (mmioDescend(*phmmioIn, pckIn, pckInRiff, 0) == 0)
    {
		
		//  quickly check for corrupt RIFF file--don't ascend past end!        
		if ((pckIn->dwDataOffset + pckIn->cksize) > (pckInRiff->dwDataOffset + pckInRiff->cksize))
			goto ERROR_IN_PROC;
		
		switch (pckIn->ckid)
		{                   
			//  explicitly skip these...            
		case mmioFOURCC('f', 'm', 't', ' '):
			break;
			
		case mmioFOURCC('d', 'a', 't', 'a'):
			break;
			
		case mmioFOURCC('f', 'a', 'c', 't'):
			break;
			
		case mmioFOURCC('J', 'U', 'N', 'K'):
			break;
			
		case mmioFOURCC('P', 'A', 'D', ' '):
			break;
			
		case mmioFOURCC('c', 'u', 'e', ' '):
			break;                                                  
			
			//  copy chunks that are OK to copy            
		case mmioFOURCC('p', 'l', 's', 't'):
			// although without the 'cue' chunk, it doesn't make much sense
			riffCopyChunk(*phmmioIn, *phmmioOut, pckIn);
			break;
			
		case mmioFOURCC('D', 'I', 'S', 'P'):
			riffCopyChunk(*phmmioIn, *phmmioOut, pckIn);
			break;
			
			
			//  don't copy unknown chunks
		default:
			break;
		}
		
		
		//  step up to prepare for next chunk..        
		mmioAscend(*phmmioIn, pckIn, 0);
    }
	
ERROR_IN_PROC:  
	{
		int nErrorT;
		// Seek back to riff header     
		nErrorT = mmioSeek(*phmmioIn, pckInRiff->dwDataOffset + sizeof(FOURCC), SEEK_SET);
	}
	
	return(nError);
}

/** BOOL RIFFAPI riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck)
*
*  DESCRIPTION:
*      
*
*  ARGUMENTS:
*      (LPWAVECONVCB lpwc, LPMMCKINFO lpck)
*
*  RETURN (BOOL NEAR PASCAL):
*
*
*  NOTES:
*
**  */

BOOL CDSoundBuffer::riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck)
{
    MMCKINFO    ck;
    PCHAR       hpBuf;
	
    //
    //
    //
    hpBuf = new char[lpck->cksize];
    if (!hpBuf)
		return (FALSE);
	
    ck.ckid   = lpck->ckid;
    ck.cksize = lpck->cksize;
    if (mmioCreateChunk(hmmioDst, &ck, 0))
		goto rscc_Error;
	
    if (mmioRead(hmmioSrc, hpBuf, lpck->cksize) != (LONG)lpck->cksize)
		goto rscc_Error;
	
    if (mmioWrite(hmmioDst, hpBuf, lpck->cksize) != (LONG)lpck->cksize)
		goto rscc_Error;
	
    if (mmioAscend(hmmioDst, &ck, 0))
		goto rscc_Error;
	
    if (hpBuf) delete[] hpBuf;
	
    return (TRUE);
	
rscc_Error:
	
    if (hpBuf) delete[] hpBuf;
	
    return (FALSE);
} /* RIFFSupCopyChunk() */



  /*      This routine loads a full wave file into memory.  Be careful, wave files can get
  pretty big these days :).  
  szFileName      -       sz Filename
  cbSize          -       Size of loaded wave (returned)
  cSamples        -       # of samples loaded.
  ppwfxInfo       -       Pointer to pointer to waveformatex structure.  The wfx structure
  IS ALLOCATED by this routine!  Make sure to free it!
  ppbData         -       Pointer to a byte pointer (globalalloc) which is allocated by this 
  routine.  Make sure to free it!
  
	Returns 0 if successful, else the error code.
*/

int CDSoundBuffer::WaveLoadFile(
				 char *pszFileName,                      // (IN)
				 UINT *cbSize,                           // (OUT)
				 DWORD *pcSamples,                       // (OUT)
				 WAVEFORMATEX **ppwfxInfo,       // (OUT)
				 BYTE **ppbData                          // (OUT)
				 )
{
	
	HMMIO                           hmmioIn;        
	MMCKINFO                        ckInRiff;
	MMCKINFO                        ckIn;
	int                                     nError;
	UINT                            cbActualRead;
	
	*ppbData = NULL;
	*ppwfxInfo = NULL;
	*cbSize = 0;
	
	if ((nError = WaveOpenFile(pszFileName, &hmmioIn, ppwfxInfo, &ckInRiff)) != 0)
	{
		goto ERROR_LOADING;
	}
	
	if ((nError = WaveStartDataRead(&hmmioIn, &ckIn, &ckInRiff)) != 0)
	{
		goto ERROR_LOADING;
	}
	
	// Ok, size of wave data is in ckIn, allocate that buffer.
	if ((*ppbData = new BYTE[ckIn.cksize]) == NULL)
	{
		nError = ER_MEM;
		goto ERROR_LOADING;
	}
	
	if ((nError = WaveReadFile(hmmioIn, ckIn.cksize, *ppbData, &ckIn, &cbActualRead)) != 0)
	{
		goto ERROR_LOADING;
	}        
	
	*cbSize = cbActualRead;
	goto DONE_LOADING;
	
ERROR_LOADING:
	if (*ppbData != NULL)
	{
		delete[] *ppbData;
		*ppbData = NULL;
	}
	if (*ppwfxInfo != NULL)
	{
		delete[] *ppwfxInfo;
		*ppwfxInfo = NULL;
	}
	
DONE_LOADING:
	// Close the wave file. 
	if (hmmioIn != NULL)
	{
		mmioClose(hmmioIn, 0);
		hmmioIn = NULL;
	}
	
	return(nError);
	
}

/*      This routine saves a wave file in currently in memory.
pszFileName -   FileName to save to.  Automatically overwritten, be careful!
cbSize          -       Size in bytes to write.
cSamples        -       # of samples to write, used to make the fact chunk. (if !PCM)
pwfxDest        -       Pointer to waveformatex structure.
pbData          -       Pointer to the data.
*/      

int CDSoundBuffer::WaveSaveFile(
				 char *pszFileName,                      // (IN)
				 UINT cbSize,                            // (IN)
				 DWORD cSamples,                         // (IN) 
				 WAVEFORMATEX *pwfxDest,         // (IN)
				 BYTE *pbData                            // (IN)
				 )
{
	
	HMMIO           hmmioOut;
	MMCKINFO        ckOut;
	MMCKINFO        ckOutRIFF;
	MMIOINFO        mmioinfoOut;
	UINT            cbActualWrite;
	int                     nError;
	
	if ((nError = WaveCreateFile(pszFileName, &hmmioOut, pwfxDest, &ckOut, &ckOutRIFF)) != 0)
	{
		goto ERROR_SAVING;
	}
	
	if ((nError = WaveStartDataWrite(&hmmioOut, &ckOut, &mmioinfoOut)) != 0)
	{
		goto ERROR_SAVING;
	}
	
	if ((nError = WaveWriteFile(hmmioOut, cbSize, pbData, &ckOut, &cbActualWrite, &mmioinfoOut)) != 0)
	{
		goto ERROR_SAVING;
	}
	
	if ((nError = WaveCloseWriteFile(&hmmioOut, &ckOut, &ckOutRIFF, &mmioinfoOut, cSamples)) != 0)
	{
		goto ERROR_SAVING;
	}       
	
ERROR_SAVING:
	
	return(nError);         
	
}

