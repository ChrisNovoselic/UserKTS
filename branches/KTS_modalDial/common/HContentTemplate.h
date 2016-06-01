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
    
    int ReadContentTemplate (HString = ""); //������ �� ���������� ����� (���� ��� ��� ����������/���������)
    int ParseTemplate (HString);
    
    //� �������� ���������� ��������� ����� ����� ����������, ������ ��� ���������� � ��
    void NewTarget (CTime, HString = "");
    //�� ����/����� �����������
    CTime GetDTTarget (CTime);

    void SetSpanData (CTimeSpan tmSpan) { m_tmSpanData = tmSpan; }
        
    int CalculateTemplate (HString, HString); //������ �������� ����������
    int GenerateContentTemplate (void); //���������� ����� ��� ������, ��� ������� 'NULL'
    int WriteContentTemplate (HString = ""); //������ � ��������� ���� (���� ��� ��� ����������/���������)
    
    int GetCountLine () { return m_ardblParametrsTotalData.GetSize (); }
    
    HString TargetFileName (BOOL = true); //����� �������� ����� (�������) ������������� ������� ����/����� ��� ������ ����� !!!
    SYSTEMTIME stmTargetLastWritten (void); //����/����� ����������� ������������ �����
        
#if defined PTR_DB
    int IsExistsTable (void);
    int IsExistsTable (HString );
	int CreateTable (); //�������� ������� �������
    int GetContentTemplate(int = -1); //������ �� ��
    int SetContentTemplate(); //���������� � ��
#endif

protected:
    CTimeSpan m_tmSpanData; //����� (���������) ����� ������ ��� ������������ ����� (�������� �� ������ ����� ����/����� ������ ����� ������ ())
    CTime m_tmTarget; //����/����� (���������) ����� ������ ��� ������������ �����
    HString m_strDirOutput;
    HInfoTemplate m_stHITTarget;

    CArray <HString> m_arstrAContent;
    CPtrArray m_arptrOnceLineTemplate;

private:    
    //������� ������ ???
	CArray <double> m_ardblParametrsTotalData;
    //������� ��˸��� ��������
    int m_iCountNullValue;
};
