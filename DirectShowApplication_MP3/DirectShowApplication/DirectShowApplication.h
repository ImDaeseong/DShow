#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		

class CDirectShowApplicationApp : public CWinApp
{
public:
	CDirectShowApplicationApp();

public:
	virtual BOOL InitInstance();
	
protected:
	ULONG_PTR	m_gdiplusToken;
	void SetClassName();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CDirectShowApplicationApp theApp;