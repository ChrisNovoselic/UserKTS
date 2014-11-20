#pragma once
#include "htornadodata.h"

class HTornado1Data : public HTornadoData {
public:
    HTornado1Data (void);
    HTornado1Data (CStringA);
    ~HTornado1Data (void);

    virtual double GetValueOfChannel (char *, CTime , int );

protected:
    virtual void InitAccumulationId (void) {
        m_arAccumulationId.push_back ("N4");
        m_arAccumulationId.push_back ("N470B02");
        m_arAccumulationId.push_back ("N471B02");
        m_arAccumulationId.push_back ("N474B02");
        m_arAccumulationId.push_back ("N502B02");
        m_arAccumulationId.push_back ("UM13G001");
        m_arAccumulationId.push_back ("R010B02");
        m_arAccumulationId.push_back ("R110B01");
        m_arAccumulationId.push_back ("R110B02");
    }
private:
};
