#include "stdafx.h"
#include "SDDControlApp.h"

BEGIN_MESSAGE_MAP(CSDDControlApp, HWinApp)
END_MESSAGE_MAP()

// The one and only CUserKTSApp object
CSDDControlApp theApp;

CSDDControlApp::CSDDControlApp(void) {
}

CSDDControlApp::~CSDDControlApp(void) {
}

BOOL CSDDControlApp::InitInstance () {
    BOOL bRes = HWinApp::InitInstance ();

    return false;
}

int CSDDControlApp::ExitInstance () {
    return HWinApp::ExitInstance ();
}
