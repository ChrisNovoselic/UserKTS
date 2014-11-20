
// PseudoKTS.h : ������� ���� ��������� ��� ���������� PseudoKTS
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

//#include "resource.h"       // �������� �������

// CPseudoKTSApp:
// � ���������� ������� ������ ��. PseudoKTS.cpp
//

class CPseudoKTSApp : public HWinAppKTS {
public:
    enum TYPE_THREAD {THREAD_GUI_FILESDB, THREAD_GUI_CONTROL, THREAD_GETDATA, THREAD_WRITEDATA, THREAD_TIMER, THREAD_DIRECTORYWATCHED, COUNT_TYPE_THREAD};

	CPseudoKTSApp ();
    virtual ~CPseudoKTSApp ();

// ���������������
public:
	virtual int InitInstance ();
	virtual int ExitInstance ();

    virtual int SetOption ();

	//virtual int Run () { return HWinApp::Run (); }

	void DestroyUIThreads (void);
    BOOL m_bDestroyUIThreads;

    HWinThread *GetPtrThread (WORD );
    HANDLE m_hEnterDirectoryWatched;

// ����������
	afx_msg void OnAppAbout ();
    afx_msg void OnThreadGuiFilesDBClose (WPARAM , LPARAM );
    afx_msg void OnThreadTimer (WPARAM , LPARAM );
    afx_msg void OnThreadEnterDirectoryWatched (WPARAM , LPARAM );
    afx_msg void OnThreadLeaveDirectoryWatched (WPARAM , LPARAM );
    afx_msg void OnRestoreDlg (WPARAM , LPARAM );
    afx_msg void OnSourceDBselectedChanged (WPARAM , LPARAM );
    afx_msg void OnQueryThreadId (WPARAM , LPARAM );
    afx_msg void OnQueryNumberPTICControl (WPARAM , LPARAM );

protected:

private:
    CMapWordToPtr m_mapPtrThread;
    CRITICAL_SECTION m_CriticalSectionPtrThread;

	DWORD CreateThreadGUIFilesDB (void);
    DWORD CreateThreadGUIControl (void);
	DWORD CreateThreadGetData (void);
	DWORD CreateThreadWriteData (void);
	DWORD CreateThreadTimer (void);
	DWORD CreateThreadDirectoryWatched ();

	DECLARE_MESSAGE_MAP()
};

//extern CPseudoKTSApp theApp;
