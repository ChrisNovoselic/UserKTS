#pragma once
#include "HWinThread.h"
#include "HContentTemplate.h"

// HThreadDirWatched

class HThreadCreateTemplate : public HWinThread
{
	DECLARE_DYNCREATE(HThreadCreateTemplate)

protected:
    // protected constructor used by dynamic creation
	
public:
    HThreadCreateTemplate();
    virtual ~HThreadCreateTemplate();
    
    int IsPtrContentTemplateRun (void) { return m_pContentTemplateRun == NULL ? 0 : 1;  }
    void Stop (void) { if (! (m_pContentTemplateRun == NULL)) m_pContentTemplateRun->m_iStop = 1; else ; }
    
    void GetContentTeplate (CPtrArray &);
    void FreeContentTemplate (void);

    enum ID_DATAITEM {HTREEITEM_TEMPLATE = 666, PATH_DIRECTORY_WATCHED,
    PTR_TREECTRLTEMPLATE = 1977, PTR_CHECKSTATETEPMLATE, PTR_ARPOSSELECTEDITEM, STRING_AREXTSELECTEDITEM, STRING_CONNECTDB, STRING_TYPESOURCE, NUMBER_OPERATION, STRING_DIRECTORYUNDO, NUMBER_IDPROGRESSBAR, SPAN_INTERVALDATA, BOOL_AUTOCAHNGESOURCE, SHORT_NUMBERBEGINSOURCE, NUMBER_IDTEMPLATE};

	BOOL InitInstance();
	int ExitInstance();

    //int Run();
	afx_msg void OnResumeRun (WPARAM , LPARAM );
	
	int AddData (HTREEITEM *);
		
private:
    HContentTemplate *m_pContentTemplateTmp;
    HContentTemplate *m_pContentTemplateRun;

protected:
	DECLARE_MESSAGE_MAP()
};


