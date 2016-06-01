#pragma once

#include <hwinapp.h>

#include <hdbsettings.h>

// HWinApp

class HWinAppKTS : public HWinApp {
//	DECLARE_DYNCREATE(HWinAppKTS)

protected:
	HWinAppKTS ();           // защищенный конструктор, используемый при динамическом создании
	virtual ~HWinAppKTS();

    int InitializeIntervals (void);
    int SetParametrLogFile (void);

public:
	virtual BOOL InitInstance ();
	virtual int ExitInstance ();

    enum ID_MODE_APP {MODE_VIEW = 1, MODE_ADMIN = 2, MODE_REMOTE = 4, MODE_LOCAL = 8, MODE_DISABLED_AUTOCREATE = 16};
	enum ID_HANDLE_EVENT {DIRECTORY_WATCHED = 0, /*GETDATA_05_MIN, GETDATA_30_MIN,*/WHOIS, COUNT_HANDLE_EVENT};
    int SetModeApp (void);
    inline int GetModeApp (void) { return m_iModeApp; }
    inline long GetUpdateSeconds (void) { return m_tmSpanUpdate.GetTotalSeconds (); }
    inline long GetWorkSeconds (void) { return m_tmSpanWorkApp.GetTotalSeconds (); }

    virtual int SetOption () = 0;

	HDatabase *getPtrDB () { return m_pDB; }
	HDatabase *GetPtrDB () {
	    if (m_pDB) {
	        if (m_pDB->IsOpen ())
	            return m_pDB;
            else {
                if ((m_strFullPathDB.isUses () == 1) && (SetConnectDB () == 0))
                    return m_pDB;
                else
                    return NULL;
            }
        }
        else
            //if (SetConnectDB () == 0)
            //    return m_pDB;
            //else
                return NULL;
            //;
    }

    //int SetConnectDB (HString &, HDatabase::DRIVER_DB);
    int SetConnectDB (void);
	void DisconnectDB (void);

    HDBSettings *m_ptrDBSettings;
	HANDLE m_arHandleEvent [COUNT_HANDLE_EVENT];
	//HANDLE m_hEvtDirectoryWatched;

    CMenu *m_ptrMenuTray;
    int ManagerIconTray (HWND, int , HICON = 0x0);
    HWND m_hWndRules;

// Implementation

protected:
    CTimeSpan m_tmSpanUpdate;
    CTimeSpan m_tmSpanWorkApp;

    DECLARE_MESSAGE_MAP()

private:
    int m_iModeApp;

    HString m_strFullPathDB;
    HDatabase *m_pDB;
    
    int InitializeDB (void);
};

//ENUM_PLUS (HWinAppKTS::ID_HANDLE_EVENT)
ENUM_INCREMENT (HWinAppKTS::ID_HANDLE_EVENT)

#define APP_HANDLE_EVENT(id) ((HWinAppKTS *) AfxGetApp ())->m_arHandleEvent [id]

#define MODE_APP ((HWinAppKTS *) AfxGetApp ())->GetModeApp ()
#define PTR_SETTINGS ((HWinAppKTS *) AfxGetApp ())->m_ptrDBSettings
#define UPDATE_SECONDS ((HWinAppKTS *) AfxGetApp ())->GetUpdateSeconds ()
#define WORK_SECONDS ((HWinAppKTS *) AfxGetApp ())->GetWorkSeconds ()