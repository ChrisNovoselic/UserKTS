#pragma once
#include "stdafx.h"
#include "hdata.h"

class HTornadoData : public HData
{
public:
    HTornadoData (void);
    HTornadoData (CStringA);
    ~HTornadoData (void);

	virtual int SetConnectDB (CStringA);
	virtual void DisconnectDB (void) {}
    
    int PrepareGetValue (CTime , int );
    int ClearGetValue (void);
        
    //double GetValueOfChannel (CTime , int , int ) { return 0; }
    virtual double GetValueOfChannel (char *, CTime , int );
};
