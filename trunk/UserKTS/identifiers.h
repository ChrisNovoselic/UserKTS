#pragma once
#if ! defined _USERKTS_IDENTIFIERS_H_
#define _USERKTS_IDENTIFIERS_H_

#define TABLENAME_TEMPLATE                  "Template_User"
//#define TABLENAME_TIME                      "Time"
//#define TABLENAME_OPTION                    "Option_User"
//#define TABLENAME_OPERATION                 "Operation"

#define IDM_RESTORE							WM_USER + 234

#define IDC_STATIC_FULLPATH					1022

#define WM_USER_MESSAGE						WM_USER + 667
#define WM_USER_ID_TIMER_UPDATE				WM_USER + 669
#define WM_USER_EXIT_CREATE_TEMPLATE		WM_USER + 671
#define WM_USER_PROGRESS_RULES				WM_USER + 672
#define WM_USER_ENDWORKBD_CREATE_TEMPLATE	WM_USER + 673
#define WM_USER_EXIT_UNDO_CLEAR				WM_USER + 675
#define WM_USER_LIVE_CREATE_TEMPLATE		WM_USER + 684
#define WM_USER_THREADUNDOCLEAR_RUN         WM_USER + 685
#define WM_USER_MAKEPROGRESSCTRL            WM_USER + 686

enum TYPE_FILELOG_MESSAGES {NOT_MESSAGES,
                    HCOMMON,
                    HDEBUG,
                    HDATA_COMMON,
                    HDATA_ENERG5,
                    HDATA_ENERG6,
                    HDATA_TORNADO,
                    HDATA_TORNADO1,
                    HDATA_TORNADO6,
                    HFIND_DB,
                    HDATABASE,
                    HRECORDSET,
                    THREAD_WATCH_DIRECTORY,
                    HANDLER_WATCH_DIRECTORY,
                    THREAD_CREATE_TEMPLATE,
                    THREAD_UNDO_CLEAR,
                    OBJECT_TEMPLATE,
                    CHECK_STATE_TEMPLATE,
                    CHECK_STATE_TEMPLATE_ISUPDATE,
                    CONTENT_TEMPLATE,
                    TREE_CTRL_KTS,
					TREE_CTRL_TEMPLATE,
                    COMMON_KTS_APP,
                    USER_KTS_APP,
                    COMMON_KTS_DLG,
                    USER_KTS_DLG,
                    HERROR,
                    ALL_MESSAGES};

#else
#endif