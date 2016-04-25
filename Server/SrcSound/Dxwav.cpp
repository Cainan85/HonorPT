#include "dxWav.h"
#include <windowsx.h>
#include <mmsystem.h>
#include <conio.h>
#include "Log.h"

HRESULT     rval_g_lpDs;
HRESULT     dsrval_lpDirectSound;

int numSound = 0;

char dataDir[64];

int mouThRead;
BYTE *mouThdataOfs = NULL;

BOOL wav_opt = 1;

LPDIRECTSOUND           g_lpDS = NULL;              // DirectSound object
BOOL                    g_bSoundPresent = FALSE;    // Do we have sound capabilites?
LPDIRECTSOUNDBUFFER     g_lpSounds[NUM_SOUNDS];     // Sound buffers
BOOL                    g_bSoundPaused = FALSE;     // Defined in WINMAIN.CPP


char sangJuWav[NUM_SOUNDS][64];
int  sangJuWav_F[NUM_SOUNDS] = { 0 };		//ÇÃ·¹±×¼ÂÆÃ

//ÇÃ·¹±×¸¦ ÀÌ¿ëÇØ Áß°£ ·ÎµåÇÔ.
int chk_load_sangJuWav(int wavN)
{
	Log::Debug("InitDirectSound : Requisitado porém foi removido.");
	return NULL;
}

char *sangJuWav2[NUM_SOUNDS] = {
"wav\\rain01.wav",
"wav\\thunder1.wav",
"wav\\Button.wav",
"wav\\New.wav",
"Wav\\CharacterDelete.wav",
"Wav\\ButtonBack.wav",
"Wav\\CharacterSelect.wav",

"Wav\\NewChar.wav",
"Wav\\Fire.wav",
"Wav\\Thunder.wav",
"Wav\\GameStart.wav",
"Wav\\Login.wav",
NULL
};

char *wnameBuf[NUM_SOUNDS] = {
"wav\\bgm0.wav",
"wav\\rain01.wav",
"wav\\thunder1.wav",
"wav\\step01.wav",
"Wav\\Effects\\weapon\\one hand swing - small 3.wav",
	NULL
};

int isPlaying(int i);

//=============== ¹è°æ¹ÂÁ÷ °ü·Ã ÇÔ¼ö ========================//
char *backMusicWav[] = {   //ÀÓ½Ã·Î ³ÖÀº°ÍÀÓ.
"wav\\622s.wav",	//0
NULL,
NULL,
};

int  BackMusic_Load(int stageNumber)
{
	Log::Debug("BackMusic_Load : Requisitado porém foi removido.");
	return NULL;
}

void BackMusicPlay()
{
	Log::Debug("BackMusicPlay : Requisitado porém foi removido.");
	return;
}

void BackMusicClr()
{
	Log::Debug("BackMusicClr : Requisitado porém foi removido.");
	return;
}

void PlayWavEffect(int num)
{
	Log::Debug("PlayWavEffect : Requisitado porém foi removido.");
	return;
}

void BackMusic_Stop()
{
	Log::Debug("BackMusic_Stop : Requisitado porém foi removido.");
	return;
}

BOOL InitDX_Wav(HWND g_hwnd)
{
	Log::Debug("InitDX_Wav : Requisitado porém foi removido.");
	return NULL;
}

void TermDX_Wav()
{
	Log::Debug("TermDX_Wav : Requisitado porém foi removido.");
	return;
}

BOOL CreateSoundBuffer(DWORD dwBuf, DWORD dwBufSize, DWORD dwFreq, DWORD dwBitsPerSample, DWORD dwBlkAlign, BOOL bStereo)
{
	Log::Debug("CreateSoundBuffer : Requisitado porém foi removido.");
	return NULL;
}

BOOL ReadData(LPDIRECTSOUNDBUFFER lpDSB, FILE* pFile, DWORD dwSize, DWORD dwPos)
{
	Log::Debug("ReadData : Requisitado porém foi removido.");
	return NULL;
}

BOOL CreateBufferFromWaveFile(char* FileName, DWORD dwBuf)
{
	Log::Debug("CreateBufferFromWaveFile : Requisitado porém foi removido.");
	return NULL;
}

BOOL ReadDataWavBuf(LPDIRECTSOUNDBUFFER lpDSB, BYTE *wavBUF, DWORD dwSize)
{
	Log::Debug("ReadDataWavBuf : Requisitado porém foi removido.");
	return NULL;
}

BOOL CreateBufferFromWavBUF(unsigned char* wavBUf, unsigned int dwBuf)
{
	Log::Debug("CreateBufferFromWavBUF : Requisitado porém foi removido.");
	return NULL;
}

//----------------------------------------------------------------------
// 
// Function     : StopAllSounds()
//
// Purpose      : Stops all sounds
//
//----------------------------------------------------------------------

BOOL StopAllWav()
{
	Log::Debug("StopAllWav : Requisitado porém foi removido.");
	return NULL;
}

#define VOL_SHIFT (-400)
#define VOL_MULT 10

#define PAN_SHIFT (-400)
#define PAN_MULT  10

#define FRQ_MULT  10

BOOL PlayWav(DWORD dwSound, DWORD dwFlags, DWORD dwVol, DWORD dwfreq)
{
	Log::Debug("PlayWav : Requisitado porém foi removido.");
	return NULL;
}

BOOL PlayWav2(DWORD dwSound, DWORD dwFlags, DWORD dwVol, DWORD dwfreq)
{
	Log::Debug("PlayWav2 : Requisitado porém foi removido.");
	return NULL;
}

BOOL IsPlayWav(DWORD dwSound)
{
	Log::Debug("IsPlayWav : Requisitado porém foi removido.");
	return NULL;
}

BOOL PlayWav3(DWORD dwSound, DWORD dwFlags, DWORD dwVol, DWORD dwfreq, DWORD dwPan)
{
	Log::Debug("PlayWav3 : Requisitado porém foi removido.");
	return NULL;
}

BOOL PlayWav_VolChange(DWORD dwSound, DWORD dwVol)
{
	Log::Debug("PlayWav_VolChange : Requisitado porém foi removido.");
	return NULL;
}

BOOL PlayWav_FrequencyChange(DWORD dwSound, DWORD dwfreq)
{
	Log::Debug("PlayWav_FrequencyChange : Requisitado porém foi removido.");
	return NULL;
}

BOOL PlayWav_PanChange(DWORD dwSound, DWORD dwPan)
{
	Log::Debug("PlayWav_PanChange : Requisitado porém foi removido.");
	return NULL;
}

BOOL StopWav(int i);

void check_backMus(int flag)
{
	Log::Debug("check_backMus : Requisitado porém foi removido.");
	return;
}

BOOL InitDX_Wav2(HWND g_hwnd)
{
	Log::Debug("InitDX_Wav2 : Requisitado porém foi removido.");
	return NULL;
}

void TermDX_Wav2()
{
	Log::Debug("TermDX_Wav2 : Requisitado porém foi removido.");
	return;
}

BOOL CreateBufferFromWaveFile2(char* FileName, int sbufN)
{
	Log::Debug("CreateBufferFromWaveFile2 : Requisitado porém foi removido.");
	return NULL;
}

BOOL sBufferClear(int sbufN)
{
	Log::Debug("sBufferClear : Requisitado porém foi removido.");
	return NULL;
}

BOOL StopWav(int i)
{
	Log::Debug("StopWav : Requisitado porém foi removido.");
	return NULL;
}

void allWAVE_playORstop(int flag)
{
	Log::Debug("allWAVE_playORstop : Requisitado porém foi removido.");
	return;
}

DWORD get_pos(DWORD dwSound)
{
	Log::Debug("get_pos : Requisitado porém foi removido.");
	return NULL;
}

BYTE getmouTH()
{
	Log::Debug("getmouTH : Requisitado porém foi removido.");
	return NULL;
}

int isPlaying(int i)
{
	Log::Debug("isPlaying : Requisitado porém foi removido.");
	return NULL;
}

#include "wave.h"
#include "shell.h"
#include "stream.h"

HRESULT SetupStreamBuffer(LPSTR lpszFilename);

LPDIRECTSOUND       lpDirectSound = NULL;

LPDIRECTSOUNDBUFFER lpDSBStreamBuffer = NULL;

HWND    m_hWnd;

LONG  m_lInTimer = FALSE;
BOOL  m_bTimerInit = FALSE;
BOOL  m_bPlaying = FALSE;
BOOL  m_bIsCloseOnRun = FALSE;
BOOL  m_bFileOpen = FALSE;
UINT  m_uTimerID = 0;
DWORD m_SecondBuffFocus = 0;

WAVEINFOCA  m_wiWave;

void LSStopBackground(DWORD dwState);
void LSResetWavePlayer(void);
void LSFillDataBuffer(void);
void CALLBACK LSTimeRunFuncForDSound(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

void LSStopBackground(DWORD dwState)
{
	Log::Debug("LSStopBackground : Requisitado porém foi removido.");
	return;
}

void LSResetWavePlayer(void)
{
	Log::Debug("LSResetWavePlayer : Requisitado porém foi removido.");
	return;
}

void LSFillDataBuffer(void)
{
	Log::Debug("LSFillDataBuffer : Requisitado porém foi removido.");
	return;
}

void CALLBACK LSTimeRunFuncForDSound(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	Log::Debug("LSTimeRunFuncForDSound : Requisitado porém foi removido.");
	return;
}

BOOL OpenBGM(LPSTR lpszFileName)
{
	Log::Debug("OpenBGM : Requisitado porém foi removido.");
	return NULL;
}

HRESULT SetupStreamBuffer(LPSTR lpszFilename)
{
	Log::Debug("SetupStreamBuffer : Requisitado porém foi removido.");
	return NULL;
}

void PlayBGM(void)
{
	Log::Debug("PlayBGM : Requisitado porém foi removido.");
	return;
}

void StopBGM(void)
{
	Log::Debug("StopBGM : Requisitado porém foi removido.");
	return;
}

void SetVolumeBGM(DWORD dwVol)
{
	Log::Debug("SetVolumeBGM : Requisitado porém foi removido.");
	return;
}

BOOL Init_BGM(HWND hwnd)
{
	Log::Debug("Init_BGM : Requisitado porém foi removido.");
	return NULL;
}

void Uninit_BGM(void)
{
	Log::Debug("Uninit_BGM : Requisitado porém foi removido.");
	return;
}

void InitDirectSound()
{
	Log::Debug("InitDirectSound : Requisitado porém foi removido.");
	return;
}
