#include "stdafx.h"
#include "HDlgChoiceDirectoryTemplate.h"

// HDlgChoiceDirectory dialog

IMPLEMENT_DYNAMIC(HDlgChoiceDirectoryTemplate, HDlgChoiceDirectory)

HDlgChoiceDirectoryTemplate::HDlgChoiceDirectoryTemplate (CWnd* pParent /*=NULL*/) : HDlgChoiceDirectory (pParent) {
}

//extern BOOL CALLBACK deleteChildWindows (HWND , LPARAM );

HDlgChoiceDirectoryTemplate::~HDlgChoiceDirectoryTemplate (void) {
}

void HDlgChoiceDirectoryTemplate::DoDataExchange (CDataExchange* pDX) {
    HDlgChoiceDirectory::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(HDlgChoiceDirectoryTemplate, CDialog)
	ON_BN_CLICKED(IDOK, &HDlgChoiceDirectoryTemplate::OnBnClickedOk)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREECTRL_DIRECTORY, &HDlgChoiceDirectory::OnTvnItemexpandingTreeDirectory)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREECTRL_DIRECTORY, &HDlgChoiceDirectory::OnTvnSelchangedTreeDirectory)
END_MESSAGE_MAP()

// HDlgChoiceDirectoryTemplate message handlers

BOOL HDlgChoiceDirectoryTemplate::OnInitDialog () {
	HDlgChoiceDirectory::OnInitDialog ();
    
    m_tvInsert.hParent = NULL;
	m_tvInsert.hInsertAfter = NULL;
	m_tvInsert.item.cchTextMax = MAX_PATH + 1;
	m_tvInsert.item.mask = TVIF_TEXT;
	m_tvInsert.hInsertAfter = TVI_FIRST;
	m_tvInsert.item.pszText = _T("ќчистить значение...");

	m_hClear = m_ptrTreeCtrlDirectory->InsertItem (&m_tvInsert);

	ExpandCurrent (m_hRoot, m_strPath);

	CDialog::OnInitDialog ();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void HDlgChoiceDirectoryTemplate::OnBnClickedOk () {
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_ptrTreeCtrlDirectory->GetSelectedItem (),
			//hRootItem = m_ptrTreeCtrlDirectory->GetRootItem ();
			hRootItem = m_hRoot;
	
	m_strPath.free ();
			
    if (hItem == m_hClear)
	    m_strPath = CString (_T("666"));
	else
	{
	    while ((hItem != hRootItem) && (hItem != NULL))
	    {
		    m_strPath.insert (0, _T ("\\"));
		    m_strPath.insert (0, m_ptrTreeCtrlDirectory->GetItemText (hItem).GetBuffer ());
		    
		    hItem = m_ptrTreeCtrlDirectory->GetParentItem (hItem);
	    }
    	
	    //ќбрежем  –ј…Ќюю '\\'
	    m_strPath = m_strPath.left (m_strPath.length () - 1);
	}

	HDlgChoiceDirectory::OnOK ();
}