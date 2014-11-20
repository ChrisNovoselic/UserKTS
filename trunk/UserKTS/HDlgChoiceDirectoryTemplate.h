#pragma once
#include <HDlgChoiceDirectory.h>

class HDlgChoiceDirectoryTemplate : public HDlgChoiceDirectory
{
	DECLARE_DYNAMIC (HDlgChoiceDirectoryTemplate)

public:
	HDlgChoiceDirectoryTemplate (CWnd* pParent = NULL);   // standard constructor
	virtual ~HDlgChoiceDirectoryTemplate ();

protected:
    virtual BOOL OnInitDialog (void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:

public:
	afx_msg void OnBnClickedOk();
private:

public:
};

