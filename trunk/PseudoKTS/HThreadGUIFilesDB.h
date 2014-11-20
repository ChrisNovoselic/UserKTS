#pragma once
#include <hwinthread.h>

#include "HDlgGUIFilesDB.h"

//#include "resource.h"		// main symbols

class HThreadGUIFilesDB : public HWinThread {
	DECLARE_DYNCREATE (HThreadGUIFilesDB)

public:
    //enum NAME_THREAD {THREAD_GUI_FILESDB, THREAD_COUNT};

	HThreadGUIFilesDB (void);
	virtual ~HThreadGUIFilesDB (void);

	int InitInstance (void);
	int ExitInstance (void);

	enum ID_DATAITEM {INT_SOURCE_ID = 86, INT_NUMBER_SELECTED_SOURCE, COUNT_ID_DATAITEM};

	//������� 1-�� ������ ����� ������������� ������
    afx_msg void OnResumeRun (WPARAM, LPARAM );
    //��������� 'INT_SOURCE_ID', 'INT_NUMBER_SELECTED_SOURCE' ��� ������������� ������� ��������
    //afx_msg void OnConnectToServer (WPARAM , LPARAM );
    //����������� ��������� ���������� � ������� ��
    afx_msg void OnMessageDirectoryWatched (WPARAM , LPARAM );
    //���� ��������� �� ������� ������ � ������������� ����������� (SEC_15)
    afx_msg void OnMessageTimer (WPARAM , LPARAM );
    //����� ������� � ������������� ���������� �������� � ���������� ����������
    afx_msg void OnMessageQueryDataConnectToServer (WPARAM , LPARAM );

    afx_msg void OnMessageInitComboboxDirWatch (WPARAM , LPARAM );

    //HDlgGUIFilesDB *m_pDlgGUI;

protected:

	DECLARE_MESSAGE_MAP()
};
