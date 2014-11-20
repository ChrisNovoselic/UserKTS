#include "stdafx.h"
#include "HTornado6Data.h"

EXT_DECLARE_HTHREAD_FILELOG

HTornado6Data::HTornado6Data (void) : HTornadoData ()
{
}

HTornado6Data::HTornado6Data (CStringA con) : HTornadoData (con) {
    FILELOG_VARIABLE

    InitAccumulationId ();
}

HTornado6Data::~HTornado6Data(void) {
#if (defined VARIANT_TASK_2) || (defined VARIANT_TASK_1)
    mapSignalAvg.RemoveAll ();
#else
#if (defined VARIANT_TASK_4) || (defined VARIANT_TASK_3)
    mapSignalAvg.clear ();
#else
#endif
#endif
}
