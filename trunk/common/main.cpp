#include "stdafx.h"
#include "HContentTemplate.h"

#ifdef PAR_ENERG_VER_5
#include "par_energ5.h"
#endif

#ifdef PAR_ENERG_VER_6
#include "par_energ6.h"
#endif

#ifdef PAR_TORNADO_PTK_1
#include "par_tornado_ptk_1.h"
#endif

#ifdef PAR_TORNADO_PTK_6
#include "par_tornado_ptk_6.h"
#endif

#ifdef _USE_SQL
#include "HData.h" //��� ��������� ������ ���������� ��� ����������� � ��
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;
DECLARE_HA2KW
EXT_DECLARE_HFILELOG

using namespace std;

extern double roundToPrecision (double val, int precision);

extern int monthDuration (int , int );

extern CString ParseTypeSource (int, CString);
extern CString ParseConnectDB (int, CString);

extern void FreeMemoryString (CString &);
extern void FreeMemoryArrayString (CStringArray &);

void AbortExit (void)
{
    if (PTR_FILELOG)
        delete PTR_FILELOG;
        
    ExitProcess (0x1);
}

int main_common (int argc, TCHAR* argv[])
{
    int nRetCode = 0;
	
	//�������� (�����������) ���-�����
    try { PTR_FILELOG = new HFileLog (10000, LEVEL_INFO_2); }
    catch (CException *err)
    {
	    _tprintf (_T("Fatal Error: not open LOG-file\n"));
	    ExitProcess (0x1); //???
    }

	// initialize MFC and print and error on failure
	if (! AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		PTR_FILELOG->WriteMessageWithDT ("Fatal Error: MFC initialization failed", LEVEL_INFO_1);
		AbortExit ();
	}
	
    int i, c;	//������� �����

	// TODO: code your application's behavior here.
    SYSTEMTIME stmBegin, stmEnd, systime;
	CTime tCur, tEnd, tSys, tTmp;

	PTR_FILELOG->WriteMessageWithDT ("������ ��������� ������...", LEVEL_INFO_0);
    		
    TCHAR strFileNameOfRead [MAX_PATH];
	memset (strFileNameOfRead, 0x0, MAX_PATH * sizeof (TCHAR));
	switch (argc)
	{
	    case 0: //�� �.�.
	        break;
	    case 1: //������ ��� ������� ���������
	        memcpy (strFileNameOfRead, argv [0], strlen (argv [0]));
	        strFileNameOfRead [strlen (strFileNameOfRead) - 3] = 'i';
	        strFileNameOfRead [strlen (strFileNameOfRead) - 2] = 'n';
	        strFileNameOfRead [strlen (strFileNameOfRead) - 1] = 'i';
	        break;
	    case 2: //��, ��� ����
	        //���� � INI-����� ��� �������� (������� ���������� ���������� ��������� ������)
	        memcpy (strFileNameOfRead, argv [1], strlen (argv [1]));
	        break;
	    default:
	        //���� � INI-����� �������� ������� (������� ���������� ���������� ��������� ������)
	        i = 1;
	        memcpy (strFileNameOfRead, argv [i], strlen (argv [i]));
	        do {
	            i ++;
	            strFileNameOfRead [strlen (strFileNameOfRead)] = 0x20;
	            memcpy (strFileNameOfRead + strlen (strFileNameOfRead), argv [i], strlen (argv [i]));
	        }
	        while (i < argc - 1);
	}
	
    CString strTmp;  //��������� ����������

    //CStringArray arstrFileOfRead;
    
    //unsigned long szFileOfRead; //������ ����� (� �������������� ������) ��� ������)
    char *bufferOfRead = 0x00; //����� ��� ������ �����

	int iReasonablyINI = 0;//��� ��������� ���������� ���������� ������������� ����������
	int iResponce; //��� ��������� ���������� ���������� �������
    
    CString strConnectDB, strTypeSource, strAutoChangeSource;
			
	//��������� �� ������� � ������ ����������� � �����
	//HFileInfo *phfi = 0x00, *phfiTmp = 0x00;

	HFileIni *pFileIni = NULL;

	HContentTemplate *pTemplate = NULL;

	//����� ���-�����
	strTmp.Format ("����� ���-�����: %s", strFileNameOfRead);
	PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
	strTmp.Empty ();
	
	pFileIni = new HFileIni (strFileNameOfRead);
	iReasonablyINI = pFileIni->IsReasonably ();
		
    if (iReasonablyINI == 0)
    {
		pFileIni->OutParametrToLog ();
		
		//�������� �������� ��� �������� ����� ���-����� = 0
		PTR_FILELOG->SetMaxCountRecord (atoi (pFileIni->GetParametrValue ("MaxCountRecordFileLOG")));
		
        if (pFileIni->GetParametrValue ("AutoExecute").CollateNoCase ("false") == 0)
        {
			strTmp = "������ �� ������������� �������� ����������";
			PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
			strTmp.Empty ();

            cout << "��������� �� �������� ����������? ���������� (y/n)? ";
            
            i = 0; c = 0;
            do
            {
                if (i != 0)
                    cout << "��������� (y/n)! ��� �������: ";
                i = getchar ();
                while (getchar () != 0xA)
                    c ++;
                if (c > 0)
                {
                    i = 1;
                    c = 0;
                }
            }
            while ((i != 'Y') && (i != 'y') && (i != 'N') && (i != 'n'));
            
            if ((i == 'N') || (i == 'n'))
                iReasonablyINI = pFileIni->GetParametrsCount ();
        }

		if (iReasonablyINI == 0)
        {
			PTR_FILELOG->SetLevelOutputInfo (atoi (pFileIni->GetParametrValue ("LevelOutputInfo")));
			
			PTR_FILELOG->SetMaxCountRecord (atoi (pFileIni->GetParametrValue ("MaxCountRecordFileLOG")));
			PTR_FILELOG->WriteMessageWithDT ("��������� �������", LEVEL_INFO_0);
						           
            strTmp = PTR_FILELOG->GetHostName ();
            if (strTmp.CollateNoCase (pFileIni->GetParametrValue ("HostNameTarget")) == 0)
                pFileIni->CorrectParametrValue ();
            strTmp.Empty ();
            
            strTmp.Format ("������ ��������� � �������� � ������: %s �� ���-�����", pFileIni->GetParametrValue ("DirTemplate") + pFileIni->GetParametrValue ("FindFile"));
			PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
			strTmp.Empty ();

			iResponce = 0;
			strTmp = pFileIni->GetParametrValue ("Time_modify");
            if ((strTmp != "default") && (! strTmp.IsEmpty ()))
	        {//������ ������ ��������� �������� �� ���������� ����/�����
	            iResponce = atoi (strTmp.Left (2)) * 60 * 60 + 
							atoi (strTmp.Mid (3, 2)) * 60 + 
							atoi (strTmp.Right (2));
	        }
	        else
				;
				
            CString strDirTemplate = pFileIni->GetParametrValue ("DirTemplate");
            if (! (strDirTemplate.GetAt (strDirTemplate.GetLength () - 1) == '\\'))
                strDirTemplate.AppendChar ('\\');

			pTemplate = new HContentTemplate (0,
			                                0,
			                                strDirTemplate + pFileIni->GetParametrValue ("FindFile"),
			                                CTimeSpan (iResponce),
			                                0,
			                                0,
			                                CTimeSpan (atoi (pFileIni->GetParametrValue ("Interval").GetBuffer ())),
			                                atoi (pFileIni->GetParametrValue ("MinNumberExt")),
			                                atoi (pFileIni->GetParametrValue ("MaxNumberExt")),
			                                false);
			                                
            strTmp.Format ("������� ������ ��� ������ � ������-��������.");
			PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_1);
			strTmp.Empty ();
			                                
			if (pTemplate->ReadContentTemplate () == 0)
			{
			    strTmp.Format ("��������� ���������� �����-�������.");
			    PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_1);
			    strTmp.Empty ();

                //pTemplate->ParseTemplate (pFileIni->GetParametrValue ("TypeSource"));
                
			    strTmp.Format ("��������� ������ � ������-��������.");
			    PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
			    strTmp.Empty ();

			    strTypeSource = pFileIni->GetParametrValue ("TypeSource");
			    
			    if (pTemplate->ParseTemplate (ParseTypeSource (0, strTypeSource)) == 0)
			    {//��������� ������ �� ������ (����� ��� ���, ������ �������, �����)
                    strTmp.Format ("���������� �������� ����/�����...");
			        PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
			        strTmp.Empty ();
        			
			        //���������� �������� ����/�����
			        strTmp = pFileIni->GetParametrValue ("DateTime_debug");
			        if ((! (strTmp == "default")) && (! strTmp.IsEmpty ()))
                    {//������ ������ ��������� �������� �� ���������� ����/�����
                        systime.wDay = atoi (strTmp.Mid (0, 2));
                        systime.wMonth = atoi (strTmp.Mid (3, 2));
                        systime.wYear = atoi (strTmp.Mid (6, 4));
                        systime.wHour = atoi (strTmp.Mid (11, 2));
                        systime.wMinute = atoi (strTmp.Mid (14, 2));
                        systime.wSecond = atoi (strTmp.Mid (17, 2));
                    }
                    else
		            {
				        //GetSystemTime (&systime);
				        GetLocalTime (&systime);
			        }
                    
                    strTmp = pFileIni->GetParametrValue ("DateTime_begin");
                    //������ ������������� ������
                    if ((strTmp != "default") && (! strTmp.IsEmpty ()))
                    {//������ ������ ��������� �������� �� ���������� ����/�����
                        stmBegin.wDay = atoi (strTmp.Mid (0, 2));
                        stmBegin.wMonth = atoi (strTmp.Mid (3, 2));
                        stmBegin.wYear = atoi (strTmp.Mid (6, 4));
                        stmBegin.wHour = atoi (strTmp.Mid (11, 2));
                        stmBegin.wMinute = atoi (strTmp.Mid (14, 2));
                        stmBegin.wSecond = atoi (strTmp.Mid (17, 2));
                    }
                    else
		            {
			            stmBegin = pTemplate->stmLastWritten ();

			            switch (atoi (pFileIni->GetParametrValue ("Interval")))
			            {
				            case 1800:
				            case 3600:
					            break;
				            default:
					            //���������� ������������ �������������� ��������
					            strTmp = pFileIni->GetParametrValue ("Time_begin");
					            if ((strTmp != "default") && (! strTmp.IsEmpty ()))
					            {
						            stmBegin.wHour = atoi (strTmp.Mid (0, 2));
						            stmBegin.wMinute = atoi (strTmp.Mid (3, 2));
						            stmBegin.wSecond = atoi (strTmp.Mid (6, 2));
					            }
					            else
						            //�������������� �������� ���� �� �������� - ������ ������ �� �����
						            stmBegin = SYSTEMTIME (systime);
			            }
		            }

                    strTmp = pFileIni->GetParametrValue ("DateTime_end");
                    if ((strTmp != "default") && (! strTmp.IsEmpty ()))
	                {//������ ������ ��������� �������� �� ���������� ����/�����
	                    stmEnd.wDay = atoi (strTmp.Mid (0, 2));
                        stmEnd.wMonth = atoi (strTmp.Mid (3, 2));
                        stmEnd.wYear = atoi (strTmp.Mid (6, 4));
                        stmEnd.wHour = atoi (strTmp.Mid (11, 2));
                        stmEnd.wMinute = atoi (strTmp.Mid (14, 2));
                        stmEnd.wSecond = atoi (strTmp.Mid (17, 2));
	                }
	                else
	                    stmEnd = SYSTEMTIME (systime);
        	            
	                strTmp.Empty ();

                    //??? ���������� ���-�� ���� ???
		            tCur = CTime (stmBegin);
		            tEnd = CTime (stmEnd);
		            tSys = CTime (systime);
	                /*			
	                tSys = CTime (stmBegin);
	                tSys += 1 * 24 * 60 * 60;
	                tSys.GetAsSystemTime (systime);
	                */
                				
	                //���������� ������ �������� �����
	                //strTmp = pTemplate->LastExt ();
	                //���������� ������ �����
        	        
	                //����� - ������� ���
	                pTemplate->SetSpanData (stmBegin.wHour * 60 * 60 + stmBegin.wMinute * 60 + stmBegin.wSecond);

	                //������ ������ CUserKTSDlg::CompileConnectDB        			
			        i = 0;
			        int posEmpty = -1;
			        CString strEmpty = "EMPTY";
			        while (true)
			        {
			            strConnectDB.Append (HData::GetParametrConnectDB (i));
			            strConnectDB.AppendChar ('=');
			            strConnectDB.Append (pFileIni->GetParametrValue (HData::GetParametrConnectDB (i)));
			            //������� 'EMPTY'
			            posEmpty = 0;
			            posEmpty = strConnectDB.Find (strEmpty, posEmpty);
			            while (posEmpty > -1)
			            {
			                strConnectDB.Delete (posEmpty, strEmpty.GetLength ());
			                posEmpty = strConnectDB.Find (strEmpty, posEmpty);
        			    }
			            
			            if (! (HData::GetParametrConnectDB (++ i) == NULL))
			                strConnectDB.AppendChar (';');
                        else
                            break;
			        }
        			
			        strTypeSource.Insert (0, "TypeSource=");
        		            			
	                while (tCur + atoi (pFileIni->GetParametrValue ("Interval")) < tEnd + 59)
		            {
	                    //if ((tSys - tCur).GetDays () < (atoi (pFileIni->GetParametrValue ("MaxNumberExt")) - atoi (pFileIni->GetParametrValue ("MinNumberExt")) + 1))
	                    //if ((tSys - tCur).GetDays () > 0)
	                    if ((tSys - tCur).GetTotalSeconds () > atoi (pFileIni->GetParametrValue ("Interval")))
                        {			        
			                pTemplate->NewTarget (tCur, pFileIni->GetParametrValue ("DirOutput"));
			                strTmp = pTemplate->TargetFileName (true);
        			        
			                memset (strFileNameOfRead, 0x0, MAX_PATH);
			                memcpy (strFileNameOfRead, strTmp.GetBuffer (), strTmp.GetLength ());
        					
					        //������ ������� � 'strTmp' ???
					        strTmp.Empty ();
					        strTmp.Format ("��� ������������� �����: %s", strFileNameOfRead);
					        PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
					        strTmp.Empty ();
        					
			                BOOL bReplacement = true;
			                //����� ����� � ����� �� ������
			                CFileFind ff;
			                HFileInfo * phfiTmp = NULL;
        			        				    
			                if (ff.FindFile (strFileNameOfRead))
			                {//������ � ����� �� ������
			                    phfiTmp = new HFileInfo (strFileNameOfRead);
			                    if (CompareSYSTEMTIME (phfiTmp->stmLastWritten (), pTemplate->stmTargetLastWritten ()) < 2)
		                            //������������ ����� �� ��� ������ (equale or earlier)
		                            if (pFileIni->GetParametrValue ("ReplaceEqualeEarlier").CollateNoCase ("false") == 0)
		                                bReplacement = false;
                            }

				            if (! (phfiTmp == NULL))
				                delete phfiTmp;
				            ff.Close ();

					        strTmp.Format ("���������� �� ���������: %i", bReplacement);
					        PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
					        strTmp.Empty ();
        				    
			                if (bReplacement)
			                {
						        PTR_FILELOG->WriteMessageWithDT ("������ �������� ����������...", LEVEL_INFO_1);
        						
						        //pTemplate->CalculateTemplate (pFileIni->GetParametrValue ("DataBase"), pFileIni->GetParametrValue ("TypeSource"));

						        int iNumAlternative = 0, iCalculateRes = -1;
                                do
                                {
                                    //iNumAlternative ++; //���������� ����
                                    //'ParseConnectDB' � 'ParseTypeSource' ���������� ������� ������ 'HThreadCreateTemplate'
                                    iCalculateRes = pTemplate->CalculateTemplate (ParseConnectDB (iNumAlternative, strConnectDB), ParseTypeSource (iNumAlternative, strTypeSource));
                                    iNumAlternative ++; //���������� ���� � ������� �� ����� � ������� 'ParseTemplate' ???
                                }
                                while ((! (iCalculateRes == ERR_PARSE_NULL)) && (! (iCalculateRes == ERR_ARGS)) && (! (iCalculateRes == 0)) && ((strAutoChangeSource.CollateNoCase ("true") == 0) || (strAutoChangeSource.CollateNoCase ("yes") == 0))); //����� ��������� ???
        						
						        if (iCalculateRes == 0)
						        {
						            PTR_FILELOG->WriteMessageWithDT ("���������� ���������� ������������� �����...", LEVEL_INFO_1);
						            //pTemplate->GenerateContentTemplate (pFileIni->GetParametrValue ("TypeSource"));
						            pTemplate->GenerateContentTemplate ();
            						
			                        strTmp = pFileIni->GetParametrValue ("DirOutput");
			                        if (! PathIsDirectory (strTmp))
			                            CreateDirectory (strTmp.Left (strTmp.GetLength () - 2), NULL);

                                    PTR_FILELOG->WriteMessageWithDT ("���������� � ������ �����...", LEVEL_INFO_1);
                                    pTemplate->WriteContentTemplate (strTmp);
                                    
						            strTmp.Format ("��������� ����: %s", strFileNameOfRead);
						            PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
						            strTmp.Empty ();
			                        //��������! ������ �� - ����� ������� ���� ������
                                }
                                else
                                    ;
                            }
                            else
                                ; //��� ������������� ������ (��� ������ � ����� �������������)
		                }
	                    else
		                    ; //���� ������ �� ����� ������ - ��� ������ (������ ���� �������� 31 ����)
        			        
                        PTR_FILELOG->WriteMessageWithDT ("�������� ����/�����...", LEVEL_INFO_0);
		                //tCur += 1 * 24 * 60 * 60;
		                tCur += atoi (pFileIni->GetParametrValue ("Interval"));
	                }//���� while �� ��������� ������� 'tCur - tEnd'
			    }
			    else
			    {
			        strTmp.Format ("������ �����-������� �� ������� ����������");
			        PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
			        strTmp.Empty ();
			    }
            }
			else
			{
			    strTmp.Format ("����-������� �� ������� ���������");
		        PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
		        strTmp.Empty ();
			}
				        
	        strTmp.Format ("���������� ����������... ������������ ��������");
            PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_0);
            strTmp.Empty ();
		    
		    FreeMemoryString (strTmp);
		    
		    if (pTemplate)
		        delete pTemplate;
		    
		    if (bufferOfRead)
	            delete bufferOfRead;
	        bufferOfRead = NULL;
        }
        else
            PTR_FILELOG->WriteMessageWithDT ("��������� �� �������", LEVEL_INFO_0);
             //����� ������������ �� �������� ���������� iReasonablyINI = ���������� ����������
    }
	else
	{
		//������ INI-����� iReasonablyINI = ����� ���������
		//������ INI-����� iReasonablyINI = -1 ���� �� ������
		if (iReasonablyINI > 0)
			strTmp.Format ("������ INI-�����. ��������: %s", pFileIni->GetParametrName (iReasonablyINI));
		else
			//strTmp = "�� ������ INI-����";
			; //����� �� ��������� � ������������ INI-�����
		
		if (strTmp.GetLength ())
		    PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_1);
		
		strTmp.Empty ();
	}

	if (pFileIni)
		delete pFileIni;
	pFileIni = NULL;
   
	delete PTR_FILELOG;
    
	return nRetCode;
}