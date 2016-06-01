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
    HString GetSourceField (int , HString &);			//����� ��� ��������� � '1-�� ���-�' �������� ���� '2-�� ���-�'
    //� 'Compile...' �������� ����������� �������� ���� �� = FILES_DB | SQL_DB
	HString CompileSourceField (int , HString &, int);	//���������� ������ �� ����������� ������ ���������� � �� (��� ���� ����������)
    HString CompileTypeSource (int , int);				//���������� ������ ���� ����������
    HString CompileConnectDB (int , int);				//���������� ������ ���������� � �� (��� ���� ����������)
    HString CompileConnectDB (HString &, int);			//�� ����������� ???
    BOOL IsAlternativeSource (int );

private:
    static TYPE_DB GetTypeDB (int );
    static int GetIdType (int );

    //������ ���� �������� �� 'Template'
    HString GetItemFullPath (int );

protected:
};

#else
#endif