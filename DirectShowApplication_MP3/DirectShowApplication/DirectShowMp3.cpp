#include "stdafx.h"
#include "DirectShowMp3.h"

CDirectShowMp3::CDirectShowMp3()
{
	iGraphBuilder = NULL;
	iMediaControl = NULL;
	iMediaEventEx = NULL;
	iBasicAudio = NULL;
	iMediaSeeking = NULL;
	bReady = false;
	m_lduration = 0;
}

CDirectShowMp3::~CDirectShowMp3()
{
	Cleanup();
}

void CDirectShowMp3::Cleanup()
{
	if (iMediaControl) 
		iMediaControl->Stop();

	if (iMediaControl)
	{
		iMediaControl->Release();
		iMediaControl = NULL;
	}

	if (iMediaEventEx)
	{
		iMediaEventEx->Release();
		iMediaEventEx = NULL;
	}

	if (iBasicAudio) 
	{
		iBasicAudio->Release();
		iBasicAudio = NULL;
	}

	if (iMediaSeeking)
	{
		iMediaSeeking->Release();
		iMediaSeeking = NULL;
	}

	if (iGraphBuilder)
	{
		iGraphBuilder->Release();
		iGraphBuilder = NULL;
	}
	bReady = false;
}

bool CDirectShowMp3::Open(CString strFile)
{
	Cleanup();
	bReady = false;

	HRESULT hr;
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&iGraphBuilder);
	if (FAILED(hr)) return false;

	hr = iGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&iMediaControl);
	if (FAILED(hr)) return false;
	
	hr = iGraphBuilder->QueryInterface(IID_IMediaEventEx, (void **)&iMediaEventEx);
	if (FAILED(hr)) return false;

	hr = iGraphBuilder->QueryInterface(IID_IBasicAudio, (void**)&iBasicAudio);
	if (FAILED(hr)) return false;

	hr = iGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&iMediaSeeking);
	if (FAILED(hr)) return false;
	
	hr = iGraphBuilder->RenderFile(strFile, NULL);
	if (SUCCEEDED(hr))
	{
		bReady = true;
		if (iMediaSeeking)
		{
			iMediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
			iMediaSeeking->GetDuration(&m_lduration);
		}
	}
	return bReady;
}

bool CDirectShowMp3::Play()
{
	if (bReady && iMediaControl != NULL)
	{
		mStatus = GraphState::Playing;
		HRESULT hr = iMediaControl->Run();
		return SUCCEEDED(hr);
	}
	return false;
}

bool CDirectShowMp3::Pause()
{
	if (bReady && iMediaControl != NULL)
	{
		mStatus = GraphState::Paused;
		HRESULT hr = iMediaControl->Pause();
		return SUCCEEDED(hr);		
	}
	return false;
}

bool CDirectShowMp3::Stop()
{
	if (bReady && iMediaControl != NULL)
	{
		mStatus = GraphState::Stopped;
		HRESULT hr = iMediaControl->Stop();
		return SUCCEEDED(hr);
	}
	return false;
}

bool CDirectShowMp3::SetVolume(long lVolume)
{
	if (bReady && iBasicAudio != NULL)
	{
		HRESULT hr = iBasicAudio->put_Volume(lVolume);
		return SUCCEEDED(hr);
	}
	return false;
}

long CDirectShowMp3::GetVolume()
{
	if (bReady && iBasicAudio != NULL)
	{
		long lVolume;
		HRESULT hr = iBasicAudio->get_Volume(&lVolume);
		if (SUCCEEDED(hr))return lVolume;
	}
	return -10000L;
}

__int64 CDirectShowMp3::GetRunningTime()
{
	return m_lduration;
}

__int64 CDirectShowMp3::GetPosition()
{
	if (bReady && iMediaSeeking != NULL)
	{
		__int64 curpos = -1;
		HRESULT hr = iMediaSeeking->GetCurrentPosition(&curpos);
		if (SUCCEEDED(hr)) return curpos;
	}
	return -1;
}

bool CDirectShowMp3::SetPositions(__int64 pCurrent)
{
	if (bReady && iMediaSeeking != NULL)
	{
		__int64 pStop = -1;
		HRESULT hr = iMediaSeeking->GetStopPosition(&pStop);
		hr = iMediaSeeking->SetPositions(&pCurrent, AM_SEEKING_AbsolutePositioning, &pStop, AM_SEEKING_AbsolutePositioning);
		if (SUCCEEDED(hr)) return true;
	}
	return false;
}

bool CDirectShowMp3::IsPause()
{
	FILTER_STATE fs;
	HRESULT hr = iMediaControl->GetState(500, (OAFilterState*)&fs);
	return (fs == State_Paused);
}

bool CDirectShowMp3::IsPlaying()
{
	FILTER_STATE fs;
	HRESULT hr = iMediaControl->GetState(500, (OAFilterState*)&fs);
	return (fs == State_Running);
	/*
	if (iMediaControl == NULL) return false;

	//song end
	if (GetRunningTime() == GetPosition()) return false;

	FILTER_STATE fs;
	HRESULT hr = iMediaControl->GetState(500, (OAFilterState*)&fs);
	return (fs == State_Running);
	*/
}

bool CDirectShowMp3::IsEndPlaying()
{	
	if (GetRunningTime() == GetPosition()) return true;
	return false;
}

void CDirectShowMp3::SkipBack()
{
	if (bReady && iMediaSeeking != NULL)
	{
		__int64   curpos = 0;
		int nResult = iMediaSeeking->GetCurrentPosition(&curpos);
		__int64  pCurrent = (curpos - 10000000);
		nResult = iMediaSeeking->SetPositions(&pCurrent, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning);
	}
}

void CDirectShowMp3::SkipForward()
{
	if (bReady && iMediaSeeking != NULL)
	{
		__int64  curpos = 0;
		int nResult = iMediaSeeking->GetCurrentPosition(&curpos);
		__int64  pCurrent = (curpos + 10000000);
		nResult = iMediaSeeking->SetPositions(&pCurrent, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning);
	}
}
