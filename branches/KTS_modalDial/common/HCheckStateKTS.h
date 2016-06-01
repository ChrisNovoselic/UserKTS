#pragma once

#if ! defined _HCHECKSTATEKTS_H_
#define _HCHECKSTATEKTS_H_

#include "stdafx.h"
#include "HObjectTemplate.h"

extern CTime resetDigitUnitsMinute (CTime );
extern CTime resetDigitTensSecond (CTime );

class HCheckStateKTS : public HObjectTemplate {
    DECLARE_DYNAMIC (HCheckStateKTS)

    enum ID_EVENT_OPERATION {ID_EVENT_CREATETEMPLATE};

public:
	HCheckStateKTS(int , HTREEITEM , HString, CTimeSpan , CTimeSpan , CTimeSpan , int , int , int , int, BOOL = true);
    ~HCheckStateKTS (void);
    
    //CEvent m_evCreateTemplate;
    //HANDLE m_arhEvtOperation [COUNT_OPERATION];
    HANDLE m_hEvtOperation;

    //virtual void SetTarget (HInfoTemplate *, CTime, BOOL);
    		
	int GetState(void); //�������� ��������� �������
	void SetState(int); // --''--
	int GetStateItem(int); //��������� �������� �� ����������� ������ � �������
	int SetStateItem(int); // --''--
	HString GetDateInfoSample (int); //���������� �� ������� � �������
	HString GetTimeInfoSample (int);
	HString GetExtInfoSample (int);  //����� ���������� �� �������
	int GetPosItemOfExt (LPWSTR ); //����� ������� � ������� �� ������ ���������� �����
	int GetPosItemOfExt (int ); //����� ������� � ������� �� ������ ���������� �����
	CTime GetDTModifyInfoSample (int);
	void ClearDTInfoTemplate (int);
	void SetDTInfoTemplate (int, CTime);
	void CopyInfoTemplateItem (int, int);
	void CopyInfoSampleToTemplateItem (int, int);
	HString GetDateInfoTemplate (int);
	HString GetTimeInfoTemplate (int);
	int GetCountInfoSampleItem () { return m_arInfoSample.GetCount (); }
	int GetCountError (void);
	inline BOOL isAutoCreate (void) { return m_iCountPassError < m_iCountPassErrorCreate ? false : true; }
	inline void IncCountPassError (void) { m_iCountPassError ++; }
	inline void ResetCountPassError (void) { m_iCountPassError  = 0; }
	int *FillArrayPseudoPosSelectedItem (int &);
	int FillArrayStringPseudoSelectedItem (HString &);
	
    BOOL IsUpdate (void);
	BOOL Update(void);
	void ClearTimeUpdate (void);

	static int CompareID(HObjectTemplate *pFirstObj, HObjectTemplate *pSecondObj) {
		if (pFirstObj->GetID () > pSecondObj->GetID ())
		    return 1;
		else
		    return -1;
        
		return 0;
    }
protected:
    CTime m_tmLastUpdate; //��. ����/����� ���������� ���������� � �������
    
    //virtual BOOL SetInfo (BOOL = true) = 0x0;
    void SetCheck (void);

    CArray <HInfoTemplate> m_arInfoTemplate;
	CArray <HInfoSample> m_arInfoSample;
	
	void UpdateVariables (void);
    
private:
	int m_iState;	

	int m_iCountPassErrorCreate, //���������� ������ �� ���������� �������� ������������� �������� ����
	    m_iCountPassError; //������� ����� ������
};

#else
#endif //_HCHECKSTATEKTS_H_