#include "stdafx.h"
#include "DirectShowApplication.h"
#include "DirectShowApplicationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDirectShowApplicationDlg::CDirectShowApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIRECTSHOWAPPLICATION_DIALOG, pParent)
{
	m_pBackSkin = NULL;

	m_nIndex = 0;
	
	m_strTitle = "";
	m_strTime = "";
	m_strCurrentMp3Path = "";

	m_lVolumn = 0;
}

void CDirectShowApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_BUTTON_ADDFILE, m_btnAddFile);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_LEFT, m_btnLeft);
	DDX_Control(pDX, IDC_BUTTON_RIGHT, m_btnRight);
	DDX_Control(pDX, IDC_BUTTON_PLUS, m_btnPlus);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
}

BEGIN_MESSAGE_MAP(CDirectShowApplicationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CDirectShowApplicationDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_ADDFILE, &CDirectShowApplicationDlg::OnBnClickedButtonAddfile)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDirectShowApplicationDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &CDirectShowApplicationDlg::OnBnClickedButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CDirectShowApplicationDlg::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, &CDirectShowApplicationDlg::OnBnClickedButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDirectShowApplicationDlg::OnBnClickedButtonMin)
END_MESSAGE_MAP()

void CDirectShowApplicationDlg::OnDestroy()
{
	m_mp3Info.clear();

	KillTimer(1);

	if (m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;

	CDialogEx::OnDestroy();
}

void CDirectShowApplicationDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		CString sDuration;
		int duration = m_Player.GetRunningTime() / 10000000;
		__int64 curPos = m_Player.GetPosition();

		int timeElapsedSec = curPos / 10000000;
		sDuration.Format(L"%d:%02d/%d:%02d", timeElapsedSec / 60, timeElapsedSec % 60, duration / 60, duration % 60);
		m_strTime = sDuration;
		Invalidate();

		if (m_Player.IsEndPlaying())
		{
			KillTimer(1);
			NextPlayMusic();
		}		
	}

	CDialogEx::OnTimer(nIDEvent);
}

BOOL CDirectShowApplicationDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CDirectShowApplicationDlg::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	CDC* pDC = GetDC();
	if (pDC != NULL)
	{
		DrawSkin(pDC);
		ReleaseDC(pDC);
	}
	MoveLocationDialog();
	return 0;
}

void CDirectShowApplicationDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawSkin(&dc);
}

void CDirectShowApplicationDlg::InitButtons()
{	
	CRect rClient;
	GetClientRect(&rClient);

	m_btnPlay.SetImage(MAKEINTRESOURCE(IDB_PNG_PLAY), _T("PNG"), AfxGetApp()->m_hInstance, ((rClient.right - rClient.left) / 2) - 11, 112, 1);
	m_btnAddFile.SetImage(MAKEINTRESOURCE(IDB_PNG_ADDFILE), _T("PNG"), AfxGetApp()->m_hInstance, ((rClient.right - rClient.left) / 2) - 27, 222, 1);
	m_btnClose.SetImage(MAKEINTRESOURCE(IDB_PNG_CLOSE), _T("PNG"), AfxGetApp()->m_hInstance, (rClient.right - 25), 7, 1);
	m_btnLeft.SetImage(MAKEINTRESOURCE(IDB_PNG_LEFT), _T("PNG"), AfxGetApp()->m_hInstance, ((rClient.right - rClient.left) / 2) - 67, 112, 1);
	m_btnRight.SetImage(MAKEINTRESOURCE(IDB_PNG_RIGHT), _T("PNG"), AfxGetApp()->m_hInstance, ((rClient.right - rClient.left) / 2) + 44, 112, 1);
	m_btnPlus.SetImage(MAKEINTRESOURCE(IDB_PNG_PLUS), _T("PNG"), AfxGetApp()->m_hInstance, ((rClient.right - rClient.left) / 2) - 11, 57, 1);
	m_btnMin.SetImage(MAKEINTRESOURCE(IDB_PNG_MIN), _T("PNG"), AfxGetApp()->m_hInstance, ((rClient.right - rClient.left) / 2) - 11, 167, 1);

	m_btnPlay.SetToolTipText(L"Play");
	m_btnAddFile.SetToolTipText(L"AddFile");
	m_btnClose.SetToolTipText(L"Close");
	m_btnLeft.SetToolTipText(L"Left");
	m_btnRight.SetToolTipText(L"Right");
	m_btnPlus.SetToolTipText(L"Plus");
	m_btnMin.SetToolTipText(L"Min");	
}

BOOL CDirectShowApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW | WS_EX_LAYERED);

	if (!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	CDC* pDC = GetDC();
	DrawSkin(pDC);
	ReleaseDC(pDC);
	
	return TRUE; 
}

CString CDirectShowApplicationDlg::GetFileExtName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('.');
	if (nPos > 0)
		return strFilename.Mid(nPos + 1);
	return strFilename;
}

CString CDirectShowApplicationDlg::GetFileName(CString strFilename)
{
	int nPos = strFilename.ReverseFind('.');
	if (nPos > 0)
		return strFilename.Left(nPos);

	return strFilename;
}

CString CDirectShowApplicationDlg::GetModulePath()
{
	TCHAR cTemp[MAX_PATH];
	TCHAR *spzRealDirEnd;
	CString strModulePath;

	GetModuleFileName(NULL, cTemp, MAX_PATH);
	spzRealDirEnd = _tcsrchr(cTemp, _T('\\'));
	*spzRealDirEnd = '\0';

	return strModulePath = (CString)cTemp;
}

BOOL CDirectShowApplicationDlg::LoadSkin()
{
	m_pBackSkin = new CGdiPlusBitmapResource;
	if (!m_pBackSkin->Load(IDB_PNG_BG, _T("PNG"), AfxGetApp()->m_hInstance))
	{
		delete m_pBackSkin;
		m_pBackSkin = NULL;
		return FALSE;
	}

	MoveLocationDialog();

	InitButtons();

	return TRUE;
}

void CDirectShowApplicationDlg::DrawSkin(CDC* pDC)
{
	CDC memDC;
	CBitmap bmp;
	CBitmap* bmp_old;

	CRect rect;
	GetWindowRect(&rect);

	int cx = rect.Width();
	int cy = rect.Height();

	memDC.CreateCompatibleDC(pDC);

	BITMAPINFO bmInfo;
	memset(&bmInfo, 0x00, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = cx;
	bmInfo.bmiHeader.biHeight = cy;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biClrUsed = 0;
	bmInfo.bmiHeader.biClrImportant = 0;

	LPBYTE pbmpBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO)&bmInfo, DIB_RGB_COLORS, (void **)&pbmpBits, NULL, 0);
	if (hBitmap == NULL)
		bmp.CreateCompatibleBitmap(pDC, cx, cy);
	else
		bmp.Attach(hBitmap);

	bmp_old = memDC.SelectObject(&bmp);
	Graphics gps(memDC.GetSafeHdc());

	gps.DrawImage(m_pBackSkin->m_pBitmap,
		Rect(0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight()),
		0, 0, m_pBackSkin->m_pBitmap->GetWidth(), m_pBackSkin->m_pBitmap->GetHeight(), UnitPixel);

	if (m_btnPlay.IsAvailableDraw())
		m_btnPlay.OnDrawLayerdWindow(gps);

	if (m_btnAddFile.IsAvailableDraw())
		m_btnAddFile.OnDrawLayerdWindow(gps);

	if (m_btnClose.IsAvailableDraw())
		m_btnClose.OnDrawLayerdWindow(gps);

	if (m_btnLeft.IsAvailableDraw())
		m_btnLeft.OnDrawLayerdWindow(gps);

	if (m_btnRight.IsAvailableDraw())
		m_btnRight.OnDrawLayerdWindow(gps);

	if (m_btnPlus.IsAvailableDraw())
		m_btnPlus.OnDrawLayerdWindow(gps);

	if (m_btnMin.IsAvailableDraw())
		m_btnMin.OnDrawLayerdWindow(gps);
		
	DisplayLyrics(gps);

	CPoint ptWindowScreenPosition(rect.TopLeft());
	CSize szWindow(cx, cy);

	BLENDFUNCTION blendPixelFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	CPoint ptSrc(0, 0);

	HDC hdc = gps.GetHDC();
	BOOL bRet = ::UpdateLayeredWindow(GetSafeHwnd(), pDC->GetSafeHdc(), &ptWindowScreenPosition, &szWindow, hdc, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
	gps.ReleaseHDC(hdc);
	memDC.SelectObject(bmp_old);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

void CDirectShowApplicationDlg::DisplayLyrics(Graphics& gps)
{
	USES_CONVERSION;

	Gdiplus::Font font1(L"±¼¸²", 11, FontStyleBold, UnitPixel);
	SolidBrush brush1(Color(234, 137, 6));
		
	StringFormat stringFormat(StringFormatFlagsDisplayFormatControl);
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	CRect rect;
	GetClientRect(&rect);

	RectF rectF1(REAL(rect.left + 1), REAL(rect.bottom - 30), REAL(rect.Width()), REAL(30));
	gps.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	gps.DrawString(m_strTime, -1, &font1, rectF1, &stringFormat, &brush1);		
}

void CDirectShowApplicationDlg::MoveLocationDialog()
{
	int nOffset = 0;
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

	MoveWindow(rcWorkArea.right - cx - nOffset, rcWorkArea.bottom - cy - nOffset, cx, cy);
	::SetWindowPos(this->m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

void CDirectShowApplicationDlg::OnBnClickedButtonPlay()
{
	if( m_Player.IsPause())
		m_Player.Play();
	else
		m_Player.Pause();
}

void CDirectShowApplicationDlg::OnBnClickedButtonAddfile()
{
	CString strSelectPath;
	LPMALLOC pMalloc;

	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		TCHAR pszBuffer[MAX_PATH];
		LPITEMIDLIST pidl;

		bi.hwndOwner = GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszBuffer;
		bi.lpszTitle = _T("Select Mp3");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;

		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, pszBuffer))
			{
				strSelectPath = pszBuffer;
			}

			pMalloc->Free(pidl);
		}
		pMalloc->Release();
	}

	if (strSelectPath == "")
		return;

	CStringArray aryMp3lst;
	SetFileInfo(strSelectPath, aryMp3lst);

	mp3Info vInfo;
	for (int i = 0; i < aryMp3lst.GetCount(); i++)
	{	
		vInfo.mp3 = aryMp3lst.GetAt(i);
		m_mp3Info.push_back(vInfo);
	}
	aryMp3lst.RemoveAll();

	NextPlayMusic(TRUE);
}

void CDirectShowApplicationDlg::OnBnClickedButtonClose()
{
	CDialog::OnOK();
}

void CDirectShowApplicationDlg::OnBnClickedButtonLeft()
{
	m_Player.SkipBack();
	
	//PrePlayMusic();
}

void CDirectShowApplicationDlg::OnBnClickedButtonRight()
{
	m_Player.SkipForward(); 
	
	//NextPlayMusic();
}

void CDirectShowApplicationDlg::OnBnClickedButtonPlus()
{
	m_lVolumn += 500;

	if (m_lVolumn > 0)
		m_lVolumn = 0;

	m_Player.SetVolume(m_lVolumn);
}

void CDirectShowApplicationDlg::OnBnClickedButtonMin()
{
	m_lVolumn -= 500;

	if (m_lVolumn < -10000)
		m_lVolumn = -10000;

	m_Player.SetVolume(m_lVolumn);
}

void CDirectShowApplicationDlg::SetFileInfo(CString strPath, CStringArray &aryMp3lst)
{
	CFileFind finder;
	CString strWildcard = strPath;
	strWildcard += _T("\\*.*");

	CString strFullPath;
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			strFullPath = finder.GetFilePath();
			SetFileInfo(strFullPath, aryMp3lst);
		}
		else
		{
			CString strFullPath = finder.GetFilePath();
			CString strFileName = finder.GetFileName();
			CString strExtention = strFileName.Mid(strFileName.ReverseFind('.') + 1);

			if (lstrcmpi(strExtention, _T("mp3")) == 0)
				aryMp3lst.Add(strFullPath);
		}
	}
	finder.Close();
}

CString CDirectShowApplicationDlg::ReadMp3Info()
{	
	CString strReturn;

	CFile mp3file(m_strCurrentMp3Path, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);
	mp3file.Seek(-128, CFile::end);

	char tempbuff[2];
	char tagbuff[124];
	char tagBuffer[4];
	mp3file.Read(tagBuffer, 3);

	char tag[4] = "TAG";

	if (strncmp(tag, tagBuffer, 3) == 0)
	{
		memset(tagbuff, 0, 124);
		for (int i = 0; i <= 122; i++)
		{
			memset(tempbuff, 0, 2);
			mp3file.Seek(-125 + i, CFile::end);
			mp3file.Read(tempbuff, 1);

			if (tempbuff[0] != '\0') tagbuff[i] = tempbuff[0];
			else tagbuff[i] = 0x20;
		}

		mp3file.Seek(-2, CFile::end);
		unsigned char tempbuff2[3];
		memset(tempbuff2, 0, 3);
		mp3file.Read(tempbuff2, 2);
		int track = tempbuff2[0];
		int genre = tempbuff2[1];

		CString tagstr;
		tagstr = tagbuff;

		CString szTitle = tagstr.Left(30);
		CString szArtist = tagstr.Mid(30, 30);

		szTitle.TrimRight();
		szArtist.TrimRight();
		strReturn.Format(_T("%s - %s"), szArtist, szTitle);
	}
	mp3file.Close();
	return strReturn;
}

void CDirectShowApplicationDlg::PrePlayMusic()
{	
	if (m_mp3Info.size() == 0) return;

	m_nIndex--;

	if (m_nIndex < 0)
		m_nIndex = m_mp3Info.size() - 1;

	m_strCurrentMp3Path = m_mp3Info[m_nIndex].mp3;
	m_strTitle = ReadMp3Info();

	Stop();
	Play();
}

void CDirectShowApplicationDlg::NextPlayMusic(bool bFirstPlay)
{
	if (m_mp3Info.size() == 0) return;

	if (bFirstPlay)
	{
		m_nIndex = 0;
		m_strCurrentMp3Path = m_mp3Info[m_nIndex].mp3;
		m_strTitle = ReadMp3Info();

		Stop();
		Play();
	}
	else
	{
		m_nIndex++;

		if (m_nIndex > (m_mp3Info.size() - 1))
			m_nIndex = 0;

		m_strCurrentMp3Path = m_mp3Info[m_nIndex].mp3;
		m_strTitle = ReadMp3Info();

		Stop();
		Play();
	}
}

void CDirectShowApplicationDlg::Stop()
{
	m_Player.Stop();

	KillTimer(1);
}

void CDirectShowApplicationDlg::Pause()
{
	m_Player.Pause();
}

void CDirectShowApplicationDlg::Play()
{
	m_Player.Open(m_strCurrentMp3Path);

	KillTimer(1);
	SetTimer(1, 1000, NULL);

	m_Player.Play();	
	m_Player.SetVolume(m_lVolumn);
}