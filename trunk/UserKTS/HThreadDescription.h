#pragma once


#define EMPTY_DATA lengthData () == sizeof (long) ? true : false

class HThreadDescription : public CObject
{
public:
    enum ID_DATAITEM {HTREEITEM_TEMPLATE = 666, PATH_DIRECTORY_WATCHED,
    PTR_TREECTRLTEMPLATE = 1977, PTR_CHECKSTATETEPMLATE, PTR_ARPOSSELECTEDITEM, STRING_AREXTSELECTEDITEM, STRING_CONNECTDB, STRING_TYPESOURCE, NUMBER_OPERATION, STRING_DIRECTORYUNDO, NUMBER_IDPROGRESSBAR, SPAN_INTERVALDATA};

    HThreadDescription ();
    ~HThreadDescription ();
	
	//HANDLE hEvtTerminate; Õ≈ –¿¡Œ“‡ÂÚ ‚ 'WaitForMultipleObjects' ???
	HANDLE hThread;
    DWORD idThread;
	DWORD idExitCodeThread;
	
	int AddData (int , void *, long );
	int AddData (HTREEITEM *);
	int AddData (int, CString);
	int AddData (int, HString);
	
	void DeleteData ();
	
	HString stringData (BYTE *);
	TCHAR *lpwstrData (BYTE *);
	//BYTE *GetDataOfNumber (int );
	BYTE *GetDataOfId (int );
	BYTE *lpDataDirWatched;
	int m_iSizeDataWatched;

protected:
	
private:
    //void *m_pInData;
    CMapWordToPtr m_mapInData;
    
    //BYTE *GetData (long );
    
    long lengthDataOfId (int );
    //long lengthData (int * = NULL);
    int countDataItem ();
    //long offsetDataOfNumber (int );
    //long offsetDataOfId (int );
};
