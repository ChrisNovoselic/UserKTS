#pragma once
#include "htornadodata.h"

class HTornado6Data : public HTornadoData {
public:
    HTornado6Data (void);
    HTornado6Data (CStringA);
    ~HTornado6Data (void);
    
private:
    virtual void InitAccumulationId (void) {
        m_arAccumulationId.push_back ("_G��");
        m_arAccumulationId.push_back ("_D��");
        m_arAccumulationId.push_back ("_G��");
        m_arAccumulationId.push_back ("_Do�");
        m_arAccumulationId.push_back ("_Do�");
        m_arAccumulationId.push_back ("_F����_��_");//CFG.dbo.Measure
    }
};
