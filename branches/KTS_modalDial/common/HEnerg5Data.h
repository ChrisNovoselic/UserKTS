#pragma once
#include "HEnergData.h"

class HEnerg5Data :	public HEnergData {
private:
	//Структура для вызова 'ReadFile'
    OVERLAPPED m_OverLapped;
	//Структура для вызова 'CreateFile'
	SECURITY_ATTRIBUTES m_sa;

	int OpenFileData (CStringA &);

protected:
	CTime m_tmLastWriteDB;
	HANDLE m_hFileOfRead, m_hTemplateFile;

    virtual void InitAccumulationId (void) {
        int i = 0;
        m_arAccumulationId.push_back ("39");
    }

public:
	HEnerg5Data ();
	HEnerg5Data (CStringA);
	~HEnerg5Data (void);

	virtual int SetConnectDB (CStringA);
	virtual void DisconnectDB (void) {}
	int SetDateDB (void);

	virtual double GetValueOfGroup (char * , CTime , int );
	virtual double GetValueOfChannel (char * , CTime , int );

    virtual int GetInfoTotalOfChannel (int = -1);
    virtual int GetInfoTotalOfGroup (int = -1);

    BYTE GetAdresData (INDEX_TIME);
    WORD GetImpulses (char * , INDEX_TIME);
    DWORD GetImpulses (char * , CTime , int );
};
