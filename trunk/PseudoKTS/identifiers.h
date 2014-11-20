#pragma once
#if ! defined _PSEUDOKTS_IDENTIFIERS_H_
#define _PSEUDOKTS_IDENTIFIERS_H_

#define COUNT_PASSED_SEC_15                 1 //Число проходов при опросе 'getData.exe'

#define TABLENAME_TEMPLATE                  "Template_Pseudo"
//#define TABLENAME_TIME                      "Time"
//#define TABLENAME_OPTION                    "Option_Pseudo"
//#define TABLENAME_OPERATION                 "Operation"

#define WM_USER_THREAD_GUI_CLOSE			WM_APP + 6
#define WM_USER_THREAD_TIMER                WM_APP + 7
#define WM_USER_TIMER                       WM_APP + 8
#define WM_USER_CONNECT_TO_SERVER           WM_APP + 9
#define WM_USER_QUERY_CONNECT_TO_SERVER     WM_APP + 10
#define WM_USER_READY_DATA					WM_APP + 11
#define WM_USER_NUMBER_PTIC_CONTROL         WM_APP + 13

//#define WM_USER_INIT_ID_THREAD              WM_APP + 14
#define WM_USER_SEC_15_THREADGETDATA        WM_APP + 15
#define WM_USER_ENG_PSEUDO_THREADGETDATA   WM_APP + 16
#define WM_USER_ENG_ORIGIN_THREADGETDATA   WM_APP + 17
#define WM_USER_WRITEDATA                   WM_APP + 18

#define WM_USER_QUERY_NUMBER_PTIC_CONTROL WM_APP + 19

#define WM_USER_SOURCEDB_SELECHANGED        WM_APP + 21

#define WM_USER_INIT_COMBOBOX_DIRWATCH      WM_APP + 22

#define IDC_COMBO_DIRWATCH                  1101
#define IDC_COMBO_SOURCEDB                  1102
#define IDC_TREECTRL_CONTROL                1103

#define IDC_LIST_ITEM_IMPULSES              1104
#define IDC_LIST_ITEM_VALUES              1105

enum TYPE_FILELOG_MESSAGES {NOT_MESSAGES,
                    HCOMMON,
                    HDEBUG,
                    HDATA_COMMON,
                    HDATA_ENERG5,
                    HDATA_ENERG6,
                    HFIND_DB,
                    HDATABASE,
                    HRECORDSET,
                    THREAD_WATCH_DIRECTORY,
                    HANDLER_WATCH_DIRECTORY,
                    THREAD_CREATE_TEMPLATE,
                    THREAD_GETDATA,
                    OBJECT_TEMPLATE,
                    CHECK_STATE_TEMPLATE,
                    CHECK_STATE_TEMPLATE_ISUPDATE,
                    CONTENT_TEMPLATE,
                    COMMON_KTS_APP,
                    COMMON_KTS_DLG,
                    PSEUDO_KTS_DLG,
					TREE_CTRL_KTS,
					TREE_CTRL_PSEUDO,
                    HERROR,
                    ALL_MESSAGES};

#else
#endif