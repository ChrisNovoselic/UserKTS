#pragma once

#include <htreectrlkts.h>

#if ! defined _HTREECTRL_PSEUDO_H_
#define _HTREECTRL_PSEUDO_H_

class HTreeCtrlPseudo : public HTreeCtrlKTS {
	DECLARE_DYNCREATE (HTreeCtrlPseudo)
public:
	HTreeCtrlPseudo (void);
	~HTreeCtrlPseudo (void);

	virtual int InitializeItems (int , HTREEITEM = 0);

    virtual HString GetItemFullPath (HTREEITEM);

    virtual int AddObjectCheckStateTemplate (HTREEITEM );
    virtual void UpdateObjectCheckStateTemplate (void);
protected:

private:
	DECLARE_MESSAGE_MAP ()
};

#else
#endif //_HTREECTRL_PSEUDO_H_