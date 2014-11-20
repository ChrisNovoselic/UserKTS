#pragma once
#include "stdafx.h"
#include "HCheckStateKTS.h"

class HCheckStateTemplate : public HCheckStateKTS {
    DECLARE_DYNAMIC (HCheckStateTemplate)

public:
	HCheckStateTemplate(int , HTREEITEM , HString, CTimeSpan , CTimeSpan , CTimeSpan , int , int , int , int, BOOL = true);
    ~HCheckStateTemplate (void);

    virtual void SetTarget (HInfoTemplate *, CTime, BOOL);

protected:
    BOOL SetInfo (BOOL = true);
    
private:
};
