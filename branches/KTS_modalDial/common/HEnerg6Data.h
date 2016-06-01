#pragma once
#include "henergdata.h"

class HEnerg6Data : public HEnergData {
public:
    HEnerg6Data (void);
    HEnerg6Data (CStringA);
    ~HEnerg6Data (void);

	virtual int SetConnectDB (CStringA);
	virtual void DisconnectDB (void);

    virtual double GetValueOfChannel (char * , CTime , int );
    virtual double GetValueOfGroup (char * , CTime , int );

    virtual int GetInfoTotalOfChannel (int = -1);
    virtual int GetInfoTotalOfGroup (int = -1);

protected:
    long m_lIdVTIList;

    virtual void InitAccumulationId (void) {
        m_arAccumulationId.push_back ("5");
        m_arAccumulationId.push_back ("6");
        m_arAccumulationId.push_back ("9");
        m_arAccumulationId.push_back ("33");
        m_arAccumulationId.push_back ("37");
        m_arAccumulationId.push_back ("38");
        m_arAccumulationId.push_back ("41");
        m_arAccumulationId.push_back ("46");
    }

private:
    //Полная информация о ВСЕХ 1-512 каналАХ
    int GetInfoOfChannels (int *, RecordChannelInfoTotal *);
    //int GetInfoOfGroups (int *);

    double GetValue (CTime , int , int, int, int); //Период значения, Период по каким значениям расчитываем, ID ВТИ, ID типа параметра
};
