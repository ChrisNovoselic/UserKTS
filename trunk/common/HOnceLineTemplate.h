#pragma once
#include "stdafx.h"

class HOnceLineTemplate : public CObject
{
public:
    HOnceLineTemplate ();
    HOnceLineTemplate (HString &);
    
	~HOnceLineTemplate ();
	
	void CopyTo (HOnceLineTemplate *);
	void CopyFrom (HOnceLineTemplate *);
    
    char *GetBufferTemplate (void) { return MB_HSTRING (m_strADescriptionTemplate); }
    char *GetBufferTarget (void) { return MB_HSTRING (m_strADescriptionTarget); }
    
    int AddParametrsDT (void); //Добавление во 2-ую строку параметров 'tm_beg', 'dt_beg', 'tm_end', 'dt_end' - МОЙ ФОРМАТ 2-ой строки!!!
    int AddParametr (int , HString , HString = "", int = 0);
    //Возврат СТРОКИ-ЦИФРы ЗАЧЕМ ??? - УБЕЖДАЕМСЯ, что ТАКой параметр ЕСТЬ и ВОЗВРАЩАЕМ его ТИП
    HString GetParametrType (int = 0);
    HString GetParametrTypeOfIdentifier (int );
    HString FindParametrValue (int );
    int FindIdentifierParametrType (int );
    
    //Формирование типа и значений параметров из БД (аналог 'AddParametr'...)
#ifdef PTR_DB
    int SetParametrsType (HString &);
    int SetParametrsValue (HString &);
#endif
    
    HString GetParametrTypeDetail (int , int);
    HString GetParametrValueDetail (int , int);

	void NewTarget (void);
	
	int SetSizeArrayParametrData ();
    
	double GetParametrData (int);
    double GetParametrDataDetail (int , int);
    
	int ApplyParametrDataDetail (int , int , double);
    
    int ApplyParametrData (int);  //Вызов одной из слндующих функций - значение параметра внутри КЛАССа
	int ApplyParametrData (int , VARIANT);
    int ApplyParametrData (int , HString &);
	//int ApplyParametrData (int , CString);
    int ApplyParametrData (int , double &);
    int ApplyParametrData (int , int &);
    
    BOOL IsPosition (LPSTR lpstr)
    {
        HString strAPosition (lpstr);
        strAPosition.trim ();
        if (strAPosition.compareNoCase (m_strAPosition) == 0)
            return true;
            
        return false;
    }
    BOOL IsPlace (LPSTR lpstr)
    {
        HString strAPlace (lpstr); 
        strAPlace.trim ();
        if (strAPlace.compareNoCase (m_strAPlace) == 0)
            return true;
            
        return false;
    }

    HString strAPosition ();
    HString strAPlace ();
    
protected:
    HString m_strAPosition;
    HString m_strAPlace;
    HString m_strADescriptionTemplate;
    HString m_strADescriptionTarget;
    double m_dblCoeffMeasure;
    CArray <HString> m_arstrAParametrType;
    CArray <HString> m_arstrAParametrValue;
	
	 //Каждому номеру канал/группа должны соответствовать суммарные данные за период
	CArray <double *> m_ardblParametrData;
	//Итоговы данные ???
	//double m_dblParametrsTotalData;
private:
    void SetCoeffMeasure ();
    double GetCoeffMeasure ();
    
    int GetParametrPosLength (int , int &, int &);
    static int GetParametrLength (int iPos, HString &strA)
	{
	    int iLen = 0;
	    
	    if ((iPos + iLen) < strA.length ())
	        while ((! (MB_AT (strA, iPos + iLen) == 0)) && (! (MB_AT (strA, iPos + iLen) == 0x20)))
                iLen ++;
        else
            iLen = -1;
        
        return iLen;
	}
};
