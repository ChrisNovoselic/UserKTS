#pragma once
#include "StdAfx.h"

#include "HDateTimeInterval.h"
EXT_DECLARE_HDATETIMEINTERVAL

// HObjectTemplate command target

extern BOOL IsNumber(BYTE );
extern CTime resetDigitUnitsMinute (CTime );
extern CTime resetDigitTensSecond (CTime );

struct HInfoTemplate {
	int iExt;
	CTime tmLastWrite;
	//CTime tmLastAccess;
	//CTime tmCreation;

	HInfoTemplate (void) {
		iExt = -1;
		tmLastWrite = 0;
	}
};

struct HInfoSample : public HInfoTemplate {
	int iState;
	//BOOL bIsSelected;

	HInfoSample (void) : HInfoTemplate () {
		iState = STATE_ITEM_NODATA;
	}
};

class HObjectTemplate : public CObject {
    DECLARE_DYNCREATE (HObjectTemplate)
public:
	HObjectTemplate ();
	//HObjectTemplate (int , HTREEITEM , CString, CTimeSpan , CTimeSpan , CTimeSpan , CTimeSpan , int , int , BOOL = true);
	HObjectTemplate (int , HTREEITEM , HString, CTimeSpan , CTimeSpan , CTimeSpan , int , int , int , BOOL = true);
	virtual ~HObjectTemplate(void);

    //Формирование имени таблицы по 'ID'
	static HString HObjectTemplate::GetNameTableTemplate (int id) {
        HString strRes;
        
        strRes.format ("%i", id);
        while (strRes.length () < 4)
            strRes.insert (0, "0");
        strRes.insert (0, _T("TEMPLATE_"));
        
        return strRes;
    }
    
    int GetID(void) { return m_iIdTemplate; }
	HTREEITEM GetHTreeItem (void) { return m_hTreeItem; }
	int GetIdInterval (void) { return m_iIdInterval; }
	CTimeSpan GetIntervalDeclarative (void) { return CTimeSpan (PTR_DATETIMEINTERVAL->GetInterval (m_iIdInterval)); } //Возвращение ИНТЕРВАЛа СОЗДАНия ШАБЛОНа (в секундах)
	CTimeSpan GetIntervalVariable (CTime , short );
	//CTimeSpan GetInterval (CTime tm) { return PTR_DATETIMEINTERVAL->GetInterval (m_iIdInterval, tm); } //Возвращение ИНТЕРВАЛа модификации ШАБЛОНа (в секундах)
	CTimeSpan GetSpanTask (void) { return m_tmSpanTask; } //Возвращение ИНТЕРВАЛа запуска задачи создания ШАБЛОНа (в секундах)
	CTimeSpan GetSpanWork (void) { return m_tmSpanWork; } //Возвращение ИНТЕРВАЛа от момента запуска задачи создания ШАБЛОНа ДО его создания (в секундах)
	CTimeSpan GetSpanModify (void); //Нахождение ближайшего к ПОЛУНОЧи времени модификации ШАБЛОНа (в секундах)
	HString GetFullPathTemplate (void) { return m_strWildcard; } //Возвращение 
	
	int GetMinExt (void) { return m_iMinNumberExt; }
	int GetMaxExt (void) { return m_iMaxNumberExt; }
	
	SYSTEMTIME stmLastWritten (void); //Дата/время модификации самого МОЛОДОГо файла
		
	int GetCountFileFind (void);
	
    //Проверка: является ли путь ШАБЛОНом
	static BOOL IsTemplate (HString &strFullPathTemplate) {
        BOOL bRes = false;
        
        if (! PathIsDirectory (WC_HSTRING (strFullPathTemplate))) {
            if (strFullPathTemplate.length () > 1)
                if ((strFullPathTemplate.GetAt (strFullPathTemplate.length () - 1) == '*') ||
                    (IsNumber (strFullPathTemplate.GetAt (strFullPathTemplate.length () - 1))) &&
                    (IsNumber (strFullPathTemplate.GetAt (strFullPathTemplate.length () - 2))) &&
                    (IsNumber (strFullPathTemplate.GetAt (strFullPathTemplate.length () - 3))))
                    bRes = true;
        }
        else
            ;
        
        return bRes;
    }
    
#ifdef PTR_DB
    int InitializeRS (TCHAR *);
    void ReleaseRS (TCHAR *, BOOL = true, BOOL = true);
#endif

protected:
    int m_iFirstExtError, m_iLastExtError;
    HFileInfo *m_pHFIFirst, *m_pHFILast;
    //HInfoTemplate m_stHITLast; //Дата/время модификации, НОМЕРа расширенИЙ - перв./кр.
    
    int m_iIdTemplate;
	HTREEITEM m_hTreeItem;
	//Смещение от начала суток дата/время МОДИФикации файла
	//Смещение от начала суток дата/время ЗАПУСКа задачи файла
	//Время отработки задачи
	//Время между предыдущим и текущим (текущим и следующим) значением  'm_tmSpanData' или 'm_tmSpanModify' или 'm_tmSpanTask'
    CTimeSpan m_tmSpanModify, m_tmSpanTask, m_tmSpanWork;
    //CTimeSpan m_tmSpanIntervalDefinition;
    int m_iIdInterval; //, m_tmSpanInterval;
    
    int SetIdInterval (CTimeSpan tmSpanDefinition) {
        int iRes = 0;
        
        m_iIdInterval = PTR_DATETIMEINTERVAL->GetIdInterval (tmSpanDefinition.GetTotalSeconds ());
        if (m_iIdInterval > 0)
            ;
        else
            iRes = -1;
        
        return iRes;
    }
    
    int m_iMinNumberExt, m_iMaxNumberExt;
    HString m_strWildcard;

    int m_iCountFileFind;
    
    virtual void SetTarget (HInfoTemplate *, CTime, BOOL);
    //void SetTarget (HInfoTemplate *, CTime);
    void SetTarget (HInfoSample *, CTime, BOOL);
    //void SetTarget (HInfoSample *, CTime);
	virtual BOOL SetInfo (BOOL = true);

	int LastExt (void); //Строка с НОМЕРом расширения самого МОЛОДОГо файла
	void SetExtError (void);

#ifdef PTR_DB
    //CMapStringToPtr m_pmapDBRS;
    CMapStringToPtr m_pmapDBRS;
#endif

private:
    //CTimeSpan m_tmSpanInterval;
};
