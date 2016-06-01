#pragma once
#include <hdatabasesql.h>

using namespace std;

class HData : public CObject {
private:
    
protected:
    vector <string> m_arAccumulationId;
    virtual void InitAccumulationId (void) = 0;

    //int IsAccumulation (char *id);
    
#if defined _USE_SQL
	HDatabaseSQL *m_pDBData;
    CRecordset *m_pRS;
#endif

public:
	HData ();
	~HData (void);

    HDatabaseSQL *GetPtrDB (void) { return m_pDBData; }

	virtual double GetValueOfChannel (char *, CTime , int ) = 0;
	virtual int SetConnectDB (CStringA) = 0;
	virtual void DisconnectDB (void);
    
    int IsAccumulationMeasure (int );
    int IsAccumulationMeasure (char *);
};

#define STR_CONNECTDB m_pDBData->m_strAConnectDB
#define PTR_DB_DATA(obj) obj->GetPtrDB ()
