#pragma once

#if ! defined _HDBSETTINGS_H_
#define _HDBSETTINGS_H_

#include <hdatabase.h>

class HDBSettings : public CObject {
    DECLARE_DYNCREATE (HDBSettings)
public:
    HDBSettings (void);
    virtual ~HDBSettings (void);

	enum TYPE_DB {FILES_DB = 1, SQL_DB = 2};

    HString GetOption (LPCTSTR );
    HString GetOption (HString &);
    HString GetSourceField (int , HString &);			//Взять для источника с '1-ый арг-т' значение поля '2-ой арг-т'
    //В 'Compile...' крайними параметрами являются ТИПы БД = FILES_DB | SQL_DB
	HString CompileSourceField (int , HString &, int);	//Построение одного из компонентов строки соединения с БД (для ВСЕХ источников)
    HString CompileTypeSource (int , int);				//Построкние списка ВСЕХ источников
    HString CompileConnectDB (int , int);				//Построение строки соединения с БД (для ВСЕХ источников)
    HString CompileConnectDB (HString &, int);			//НЕ РЕАЛИЗОВАНА ???
    BOOL IsAlternativeSource (int );

private:
    static TYPE_DB GetTypeDB (int );
    static int GetIdType (int );

    //Полный путь элемента из 'Template'
    HString GetItemFullPath (int );

protected:
};

#else
#endif