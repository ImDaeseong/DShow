#pragma once
#include "afxwin.h"

struct mp3Info
{
	CString mp3;
	CString lyric;
};

class CDirectShowApplicationDlg : public CDialogEx
{
public:
	CDirectShowApplicationDlg(CWnd* pParent = NULL);	
	
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIRECTSHOWAPPLICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	
	
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:	
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonAddfile();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonLeft();
	afx_msg void OnBnClickedButtonRight();	
	afx_msg void OnBnClickedButtonPlus();
	afx_msg void OnBnClickedButtonMin();
	CSkinButton m_btnPlay;
	CSkinButton m_btnAddFile;
	CSkinButton m_btnClose;
	CSkinButton m_btnLeft;
	CSkinButton m_btnRight;
	CSkinButton m_btnPlus;
	CSkinButton m_btnMin;

private:
	CString GetFileExtName(CString strFilename);
	CString GetFileName(CString strFilename);
	CString GetModulePath();
	void MoveLocationDialog();
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	void InitButtons();
	CGdiPlusBitmapResource* m_pBackSkin;	
	void DisplayLyrics(Graphics& gps);

	CString ReadMp3Info();
	void SetFileInfo(CString strPath, CStringArray &aryMp3lst);
	
	CString m_strTime;
	CString m_strTitle;
	CString m_strCurrentMp3Path;
	
	std::vector<mp3Info> m_mp3Info;
	int m_nIndex;

	void PrePlayMusic();
	void NextPlayMusic(bool bFirstPlay = false);

	CDirectShowMp3 m_Player;
	void Stop();
	void Pause();
	void Play();

	long m_lVolumn;
};
