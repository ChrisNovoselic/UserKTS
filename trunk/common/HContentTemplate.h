#pragma once
#include "hobjecttemplate.h"

class HContentTemplate : public HObjectTemplate {
    DECLARE_DYNCREATE (HContentTemplate)
public:
    HContentTemplate () {}
    //HContentTemplate(int , HTREEITEM , HString, CTimeSpan , CTimeSpan , CTimeSpan , CTimeSpan , int , int , BOOL = true);
    HContentTemplate (int , HTREEITEM , HString, CTimeSpan , CTimeSpan , CTimeSpan , int , int , int , BOOL = true);
    
    ~HContentTemplate (void);
    
        int m_iStop;
    
    CPtrArray& CopyContentTemplate () { return m_arptrOnceLineTemplate; }
    void CopyContentTemplate (CPtrArray &);
    
    int ReadContentTemplate (HString = ""); //Чтение из указанного файла (ОДИН раз при ДОБАВЛЕНИе/ИЗМЕНЕНИе)
    int ParseTemplate (HString);
    
    //В качестве аргументов начальное время сбора информации, строка для соединения с БД
    void NewTarget (CTime, HString = "");
    //Из дата/время МОДИФИКАЦии
    CTime GetDTTarget (CTime);

    void SetSpanData (CTimeSpan tmSpan) { m_tmSpanData = tmSpan; }
        
    int CalculateTemplate (HString, HString); //Расчёт значений ПАРАМЕТРов
    int GenerateContentTemplate (void); //Генерирует БАЙТы для записи, при НЕУДАЧе 'NULL'
    int WriteContentTemplate (HString = ""); //Запись в указаннЫЙ файл (ОДИН раз при ДОБАВЛЕНИе/ИЗМЕНЕНИе)
    
    int GetCountLine () { return m_ardblParametrsTotalData.GetSize (); }
    
    HString TargetFileName (BOOL = true); //Кроме возврата имени (ПОЛНого) устанавливает ТЕКУЩее дата/время для НОВого файла !!!
    SYSTEMTIME stmTargetLastWritten (void); //Дата/время модификации ФОРМИРУЕМОГо файла
        
#if defined PTR_DB
    int IsExistsTable (void);
    int IsExistsTable (HString );
	int CreateTable (); //Создание таблицы ШАБЛОНа
    int GetContentTemplate(int = -1); //Чтение из БД
    int SetContentTemplate(); //Сохранение в БД
#endif

protected:
    CTimeSpan m_tmSpanData; //ВРЕМя (началЬное) СБОРа данных для ФОРМИРУЕМОГо файла (Смещение от начала суток дата/время НАЧАЛа сбора данных ())
    CTime m_tmTarget; //Дата/время (началЬное) СБОРа данных для ФОРМИРУЕМОГо файла
    HString m_strDirOutput;
    HInfoTemplate m_stHITTarget;

    CArray <HString> m_arstrAContent;
    CPtrArray m_arptrOnceLineTemplate;

private:    
    //Итоговы данные ???
	CArray <double> m_ardblParametrsTotalData;
    //Счётчик НУЛёВых значений
    int m_iCountNullValue;
};
