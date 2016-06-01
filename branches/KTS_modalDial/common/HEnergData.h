#pragma once
#include "hdata.h"

#define COUNT_PARAMETR_VYVOD 6
#define COUNT_VYVOD 6

//Строго и ТОЛЬКо в этом виде для 5-ой версии
struct RecordChannelInfo {
	char pchName [21];
    float sKU;
    float sKI;
    float sKR;
	boolean bControl;
	BYTE Razm_5;

    RecordChannelInfo () {
        memset (pchName, 0x0, 21);
        sKI = 0; sKU = 0; sKR = 0;
        bControl = false;
        Razm_5 = -1;
    }

    RecordChannelInfo operator = (RecordChannelInfo &src) {
        memcpy (pchName, src.pchName, 21);
        sKI = src.sKI;
        sKU = src.sKU;
        sKR = src.sKR;

        bControl = src.bControl;
        Razm_5 = src.Razm_5;

        return *this;
    }
};

struct RecordChannelInfoTotal {
    RecordChannelInfo rci;

	int iOwnerNumberUSD;

    HString strName;

	long int iKI_6;
    long int iKU_6;
    //int iKR_6;

    BYTE Razm_6;

    long idVTI_SEC_15; long idVTI_MIN_30;
    int idUnitGrp;
    int timeintcoeffdef;
    int counterdivcoeff;

    RecordChannelInfoTotal (void) {
        iOwnerNumberUSD = -1;

        strName = "";
        
        iKI_6 = 0;
		iKU_6 = 0;
		//iKR_6 = 0;

        Razm_6 = 0;

		idVTI_SEC_15 = -1; idVTI_MIN_30 = -1;
        idUnitGrp = -1;
        timeintcoeffdef = -1;
        counterdivcoeff = -1;
    }

    ~RecordChannelInfoTotal (void) {
        strName.free ();
    }

    RecordChannelInfoTotal operator = (RecordChannelInfoTotal src) {
        rci = src.rci;

        strName = src.strName;

        idVTI_SEC_15 = src.idVTI_SEC_15; idVTI_MIN_30 = src.idVTI_MIN_30;
        iKI_6 = src.iKI_6;
        iKU_6 = src.iKU_6;
        Razm_6 = src.Razm_6;
        idUnitGrp = src.idUnitGrp;
        timeintcoeffdef = src.timeintcoeffdef;
        counterdivcoeff = src.counterdivcoeff;

        return *this;
    }
};

struct RecordGroupInfoTotal {
    long idVTI;
    int idUnitGrp;
    int timeintcoeffdef;

    RecordGroupInfoTotal (void) {
        idVTI = -1;
        idUnitGrp = -1;
        timeintcoeffdef = -1;
    }

    ~RecordGroupInfoTotal (void) {
    }

    RecordGroupInfoTotal operator = (RecordGroupInfoTotal src) {
        idVTI = src.idVTI;
        idUnitGrp = src.idUnitGrp;
        timeintcoeffdef = src.timeintcoeffdef;

        return *this;
    }
};

class HEnergData : public HData {
public:
	HEnergData (void);
	~HEnergData (void);
	
	//virtual double GetValueOfChannel (char * , CTime , int ) = 0;
    virtual double GetValueOfGroup (char * , CTime , int ) = 0;

    virtual int GetInfoTotalOfChannel (int = -1) = 0;
    virtual int GetInfoTotalOfGroup (int = -1) = 0;

    RecordChannelInfoTotal *GetRecordCIT (int );
    RecordGroupInfoTotal *GetRecordGIT (int );

    double GetValueOfImpulses (WORD, double, RecordChannelInfo *, LONGLONG);
    double GetValueOfImpulses (WORD, double, RecordChannelInfoTotal *, LONGLONG);
	
	enum INDEX_PARAMETR_VYVOD { CONSUMPTION_DIRECT_FIRST, CONSUMPTION_REVERSE_FIRST, TEMPERATURE_DIRECT_FIRST, TEMPERATURE_REVERSE_FIRST, PRESSURE_DIRECT_FIRST, PRESSURE_REVERSE_FIRST,
	                            CONSUMPTION_DIRECT_SECOND, CONSUMPTION_REVERSE_SECOND, TEMPERATURE_DIRECT_SECOND, TEMPERATURE_REVERSE_SECOND, PRESSURE_DIRECT_SECOND, PRESSURE_REVERSE_SECOND,
	                            CONSUMPTION_DIRECT_THIRD, CONSUMPTION_REVERSE_THIRD, TEMPERATURE_DIRECT_THIRD, TEMPERATURE_REVERSE_THIRD, PRESSURE_DIRECT_THIRD, PRESSURE_REVERSE_THIRD,
	                            CONSUMPTION_DIRECT_FOURTH, CONSUMPTION_REVERSE_FOURTH, TEMPERATURE_DIRECT_FOURTH, TEMPERATURE_REVERSE_FOURTH, PRESSURE_DIRECT_FOURTH, PRESSURE_REVERSE_FOURTH,
	                            CONSUMPTION_DIRECT_FIFTH, CONSUMPTION_REVERSE_FIFTH, TEMPERATURE_DIRECT_FIFTH, TEMPERATURE_REVERSE_FIFTH, PRESSURE_DIRECT_FIFTH, PRESSURE_REVERSE_FIFTH,
	                            CONSUMPTION_DIRECT_PERVMAY, CONSUMPTION_REVERSE_PERVMAY, TEMPERATURE_DIRECT_PERVMAY, TEMPERATURE_REVERSE_PERVMAY, PRESSURE_DIRECT_PERVMAY, PRESSURE_REVERSE_PERVMAY,
	                            CONSUMPTION_DIRECT_FEEDING, CONSUMPTION_REVERSE_FEEDING, TEMPERATURE_WATER, LAST_INDEX};
	
#ifdef PAR_ENERG_VER_6
    int m_idClassVal; //Признак для отличия ELEKTRO(25) & TEPLO(15) v.6.x
    void SetClassVal (int id) { m_idClassVal = id; }
#endif

    int SetValueParametrVyvod (CTime , int );
	double GetTeploVyvodWithPodpitka ();
	double GetTeploVyvod ();

protected:
    //RecordChannelInfoTotal *m_ptrRecCIT;
    //RecordGroupInfoTotal *m_ptrRecGIT;
    std::map <int, RecordChannelInfoTotal *> m_mapCIT;
    std::map <int, RecordGroupInfoTotal *> m_mapGIT;

typedef CMap <TCHAR *, TCHAR *, int, int &> HMapStringAToInt;
typedef CMap <TCHAR *, TCHAR *, HMapStringAToInt *, HMapStringAToInt *> HMapStringAToPtr;
    CMap <TCHAR *, TCHAR *, HMapStringAToPtr *, HMapStringAToPtr *> m_mapParametrVyvod;
	
    int m_arEnergyUserNumberParametrVyvod [LAST_INDEX];
    double m_arEnergyDataParametrVyvod [LAST_INDEX];
	static int m_arVzletGrafaParametrVyvod [LAST_INDEX];
    double m_arVzletDataParametrVyvod [LAST_INDEX];
    
private:
    static double Entalpia (double , double );

    CRITICAL_SECTION m_CriticalSection;
};

ENUM_INCREMENT (HEnergData::INDEX_PARAMETR_VYVOD)
