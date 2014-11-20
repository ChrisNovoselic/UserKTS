#pragma once
#include "htornadodata.h"

class HTornado6Data : public HTornadoData {
public:
    HTornado6Data (void);
    HTornado6Data (CStringA);
    ~HTornado6Data (void);
    
private:
    virtual void InitAccumulationId (void) {
        m_arAccumulationId.push_back ("_Gïâ");
        m_arAccumulationId.push_back ("_Dïð");
        m_arAccumulationId.push_back ("_Gñâ");
        m_arAccumulationId.push_back ("_DoÀ");
        m_arAccumulationId.push_back ("_DoÁ");
        m_arAccumulationId.push_back ("_Fïàðà_çà_");//CFG.dbo.Measure
    }
};
