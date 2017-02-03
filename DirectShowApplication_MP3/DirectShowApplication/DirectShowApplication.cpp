#include "stdafx.h"
#include "DirectShowApplication.h"
#include "DirectShowApplicationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDirectShowApplicationApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CDirectShowApplicationApp::CDirectShowApplicationApp()
{
	m_gdiplusToken = NULL;
}

CDirectShowApplicationApp theApp;

BOOL CDirectShowApplicationApp::InitInstance()
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("DirectShowApplicationInstance_"));
	if (ERROR_ALREADY_EXISTS == ::GetLastError())
		return FALSE;

	CoInitialize(NULL);

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	SetClassName();
	
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		
	CDirectShowApplicationDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	else if (nResponse == -1)
	{
	}
	
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	return FALSE;
}

int CDirectShowApplicationApp::ExitInstance()
{
	if (m_gdiplusToken != NULL)
	{
		GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = NULL;
	}

	CoUninitialize();

	return CWinApp::ExitInstance();
}

void CDirectShowApplicationApp::SetClassName()
{
	WNDCLASS wc;
	GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);;

	wc.lpszClassName = _T("DirectShowApplicationModule");
	AfxRegisterClass(&wc);
}