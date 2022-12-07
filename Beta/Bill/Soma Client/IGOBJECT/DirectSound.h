// DirectSound.h: interface for the CDirectSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTSOUND_H__01D43004_1A98_11D2_B0C7_0060970DBAD5__INCLUDED_)
#define AFX_DIRECTSOUND_H__01D43004_1A98_11D2_B0C7_0060970DBAD5__INCLUDED_

const int MAX_SOUND_CHANNEL = 64; // 버퍼 갯수
const int MAX_SOUND_DUPLICATE  = 8; // 중복 버퍼 갯수..

//#include <mmreg.h>
#include <mmsystem.h>
#include <DSound.h>

class CDirectSound
{
protected:
	int WaveOpenFile(const char* pszFileName, HMMIO *phmmioIn, WAVEFORMATEX* ppwfxInfo, MMCKINFO* pckInRIFF);
	int WaveStartReadData(HMMIO* phmmioIn, MMCKINFO* pckIn);
	int WaveReadFile(HMMIO hmmioIn, UINT cbRead, BYTE* pbDest, MMCKINFO* pckIn, UINT* cbActualRead);
	int WaveCloseReadFile(HMMIO* phmmio);
	void FillBufferWithSilence(LPDIRECTSOUNDBUFFER lpdsb);
	
	LPDIRECTSOUNDBUFFER		CreateStaticBuffer(int nCh, int nBit, DWORD dwRate, DWORD dwBytes); // 채널, 비트수, 샘플링, 바이트수..

protected:
	HWND					m_hWnd; 			//application's window handle
	LPDIRECTSOUND m_lpDS;
	LPDIRECTSOUNDBUFFER		m_lpPDSB; // 1 차 사운드 버퍼
	
	LPDIRECTSOUNDBUFFER		m_lpDSBs[MAX_SOUND_CHANNEL]; // 2차 사운드 버퍼
	LPDIRECTSOUNDBUFFER		m_lpDSBDups[MAX_SOUND_CHANNEL][MAX_SOUND_DUPLICATE]; // 최고 복제 버퍼..

	char	m_szSoundFileName[MAX_SOUND_CHANNEL][128];	// 2차 사운드 버퍼에 할당된 사운드 파일 이름들
	
	LPDIRECTSOUNDBUFFER	m_lpDSBStream;		// 스트리밍 사운드 버퍼
	HMMIO				m_hmmio;			// 스트리밍 파일 핸들..
	DWORD				m_dwMidBuffer;		// 스트리밍 버퍼 사이즈의 정확히 중간.
	DWORD				m_dwLastPlayPos;	// 스트리밍 버퍼 사이즈의 마지막.
	MMCKINFO			m_mmckinfo;			// 스트리밍 청크 인포

	float 	m_fVolStream; // 배경음악 볼륨.. 0 ~ 99;
	float 	m_fVolStatic; // 효과음 볼륨..(전체 채널에 적용된다.); 0 ~ 99;

public:
	float 	GetVolumeStream();
	void 	SetVolumeStream(float fVol);
	float 	GetVolumeStatic();
	void 	SetVolumeStatic(float fVol);
	
	void	SetupStream(const char* szFileName);
	void	StopStream();
	void	StreamProc();

	CDirectSound();
	virtual ~CDirectSound();
	
	void Init(HWND hwnd);
	void UnloadAllWave();
	BOOL LoadWave(int nSChn, const char* szFileName);
	void UnloadWave(int nSChn);
	void Play(int nSChn, BOOL bLooping = FALSE, float fVol = 1.0f);
	void Stop(int channel);
	void StopAllChannel();
};

#endif // !defined(AFX_DIRECTSOUND_H__01D43004_1A98_11D2_B0C7_0060970DBAD5__INCLUDED_)
