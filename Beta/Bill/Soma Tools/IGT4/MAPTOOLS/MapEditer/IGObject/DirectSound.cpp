// DirectSound.cpp: implementation of the CDirectSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdAfx.h"
#include "DirectSound.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectSound::CDirectSound()
{
	m_hWnd = NULL; 			//application's window handle
	m_lpDS = NULL;
	m_lpPDSB = NULL; // 1 차 사운드 버퍼
	
	MAKE_ZERO(m_lpDSBs); // 2차 사운드 버퍼
	MAKE_ZERO(m_lpDSBDups); // 최고 복제 버퍼..

	MAKE_ZERO(m_szSoundFileName);	// 2차 사운드 버퍼에 할당된 사운드 파일 이름들
	
	m_lpDSBStream = NULL;		// 스트리밍 사운드 버퍼
	m_hmmio = NULL;			// 스트리밍 파일 핸들..
	m_dwMidBuffer = 0;		// 스트리밍 버퍼 사이즈의 정확히 중간.
	m_dwLastPlayPos = 0;	// 스트리밍 버퍼 사이즈의 마지막.
	MAKE_ZERO(m_mmckinfo);			// 스트리밍 청크 인포

	m_fVolStream = 1.0f;
	m_fVolStatic = 1.0f;
}

CDirectSound::~CDirectSound()
{
	UnloadAllWave();
	StopStream();
	RELEASE_SAFE(m_lpPDSB);
	RELEASE_SAFE(m_lpDS);
}

void CDirectSound::Init(HWND hwnd)
{
	m_hWnd = hwnd;
	DWORD ret;
	int x;
	
	HRESULT hr;
	for (x=0;x<MAX_SOUND_CHANNEL;x++)
	{
		m_lpDSBs[x] = NULL;
	}

	ret = DirectSoundCreate(NULL, &m_lpDS, NULL);

	if (ret != DS_OK)
	{
		MessageBox(NULL, "Can't create Direct Sound.", "Direct Sound", MB_OK);
		return;
	}

	ret = m_lpDS->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY);

	if (ret != DS_OK)
	{
		MessageBox(NULL, "Can't set Cooperative level.", "Direct Sound", MB_OK);
		return;
	}

	m_lpDS->SetSpeakerConfig(DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_WIDE); // 스피커를좌우 20 도 각도로 세팅

	// 1차 버퍼 생성
	DSBUFFERDESC dsbdesc;
	WAVEFORMATEX wfm;
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	dsbdesc.dwBufferBytes = 0;
	dsbdesc.lpwfxFormat = NULL;

	memset(&wfm, 0, sizeof(WAVEFORMATEX));
	wfm.wFormatTag = WAVE_FORMAT_PCM;
	wfm.nChannels = 2;
	wfm.nSamplesPerSec = 22050;
	wfm.wBitsPerSample = 16;
	wfm.nBlockAlign = wfm.wBitsPerSample / 8 * wfm.nChannels;
	wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nBlockAlign;

	hr = m_lpDS->CreateSoundBuffer(&dsbdesc, &m_lpPDSB, NULL);
	if(hr != DS_OK || m_lpPDSB == NULL)
	{
		MessageBox(NULL, "Can't Create Primary Sound Buffer.", "Direct Sound", MB_OK);
		return;
	}
	hr = m_lpPDSB->SetFormat(&wfm);
	hr = m_lpPDSB->Play(0, 0, DSBPLAY_LOOPING);
}

LPDIRECTSOUNDBUFFER CDirectSound::CreateStaticBuffer(int nCh, int nBit, DWORD dwRate, DWORD dwBytes)
{
	if(!m_lpDS) return NULL;

	DSBUFFERDESC dsbdesc;
	PCMWAVEFORMAT pcmwf;
	HRESULT hr;

	memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
	pcmwf.wf.wFormatTag 		= WAVE_FORMAT_PCM;
	pcmwf.wf.nChannels			= nCh;
	pcmwf.wf.nSamplesPerSec		= dwRate;
	pcmwf.wf.nBlockAlign		= nCh * nBit / 8;
	pcmwf.wf.nAvgBytesPerSec	= pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
	pcmwf.wBitsPerSample		= nBit;

	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize				= sizeof(DSBUFFERDESC);
//	dsbdesc.dwFlags 			= DSBCAPS_CTRLDEFAULT;
	dsbdesc.dwFlags 			= DSBCAPS_STATIC;
	dsbdesc.dwBufferBytes		= dwBytes;
	dsbdesc.lpwfxFormat 		= (LPWAVEFORMATEX)&pcmwf;

	LPDIRECTSOUNDBUFFER lpdsb;
	hr = m_lpDS->CreateSoundBuffer(&dsbdesc, &lpdsb, NULL);
	if (hr != DS_OK)
	{
		MessageBox(NULL, "Can't create Sound buffer.", "Direct Sound", MB_OK);
		return NULL;
	}

	return lpdsb;
}

void CDirectSound::Stop(int nSChn)
{
	if(!m_lpDS) return;
	if(nSChn < 0 || nSChn >= MAX_SOUND_CHANNEL) return;

	if(!m_lpDSBs[nSChn]) return;
	m_lpDSBs[nSChn]->Stop();
}

// 기존에 정해진 볼륨에서의 비율(0~100)을 넣는다..
void CDirectSound::Play(int nSChn, BOOL bLooping, float fVol)
{
	if(!m_lpDS) return;
	if(nSChn < 0 || nSChn >= MAX_SOUND_CHANNEL) return;
	if (!m_lpDSBs[nSChn]) return;
	 
	if(fVol < 0.0f) fVol = 0.0f;
	if(fVol > 1.0f) fVol = 1.0f;
	LONG vol = (LONG)(-1000.0f * sqrt(1.0f - fVol*fVol));
	m_lpDSBs[nSChn]->SetVolume(vol);

	// 플레이 중이면 중복 채널 생성후 플레이..
	DWORD dwStatus = 0;
	m_lpDSBs[nSChn]->GetStatus(&dwStatus);
	if(dwStatus & DSBSTATUS_PLAYING)
	{
		for(int i = 0; i < MAX_SOUND_DUPLICATE; i++)
		{
			m_lpDSBDups[nSChn][i]->GetStatus(&dwStatus);
			if(i == MAX_SOUND_DUPLICATE - 1) 
			{
				m_lpDSBDups[nSChn][i]->Stop();
				m_lpDSBDups[nSChn][i]->SetVolume(vol);
				m_lpDSBDups[nSChn][i]->Play(0, 0, bLooping == TRUE ? DSBPLAY_LOOPING : 0);
				continue;
			}
			if(dwStatus & DSBSTATUS_PLAYING) continue;

			m_lpDSBDups[nSChn][i]->SetVolume(vol);
			m_lpDSBDups[nSChn][i]->Play(0, 0, bLooping == TRUE ? DSBPLAY_LOOPING : 0);
			return;
		}
	}
	else
	{
		// 루핑 되지 않는 거면 첨으로 돌려서 플레이..
		HRESULT hr;
		if(!bLooping) hr = m_lpDSBs[nSChn]->SetCurrentPosition(0);
		hr = m_lpDSBs[nSChn]->Play(0, 0, bLooping == TRUE ? DSBPLAY_LOOPING : 0);
	}
}


void CDirectSound::UnloadWave(int nSChn)
{
	if(!m_lpDS) return;
	if(nSChn < 0 || nSChn >= MAX_SOUND_CHANNEL) return;

	if(!m_lpDSBs[nSChn]) return;

	for(int i = 0; i < MAX_SOUND_DUPLICATE; i++)
	{
		if(m_lpDSBDups[nSChn][i])
		{
			m_lpDSBDups[nSChn][i]->Stop();
			RELEASE_SAFE(m_lpDSBDups[nSChn][i]);
		}
	}

	lstrcpy(m_szSoundFileName[nSChn], "");
	m_lpDSBs[nSChn]->Stop();
	RELEASE_SAFE(m_lpDSBs[nSChn]);
}

BOOL CDirectSound::LoadWave(int nSChn, const char* szFileName)
{
	if(!m_lpDS) return FALSE;
	if(!lstrlen(szFileName)) return NULL;
	if(nSChn < 0 || nSChn >= MAX_SOUND_CHANNEL) return FALSE;
	HRESULT hr = 0;
	
	// 직전에 불렀던 파일 이름과 같으면 돌아간다.
	if(lstrcmp(szFileName, m_szSoundFileName[nSChn]) == NULL) return TRUE;
	UnloadWave(nSChn); // 전에 로딩된 웨이브 해제
	lstrcpy(m_szSoundFileName[nSChn], szFileName); // 파일 이름을 기억해 둔다.

	// 웨이브 파일 읽기가 실패하면 돌아간다.
	WAVEFORMATEX wfmtx;
	MMCKINFO mmckinfo, mmckinfoParent;
	HMMIO hmmio;

	if(WaveOpenFile(szFileName, &hmmio, &wfmtx, &mmckinfoParent) == FALSE)
	{
		MessageBox(m_hWnd, szFileName, "Wave 파일 읽기 실패", MB_OK);
		return FALSE;
	}
	if(WaveStartReadData(&hmmio, &mmckinfo) == FALSE)
	{
		MessageBox(m_hWnd, szFileName, "Wave 파일 읽기 실패", MB_OK);
		return FALSE;
	}
		
	DSBUFFERDESC dsbdesc;
	MAKE_ZERO(dsbdesc);
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	dsbdesc.dwBufferBytes = mmckinfo.cksize;
	dsbdesc.lpwfxFormat = &wfmtx;

	if(m_lpDS->CreateSoundBuffer(&dsbdesc, &m_lpDSBs[nSChn], NULL) != DS_OK)
	{
		WaveCloseReadFile(&hmmio);
		MessageBox(m_hWnd, szFileName, "DirectSound Buffer 채널 생성 실패", MB_OK);
		return FALSE;
	}
	
	void* lpBuff;
	DWORD dwBytesLocked;
	UINT dwBytesRead;

	hr = m_lpDSBs[nSChn]->Lock(0, 0, &lpBuff, &dwBytesLocked, 0, 0, DSBLOCK_ENTIREBUFFER); // 락 걸고..
	dwBytesRead = mmioRead(hmmio, (char*)lpBuff, dwBytesLocked);
	if(dwBytesRead != dwBytesLocked)
	{
		hr = m_lpDSBs[nSChn]->Unlock(lpBuff, dwBytesLocked, NULL, 0); // 언락.
		WaveCloseReadFile(&hmmio);
		MessageBox(m_hWnd, szFileName, "DirectSound WaveFile Data 읽기 실패", MB_OK);
		return FALSE;
	}
	hr = m_lpDSBs[nSChn]->Unlock(lpBuff, dwBytesLocked, NULL, 0); // 언락.

	WaveCloseReadFile(&hmmio); // 웨이브 파일 닫기..

	// 중복 사운드 채널 만들어둠..
	for(int i = 0; i < MAX_SOUND_DUPLICATE; i++)
	{
		m_lpDS->DuplicateSoundBuffer(m_lpDSBs[nSChn], &m_lpDSBDups[nSChn][i]);
	}

	return TRUE;
}

void CDirectSound::StopAllChannel()
{
	for(int i = 0; i < MAX_SOUND_CHANNEL; i++)
	{
		if(m_lpDSBs[i])
		{
			DWORD dwSt = NULL;
			m_lpDSBs[i]->GetStatus(&dwSt);
			if(dwSt == DSBSTATUS_PLAYING)
				m_lpDSBs[i]->Stop();
			RELEASE_SAFE(m_lpDSBs[i]);
		}
	}
}

void CDirectSound::UnloadAllWave()
{
	StopAllChannel();
	for(int i = 0; i < MAX_SOUND_CHANNEL; i++)
	{
		UnloadWave(i);
	}
}
// 스트리밍 채널 사용시 적어도 초당 5 번 이상씩 실행 해주어야 한다.
void CDirectSound::StreamProc()
{
	if(m_hmmio == NULL) return;
	if(m_lpDSBStream == NULL) return;
	
	DWORD dwPlay;

	if(m_lpDSBStream->GetCurrentPosition(&dwPlay, NULL) != DS_OK)
		return;
	
	// 현재 위치가 버퍼의 처음이나 중간 지점이라면 버퍼의 절반에 데이터를 채운다..
	if((dwPlay >= m_dwMidBuffer && m_dwLastPlayPos < m_dwMidBuffer) || dwPlay < m_dwLastPlayPos)
	{
		DWORD dwBytesLocked = 0;
		UINT dwBytesRead = 0;
		void* lpBuff = NULL;
		DWORD dwOffset = (dwPlay >= m_dwMidBuffer) ? 0 : m_dwMidBuffer;

		// 사운드 버퍼 락..
		HRESULT rval = m_lpDSBStream->Lock(dwOffset, m_dwMidBuffer, &lpBuff, &dwBytesLocked, NULL, NULL, 0);
		if(rval != DS_OK) MessageBox(m_hWnd, "....Sound", "Error", MB_OK);

		
		dwBytesRead = mmioRead(m_hmmio, (char*)lpBuff, dwBytesLocked);
		if(dwBytesRead < dwBytesLocked) // 파일의 끝에 가면...
		{
			if(WaveStartReadData(&m_hmmio, &m_mmckinfo) == FALSE)
			{
				MessageBox(m_hWnd, "Streaming Wave 파일을 읽지 못하였습니다.", "DirectSound Error", MB_OK);
				m_lpDSBStream->Unlock(lpBuff, dwBytesLocked, NULL, 0); // 사운드 버퍼 언락..
				StopStream();
				return;
			}

			char* lpTemp = (char*)lpBuff;
			lpTemp += dwBytesRead;
			mmioRead(m_hmmio, lpTemp, dwBytesLocked - dwBytesRead);
		}
		// 사운드 버퍼 언락..
		m_lpDSBStream->Unlock(lpBuff, dwBytesLocked, NULL, 0);
	}
	m_dwLastPlayPos = dwPlay; // 최종적으로 플레이한 위치 기록..
}

void CDirectSound::SetupStream(const char* szFileName)
{
	if(!m_lpDS) return;
	if(!lstrlen(szFileName)) return;

	WAVEFORMATEX wfmtx;
	
	// 웨이브 파일 읽기가 실패하면 돌아간다.
	MAKE_ZERO(m_mmckinfo);
	if(WaveOpenFile(szFileName, &m_hmmio, &wfmtx, &m_mmckinfo) == FALSE)
	{
		StopStream();
		MessageBox(m_hWnd, szFileName, "Wave File 읽기에 실패 하였습니다.", MB_OK);
		return;
	}
	
	if(WaveStartReadData(&m_hmmio, &m_mmckinfo) == FALSE)
	{
		StopStream();
		MessageBox(m_hWnd, szFileName, "Wave File 읽기에 실패 하였습니다.", MB_OK);
		return;
	}
		
	DSBUFFERDESC dsbdesc;
	MAKE_ZERO(dsbdesc);
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME;
	dsbdesc.dwBufferBytes = wfmtx.nAvgBytesPerSec * 1; // 1초 분량
	dsbdesc.lpwfxFormat = &wfmtx;

	if(m_lpDS->CreateSoundBuffer(&dsbdesc, &m_lpDSBStream, NULL) != DS_OK)
	{
		WaveCloseReadFile(&m_hmmio);
		MessageBox(m_hWnd, szFileName, "Streaming Sound Buffer 생성에 실패 하였습니다.", MB_OK);
		return;
	};
	
	FillBufferWithSilence(m_lpDSBStream); // 잡음이 플레이되는걸 막는다.
	m_lpDSBStream->Play(0, 0, DSBPLAY_LOOPING);
	m_dwMidBuffer = dsbdesc.dwBufferBytes / 2;
}

void CDirectSound::StopStream()
{
	if(m_hmmio != NULL) mmioClose(m_hmmio, 0); // 파일 닫기
	m_hmmio = NULL;
	
	RELEASE_SAFE(m_lpDSBStream);
}



int CDirectSound::WaveOpenFile(const char* pszFileName, HMMIO *phmmioIn, WAVEFORMATEX* pwfxInfo, MMCKINFO* pckInRIFF)
{
    HMMIO			hmmioIn;
    MMCKINFO		ckIn;       // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temporary PCM format structure to load in.   
    BOOL			nError;     // Return value.


    // Initialization...
    nError = TRUE;
    hmmioIn = NULL;
    
    if ((hmmioIn = mmioOpen((char*)pszFileName, NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
    {
        goto ERROR_READING_WAVE;
    }

    if (mmioDescend(hmmioIn, pckInRIFF, NULL, 0) != MMSYSERR_NOERROR)
    {
        goto ERROR_READING_WAVE;
    }


    if ((pckInRIFF->ckid != FOURCC_RIFF) || (pckInRIFF->fccType != mmioFOURCC('W', 'A', 'V', 'E')))
    {
        goto ERROR_READING_WAVE;
    }
            
    /* Search the input file for for the 'fmt ' chunk.     */
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
    {
        goto ERROR_READING_WAVE;        
    }
                    
    /* Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    * if there are extra parameters at the end, we'll ignore them */
    
    if (ckIn.cksize < (long) sizeof(PCMWAVEFORMAT))
    {
        goto ERROR_READING_WAVE;
    }
                                                                
    /* Read the 'fmt ' chunk into <pcmWaveFormat>.*/     
    if (mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, (long) sizeof(pcmWaveFormat)) != (long) sizeof(pcmWaveFormat))
    {
        goto ERROR_READING_WAVE;
    }
                            
    if (pcmWaveFormat.wf.wFormatTag != WAVE_FORMAT_PCM)
	{
		goto ERROR_READING_WAVE;
	}
                            
    // Copy the bytes from the pcm structure to the waveformatex structure
    memcpy(pwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));

    /* Ascend the input file out of the 'fmt ' chunk. */
    if (mmioAscend(hmmioIn, &ckIn, 0) != MMSYSERR_NOERROR)
    {
        goto ERROR_READING_WAVE;
    }
    

    goto TEMPCLEANUP;       

ERROR_READING_WAVE:
    if (hmmioIn != NULL)
	{
		mmioClose(hmmioIn, 0);
		hmmioIn = NULL;
	}
    nError = FALSE;
    
TEMPCLEANUP:
    *phmmioIn = hmmioIn;

    return(nError);

}


int CDirectSound::WaveStartReadData(HMMIO* phmmioIn, MMCKINFO* pckIn)
{
    if(pckIn == NULL) return FALSE;
	int nSeek = mmioSeek(*phmmioIn, 0, SEEK_SET);

	int nError = 0;

	pckIn->fccType = mmioFOURCC('W', 'A', 'V', 'E');
	nError = mmioDescend(*phmmioIn, pckIn, NULL, MMIO_FINDRIFF);
	if(nError != MMSYSERR_NOERROR) return FALSE;

	pckIn->ckid = mmioFOURCC('f', 'm', 't', ' ');
	nError = mmioDescend(*phmmioIn, pckIn, NULL, MMIO_FINDCHUNK);
	if(nError != MMSYSERR_NOERROR) return FALSE;

    //  Search the input file for for the 'data' chunk.
	nSeek = mmioSeek(*phmmioIn, sizeof(PCMWAVEFORMAT), SEEK_CUR);
	pckIn->ckid = mmioFOURCC('d', 'a', 't', 'a');
    nError = mmioDescend(*phmmioIn, pckIn, NULL, MMIO_FINDCHUNK);
	if(nError != MMSYSERR_NOERROR) return FALSE;
    
	return TRUE;
}

int CDirectSound::WaveReadFile(HMMIO hmmioIn, UINT cbRead, BYTE* pbDest, MMCKINFO* pckIn, UINT* cbActualRead)
{
	MMIOINFO    mmioinfoIn;         // current status of <hmmioIn>
    BOOL		nError;
    UINT		cT, cbDataIn;

	nError = TRUE;

    if (mmioGetInfo(hmmioIn, &mmioinfoIn, 0) != 0)
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
			if (mmioAdvance(hmmioIn, &mmioinfoIn, MMIO_READ) != MMSYSERR_NOERROR)
			{
				goto ERROR_CANNOT_READ;
			} 
			if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
			{
				goto ERROR_CANNOT_READ;
			}
		}

		// Actual copy.
		*((BYTE*)pbDest+cT) = *((BYTE*)mmioinfoIn.pchNext++);
    }

    if ((nError = mmioSetInfo(hmmioIn, &mmioinfoIn, 0)) != 0)
    {
        goto ERROR_CANNOT_READ;
    }

    *cbActualRead = cbDataIn;
    goto FINISHED_READING;

ERROR_CANNOT_READ:
    *cbActualRead = 0;
	nError = FALSE;

FINISHED_READING:
	nError = TRUE;
    return(nError);

}

int CDirectSound::WaveCloseReadFile(HMMIO* phmmio)
{
    if (*phmmio != NULL)
    {
        mmioClose(*phmmio, 0);
        *phmmio = NULL;
    }
	return(0);
}

void CDirectSound::FillBufferWithSilence(LPDIRECTSOUNDBUFFER lpdsb)
{
	if(m_lpDS == NULL || lpdsb == NULL) return;
	DSBCAPS dsbcaps;
	MAKE_ZERO(dsbcaps);
	dsbcaps.dwSize = sizeof(DSBCAPS);
	lpdsb->GetCaps(&dsbcaps);

	void* lpBuff1 = NULL;
	DWORD dwBytesLocked1 = 0;
	void* lpBuff2 = NULL;
	DWORD dwBytesLocked2 = 0;

	HRESULT rval = lpdsb->Lock(0, dsbcaps.dwBufferBytes, &lpBuff1, &dwBytesLocked1, &lpBuff2, &dwBytesLocked2, DSBLOCK_ENTIREBUFFER);

	if(rval != DS_OK) return;
	if(dwBytesLocked1) memset(lpBuff1, 0, dwBytesLocked1);
	lpdsb->Unlock(lpBuff1, dwBytesLocked1, NULL, 0);
}

// 0 ~ 1.0 까지 볼륨 조정
void CDirectSound::SetVolumeStream(float fVol)
{
	if(fVol < 0.0f) fVol = 0.0f;
	if(fVol > 1.0f) fVol = 1.0f;
	m_fVolStream = fVol;

	LONG vol = (LONG)(-10000.0f * (1.0f - fVol));
	if(m_lpDSBStream)
		m_lpDSBStream->SetVolume(vol);
}

float CDirectSound::GetVolumeStream()
{
	return m_fVolStream;
}

// 0 ~ 1.0 까지 볼륨 조정
void CDirectSound::SetVolumeStatic(float fVol)
{
	if(fVol < 0.0f) fVol = 0.0f;
	if(fVol > 1.0f) fVol = 1.0f;
	m_fVolStatic = fVol;

	LONG vol = (LONG)(-10000.0f * (1.0f - fVol));
	for(int i = 0; i < MAX_SOUND_CHANNEL; i++)
	{
		if(m_lpDSBs[i])
			m_lpDSBs[i]->SetVolume(vol);
	}
}

float CDirectSound::GetVolumeStatic()
{
	return m_fVolStatic;
}
