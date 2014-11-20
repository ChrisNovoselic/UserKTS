#pragma once
#if ! defined _COMMONKTS_IDENTIFIERS_H_
#define _COMMONKTS_IDENTIFIERS_H_

typedef afx_msg void (PTR_HANDLER_ONTHREADMESSAGE) (WPARAM, LPARAM );

#define TABLENAME_TIME                      "Time"
#define TABLENAME_OPTION                    "Option"
#define TABLENAME_OPERATION                 "Operation"

#define IDP_OLE_INIT_FAILED     "�� ������� ��������� ������������� OLE."
#define IDP_SOCKETS_INIT_FAILED "�� ������� ���������������� Windows Sockets."

#define IDM_OPERATION_UPDATE            32793
#define IDM_TEMPLATE_ADD                32794
#define IDM_TEMPLATE_CHANGE             32795
#define IDM_TEMPLATE_DEL                32796
#define IDM_HELP_ABOUT					32797 //16
#define IDM_HELP_CONTEXT                32798

#define IDM_RESTORE							WM_USER + 234
#define IDM_RESTORE_FILES_DB			    WM_USER + 235
#define IDM_RESTORE_CONTROL			        WM_USER + 236
#define IDM_EXIT							WM_USER + 237
//#define IDD_ABOUTBOX						100
#define IDM_ABOUTBOX						0x0010 //16 = 0x0010

#define IDC_PROGRESS_CTRL					WM_USER + 1170
#define HEIGHT_PROGRESSCTRL					15
#define OFFSET_CONTROL						6
#define HEIGHT_CONTROL						22

#define PREFIX_IDM_OPERATION				WM_USER + 1070
enum ID_OPERATION {ID_OPERATION_UPDATEDATA = PREFIX_IDM_OPERATION + 1, ID_OPERATION_DELETE, ID_OPERATION_CREATE, ID_OPERATION_CREATE_ALTERNATIVE, ID_OPERATION_INCREMENTEXT, ID_OPERATION_DECREMENTEXT, ID_OPERATION_COPYPREV, ID_OPERATION_COPYNEXT, ID_OPERATION_CHANGE_DATETIME, ID_LAST_OPERATION};
#define COUNT_OPERATION ID_LAST_OPERATION - PREFIX_IDM_OPERATION

#define IDC_TREECTRL_TEMPLATE				64798
#define IDC_COMMAND_DIR_LIST				1010
//#define IDC_COMMAND_DIR_LIST				WM_USER + 167
#define IDC_COMMAND_OPERATION				1011
#define IDC_COMBO_OPERATION					1025
#define IDC_LIST_ITEM_FILES					1018
#define IDC_CHECK_AUTO_UPDATE				1026

#define WM_USER_THREAD_RESUME				WM_USER + 1270
#define WM_USER_MESSAGE_DIR_WATCH			WM_USER + 1271
#define WM_USER_ENTER_DIRECTORY_WATCHED		WM_USER + 1272
#define WM_USER_LEAVE_DIRECTORY_WATCHED		WM_USER + 1273
#define WM_CHANGE_STAFF_OPERATION			WM_USER + 1275
#define WM_USER_MESSAGE_ICON_TRAY			WM_USER + 567
#define WM_USER_TOSYSTEMTRAY				WM_USER + 568
#define WM_USER_AUTO_CREATE                 WM_APP + 1010
#define WM_USER_WHOIS                       WM_APP + 3010

#define WM_USER_THREAD_LIVE                 WM_APP + 4001
#define WM_USER_QUERY_THREAD_ID             WM_APP + 4002
#define WM_USER_INIT_ID_THREAD              WM_APP + 4003
#define WM_USER_RESET_THREAD_RESPONCE       WM_APP + 4004

#define STATE_ITEM_NODATA           WM_USER + 867
#define STATE_ITEM_FAILED           WM_USER + 868
#define STATE_ITEM_OK               WM_USER + 869

//�������������� ������ ��� ���������� ������ � ����. 'Template'
#define ERR_BAD_PATH						WM_USER + 767
#define ERR_BAD_EXT							WM_USER + 768
#define ERR_DOUBLE_REC_TEMPLATE				WM_USER + 769
#define ERR_CREATE_TABLE					WM_USER + 770
#define ERR_ADD_REC_LISTTABLE				WM_USER + 771
#define ERR_ADD_REC_TEMPLATE				WM_USER + 772
#define ERR_REC_NOT_FOUND					WM_USER + 773
#define ERR_EDIT_REC_TEMPLATE				WM_USER + 774
#define ERR_DEL_TABLE						WM_USER + 775
#define ERR_SET_CONTENT_TEMPLATE			WM_USER + 776
//#define ERR_CONNECT_DB						WM_USER + 777
#define ERR_CONNECT_RS						WM_USER + 778
#define ERR_QUERY_DB						WM_USER + 779
#define ERR_UPDATE_REC_IS_TEMPLATE			WM_USER + 780
#define ERR_DELETE_TABLE					WM_USER + 781
#define ERR_CREATE_OBJECT_TEMPLATE			WM_USER + 782

//#define ERR_BAD_LOGFILE						WM_USER + 783
#define ERR_ARGS							WM_USER + 784
#define ERR_PARSE_NULL						WM_USER + 785
#define ERR_NULL_VALUE						0x0400 + 786
#define ERR_BREAK							WM_USER + 787
#define ERR_CALC_NULL						WM_USER + 788

#else
#endif