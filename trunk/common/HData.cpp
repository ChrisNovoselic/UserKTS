#include "stdafx.h"

#include "HData.h"

EXT_DECLARE_HTHREAD_FILELOG

#ifdef _DEBUG
extern CMemoryState g_memStateDiff,
                    g_memStateBegin,
                    g_memStateEnd;
#endif

#if defined(PAR_ENERG_VER_6) || defined(PAR_TORNADO)
extern BOOL FreeMemoryDBObject (CObject *&);
#endif

HData::HData (void) : CObject (), m_pRS (0x0) {
    m_pDBData = DEBUG_NEW HDatabaseSQL ();

#ifdef _DEBUG
    //TRACE ("--------------------HData::HData ()--------------------\n");
    //g_memStateBegin.DumpAllObjectsSince ();
#else
#endif
}

HData::~HData (void) {
    for (vector <string>::iterator i = m_arAccumulationId.begin (); ! (i == m_arAccumulationId.end ()); i ++)
        (*i).clear ();
    m_arAccumulationId.clear ();

    DisconnectDB ();
}

void HData::DisconnectDB (void) {
    FreeMemoryDBObject ((CObject *&) m_pRS);
    FreeMemoryDBObject ((CObject *&) m_pDBData);
}

/*static int HData::IsAccumulation (char **arAccumulationMeasures, char *id) {
	int iRes = FALSE;
    for (int i = 0; ! (arAccumulationMeasures [i] == 0x0); i ++)
        if (strcmp (id, arAccumulationMeasures [i]) == 0) {
            iRes = TRUE;
            break;
        }
        else
            ;
        
    return iRes;
}*/

int HData::IsAccumulationMeasure (char *id) {
    int iRes = FALSE;
    for (vector <string>::iterator i = m_arAccumulationId.begin (); ! (i == m_arAccumulationId.end ()); i ++)
        if (strcmp (id, (*i).c_str ()) == 0) {
            iRes = TRUE;
            break;
        }
        else
            ;
        
    return iRes;
}

int HData::IsAccumulationMeasure (int id) {
    int iRes = FALSE;
    size_t szId = 19;
    char *pchId = DEBUG_NEW char [szId];
    memset (pchId, 0x0, szId * sizeof (char));
    itoa (id, pchId, 10);

    iRes = IsAccumulationMeasure (pchId);

    delete pchId;

    return iRes;
}

/*static int HData::IsAccumulation (int *arAccumulationMeasures, int id) {
	for (int i = 0; arAccumulationMeasures [i] > 0; i ++)
        if (id == arAccumulationMeasures [i])
            return true;
        
    return false;
}*/