#pragma once
#include "stdafx.h"
#include <HCheckStateKTS.h>

class HCheckStateTemplate : public HCheckStateKTS {
public:
	//HCheckStateTemplate(int , HTREEITEM , CString, CTimeSpan , CTimeSpan , CTimeSpan , CTimeSpan , int , int , int, BOOL = true);
	HCheckStateTemplate (int , HTREEITEM , HString, CTimeSpan , CTimeSpan , CTimeSpan , int , int , int , int, BOOL = true);
    ~HCheckStateTemplate(void);
    
protected:
    BOOL SetInfo (BOOL = true);
    
private:
};
