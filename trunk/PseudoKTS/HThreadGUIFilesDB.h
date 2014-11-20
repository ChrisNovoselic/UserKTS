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

	//Функция 1-го вызова после возобновления потока
    afx_msg void OnResumeRun (WPARAM, LPARAM );
    //Сохраняем 'INT_SOURCE_ID', 'INT_NUMBER_SELECTED_SOURCE' для использования другими потоками
    //afx_msg void OnConnectToServer (WPARAM , LPARAM );
    //Отслеживаем состояние директории с файлами БД
    afx_msg void OnMessageDirectoryWatched (WPARAM , LPARAM );
    //Приём сообщений от гланого потока с установленным промежутком (SEC_15)
    afx_msg void OnMessageTimer (WPARAM , LPARAM );
    //Прием запроса о необходимости оповестить адресата о параметрах соединения
    afx_msg void OnMessageQueryDataConnectToServer (WPARAM , LPARAM );

    afx_msg void OnMessageInitComboboxDirWatch (WPARAM , LPARAM );

    //HDlgGUIFilesDB *m_pDlgGUI;

protected:

	DECLARE_MESSAGE_MAP()
};
