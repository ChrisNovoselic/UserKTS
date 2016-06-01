#include "stdafx.h"
#include "HThreadDescription.h"

HThreadDescription::HThreadDescription () : CObject ()
{
    idExitCodeThread = WAIT_IO_COMPLETION;
    m_iSizeDataWatched = 4096;
    lpDataDirWatched = DEBUG_NEW BYTE [m_iSizeDataWatched + 1];
}

HThreadDescription::~HThreadDescription ()
{
    //По перегруженному оператору '++' - УДОБНО, но НЕЛЬЗЯ изменить разность между элементами '++ = + 1'
    /*
    BYTE *pData = NULL;
    
    for (ID_DATAITEM curId = ID_DATAITEM::HTREEITEM_TEMPLATE; curId <= ID_DATAITEM::SPAN_INTERVALDATA; curId ++)
    {
        data = GetDataOfId (curId);
        //if (data)
        //    delete data;
    }
    */
    
    DeleteData ();
        
    delete lpDataDirWatched;
}

void HThreadDescription::DeleteData (void)
{
    BYTE *pData = NULL;
    POSITION pos;
    WORD key;
    
    if (pos = m_mapInData.GetStartPosition())
    {
        while (pos)
        {        
            m_mapInData.GetNextAssoc (pos, key, (void *&) pData);
            if (pData)
                delete pData;
        }
        
        m_mapInData.RemoveAll ();
    }
    else
        ;
}

int HThreadDescription::AddData (int id, void *data, long len)
{
    BYTE *pInData = NULL;
    
    m_mapInData.Lookup (id, (void *&) pInData);
    if (pInData)
        delete pInData;
        
    pInData = DEBUG_NEW BYTE [len + sizeof (long)];
    
    memset (pInData, 0x0, len + sizeof (long));

    *(long *) pInData = len;
    memcpy (pInData + sizeof (long), data, len);

    m_mapInData.SetAt (id, pInData);

    return m_mapInData.GetSize ();
}

int HThreadDescription::AddData (HTREEITEM *phItem) {
    return AddData (HTREEITEM_TEMPLATE, (BYTE *) phItem, sizeof (HTREEITEM));
}

int HThreadDescription::AddData (int id, CString str) {
    return AddData (id, (BYTE *) str.GetBuffer (), str.GetLength () * sizeof (TCHAR) + sizeof (TCHAR));
}

int HThreadDescription::AddData (int id, HString str) {
    return AddData (id, (BYTE *) str.GetBuffer (), (str.length () + 1) * str.szElement ());
}

BYTE *HThreadDescription::GetDataOfId (int id) {
    BYTE *pData = NULL;
    m_mapInData.Lookup (id, (void *&)pData);
    
    if (pData)
        return (pData + sizeof (long));
    else
        return NULL;
}

HString HThreadDescription::stringData (BYTE *data) {
    //CString strRes;
	//while (* (TCHAR *) data)
	//{
	//    strRes.appendChar (* (TCHAR *) data);
	//    data += sizeof (TCHAR);
	//}
	//
	//strRes.AppendChar (0x0);
	
	//return strRes;	
	 return HString (data, HString::WCSTR);
}

TCHAR *HThreadDescription::lpwstrData (BYTE *data) {
    TCHAR *strRes = NULL;
    short shLenRes = 0;
	while (* (TCHAR *) (data + shLenRes ++));
	
	strRes = DEBUG_NEW TCHAR [shLenRes + 1];
	memcpy ((BYTE *) strRes, (BYTE *) data, shLenRes * sizeof (TCHAR));
	strRes [shLenRes] = 0x0;
	
	return strRes;
}

int HThreadDescription::countDataItem () {
    return m_mapInData.GetSize ();
}

long HThreadDescription::lengthDataOfId (int id) {
    BYTE *pData = NULL;
    m_mapInData.Lookup (id, (void *&)pData);
    
    return *((long *) pData);
}

//Работа с ЛИНЕЙНым массивом данных
/*
HThreadDescription::HThreadDescription () : CObject ()
{
    m_pInData = DEBUG_NEW BYTE [sizeof (long)];
    //memset (m_pInData, 0, sizeof (long));
    * ((long *) m_pInData) = 0;
}

HThreadDescription::~HThreadDescription ()
{
    BYTE *data = NULL;
    for (ID_DATAITEM curId = ID_DATAITEM::HTREEITEM_TEMPLATE; curId <= ID_DATAITEM::SPAN_INTERVALDATA; curId ++)
    {
        data = GetDataOfId (curId);
        //if (data)
        //    delete data;
    }
    delete m_pInData;
}

int HThreadDescription::AddData (int id, void *data, long len)
{
    long length = 0;
    int countItem = -1;
    BYTE *pInData = NULL;

    if (m_pInData)
    {
        length = lengthData (&countItem);
        if (countItem == 0)
        {
            delete m_pInData;
            m_pInData = DEBUG_NEW BYTE [sizeof (long) + sizeof (int) + len + sizeof (long)];
            memset (m_pInData, 0x0, sizeof (long) + sizeof (int) + len + sizeof (long));
        }
        else
        {
            pInData = DEBUG_NEW BYTE [length];
            memset (pInData, 0, length);
            memcpy (pInData, m_pInData, length);
            
            delete m_pInData;
            m_pInData = DEBUG_NEW BYTE [length + sizeof (long) + sizeof (int) + len + sizeof (long)];
            memset (m_pInData, 0x0, length + sizeof (long) + sizeof (int) + len + sizeof (long));
            memcpy (m_pInData, pInData, length);
            
            delete pInData;
        }
        
        * ((long *)(((BYTE *) m_pInData) + length - sizeof (long))) = len + sizeof (long) + sizeof (int);
        * ((int *)(((BYTE *) m_pInData) + length)) = id;
        memcpy ((((BYTE *) m_pInData + length - sizeof (long) + sizeof (long) + sizeof (int))), data, len);
        //Заполнение 0x0 КРАЙнего БАЙТа
        //Вариант №1
        memset ((((BYTE *) m_pInData + length - sizeof (long) + sizeof (long) + sizeof (int) + len)), 0, sizeof (long));
        //Вариант №2
        //* (long *)(((BYTE *) m_pInData + length + sizeof (int) + len)) = 0;
    }
    else
        return -1;
        
    return ++countItem;
}

int HThreadDescription::AddData (HTREEITEM hItem)
{
    return AddData (HTREEITEM_TEMPLATE, (BYTE *) &hItem, sizeof (HTREEITEM));
}

int HThreadDescription::AddData (int id, CString str)
{
    return AddData (id, (BYTE *) str.GetBuffer (), str.GetLength () * sizeof (TCHAR) + sizeof (TCHAR));
}

BYTE *HThreadDescription::GetData (long offset)
{
    BYTE *pRes = NULL;
    long length = * (long *)(((BYTE *) m_pInData) + offset - (sizeof (long) + sizeof (int))) - (sizeof (long) + sizeof (int));

    pRes = DEBUG_NEW BYTE [length];
    memset (pRes, 0x0, length); 
    memcpy (pRes, ((BYTE *) m_pInData) + offset, length);
    
    return pRes;
}

BYTE *HThreadDescription::GetDataOfNumber (int num)
{
    long offset = offsetDataOfNumber (num);
    
    if (offset < 0)
        return NULL;
    
    if (offset == 0)
        offset += (sizeof (long) + sizeof (int));
    
    return GetData (offset);
}

BYTE *HThreadDescription::GetDataOfId (int id)
{
    long offset = offsetDataOfId (id);
    
    if (offset < 0)
        return NULL;
    
    if (offset == 0)
        offset += (sizeof (long) + sizeof (int));
  
    return GetData (offset);
}

CString HThreadDescription::stringData (BYTE *data)
{
    CString strRes;
	while (* (TCHAR *) data)
	{
	    strRes.AppendChar (* (TCHAR *) data);
	    data += sizeof (TCHAR);
	}
	
	return strRes;
}

int HThreadDescription::countDataItem ()
{
    int iRes = -1;
    long length = 0, offset = 0;
    
    if (m_pInData)
    {
        do
        {
            offset = * (long *)((BYTE *) m_pInData + length);
            length += offset + sizeof (long);
            
            iRes ++;
        }
        while (offset);
    }
    else
        ; //???
        
    return iRes;
}

long HThreadDescription::lengthData (int *pCountItem)
{
    long length = sizeof (long), offset = 0;
    
    if (pCountItem)
        *pCountItem = -1;
        
    if (m_pInData)
    {
        do
        {
            offset = * (long *)((BYTE *) m_pInData + length - sizeof (long));
            length += offset;
            
            if (pCountItem)
                (* pCountItem) ++;
        }
        while (offset);
    }
    else
        //Ошибка!
        return -1;

    return length;
}

long HThreadDescription::offsetDataOfNumber (int num)
{
    long lRes = 0;

    if (m_pInData)
    {
        //while (num)
        while (num && (* (long *) ((BYTE *) m_pInData + lRes)))
        {
            lRes += (* (long *) ((BYTE *) m_pInData + lRes));
            
            num --;
        }
        
        //Как проверить, что это НАШе смещение ???
        lRes += (sizeof (long) + sizeof (int));
    }
    else
        return -1;
        
    return lRes;
}

long HThreadDescription::offsetDataOfId (int id)
{
    long lRes = 0;

   if (m_pInData)
    {
        //while (num)
        while (! (id == (* (int *) ((BYTE *) m_pInData + lRes  + sizeof (long)))) && ((* (long *) ((BYTE *) m_pInData + lRes))))
        {
            lRes += (* (long *) ((BYTE *) m_pInData + lRes));
        }
        
        if (* (long *) ((BYTE *) m_pInData + lRes))
            lRes += sizeof (long) + sizeof (int);
        else
            lRes = -1;
    }
    else
        return -1;
        
    return lRes;
}
*/