#include "stdafx.h"

#include "hthreadcreatetemplate.h"

EXT_DECLARE_HTHREAD_FILELOG

HThreadCreateTemplate::HThreadCreateTemplate () : HWinThread () {
}

HThreadCreateTemplate::~HThreadCreateTemplate () {
}

BEGIN_MESSAGE_MAP (HThreadCreateTemplate, HWinThread)
    ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &OnResumeRun)
END_MESSAGE_MAP ()

int HThreadCreateTemplate::InitInstance (void) {
	int iRes = -1;
	//������ ����� ������ �������� ������
	iRes = HWinThread::InitInstance ();

    FILELOG_VARIABLE

    if (iRes == 1) {
	}
	else ; //������������� - ������

	return iRes;
}

int HThreadCreateTemplate::ExitInstance (void) {
	//������� ����� ������ �������� ������
	return HWinThread::ExitInstance ();
}

afx_msg void HThreadCreateTemplate::OnResumeRun (WPARAM, LPARAM ) {
}