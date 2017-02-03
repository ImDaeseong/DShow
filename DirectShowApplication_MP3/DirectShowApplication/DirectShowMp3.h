#pragma once

#include <dshow.h>
#pragma comment(lib, "strmiids.lib")

enum GraphState
{
	Stopped = 0,
	Paused = 1,
	Playing = 2
};

class CDirectShowMp3
{
public:
	CDirectShowMp3();
	~CDirectShowMp3();

public:
	void Cleanup();
	bool Open(CString strFile);
	bool Play();
	bool Pause();
	bool Stop();	
	bool SetVolume(long lVolume);
	long GetVolume();
	__int64 GetRunningTime();
	__int64 GetPosition();
	bool SetPositions(__int64 pCurrent);
	bool IsPause();
	bool IsPlaying();
	bool IsEndPlaying();
	void SkipBack();
	void SkipForward();	

private:
	IGraphBuilder* iGraphBuilder;
	IMediaControl* iMediaControl;
	IMediaEventEx* iMediaEventEx;
	IBasicAudio*   iBasicAudio;
	IMediaSeeking* iMediaSeeking;

	GraphState mStatus;

	bool bReady;
	__int64 m_lduration;
};

