#include "stdafx.h"
#include "HOnceLineTemplate.h"
#include "UserKTS.h"
extern CUserKTSApp theApp;

extern int FindDate(int, char * , int = 5); //По умолчанию поиск по укороченному варианту
extern int FindTime(int , char * , int = 5); //По умолчанию поиск по укороченному варианту

extern double roundToPrecision (double , int );

HOnceLineTemplate::HOnceLineTemplate(void) {
}

HOnceLineTemplate::HOnceLineTemplate (HString &description) {
#if defined _DEGUG
	FILELOG_VARIABLE

	FILELOG_IF_WRITE (HDEBUG)
	strLog.format ("%s", "HOnceLineTemplate::Конструктор (инициализация - ДО)");
	FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
	FILELOG_ENDIF
#else
#endif

    m_strADescriptionTemplate = description;
    m_strADescriptionTarget = "";
    m_strAPosition = m_strADescriptionTemplate.mid (0, 4);
    m_strAPosition.trim ();
    m_strAPlace = m_strADescriptionTemplate.mid (5, 7);
    m_strAPlace.trim ();

#if defined _DEGUG
	FILELOG_IF_WRITE (HDEBUG)
	strLog.format ("%s", "HOnceLineTemplate::Конструктор (инициализация - ПОСЛЕ)");
	FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
	FILELOG_ENDIF
#else
#endif
    
    SetCoeffMeasure ();

#if defined _DEGUG
	FILELOG_IF_WRITE (HDEBUG)
	strLog.format ("%s", "HOnceLineTemplate::Конструктор (SetCoeffMeasure - ПОСЛЕ)");
	FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
	FILELOG_ENDIF
#else
#endif
    
    //Добавление ОБЩего параметра с НУЛевым номером, формализующего использование СОВОКУПНОСТи остальных
    //НО ЕСЛи последовательностЕЙ параметров не ОДНа ???
    //m_arstrAParametrType.Add ("");
    //m_arstrAParametrValue.Add ("");
}

void HOnceLineTemplate::NewTarget () {
    int i, c;

    m_strADescriptionTarget.free ();
    m_strADescriptionTarget = m_strADescriptionTemplate;

    //for (i = 0; i < m_arstrAParametrValue.GetSize (); i ++)
    for (i = 0; i < m_arstrAParametrType.GetSize (); i ++) {
        if ((m_strADescriptionTarget.length () > 59) && (m_ardblParametrData.GetSize ())) {
            c = 1;
            //while (! (GetParametrValueDetail (i, c).IsEmpty ()))
            while (GetParametrTypeDetail (i, c).isUses () == 1)
                c++;

            memset (m_ardblParametrData.GetAt (m_ardblParametrData.GetSize () - 1), 0x0, c * sizeof (double));
        }
        else
            ; //Здесь ЕСТЬ параметры, но ДАТА/ВРЕМЯ
    }
}

int HOnceLineTemplate::ApplyParametrDataDetail (int iNum, int iNumDetail, double val)
{
    if (iNum < m_ardblParametrData.GetSize ())
    {
        //if (abs (val) > pow (10, 13))
        if (abs (val) > 10000000000000)
            val = 0.0;
        else
            ;

        m_ardblParametrData [iNum] [iNumDetail] = val;

        //Обычное суммирование
        m_ardblParametrData [iNum] [0] += val;
        
        return 0;
    }
    else
        //выход за пределы МАССИВа
        return -1;
}

double HOnceLineTemplate::GetParametrData (int iNum) {
    return m_ardblParametrData [iNum] [0];
}

double HOnceLineTemplate::GetParametrDataDetail (int iNum, int iNumDetail) {
    return m_ardblParametrData [iNum] [iNumDetail];
}

int HOnceLineTemplate::GetParametrPosLength (int iNum, int &iPosition, int &iLength) {
    HString strAParametr;
    int iPos = 0;
    iPosition = -1, iLength = -1;
    if (m_arstrAParametrType.GetSize ())
    {
        while ((iPos < m_strADescriptionTarget.length ()) && (iPos > -1))
        {
            while ((! (MB_AT (m_strADescriptionTarget, iPos) == '{')) && (iPos < m_strADescriptionTarget.length ()))
                iPos ++;

            if (! (iPos < m_strADescriptionTarget.length ()))
                iPos = -1;
            else {
                iPos ++;
                while ((! (MB_AT (m_strADescriptionTarget, iPos) == '#')) && (iPos < m_strADescriptionTarget.length ()))
                    iPos ++;
                    
                if (! (iPos < m_strADescriptionTarget.length ()))
                    iPos = -1;
                else
                {
                    iPos ++;
                    while ((! (MB_AT (m_strADescriptionTarget, iPos) == '#')) && (! (MB_AT (m_strADescriptionTarget, iPos) == '=')) && (iPos < m_strADescriptionTarget.length ()))
                        strAParametr.appendChar (MB_AT (m_strADescriptionTarget, iPos ++));
                        
                    if (atoi (MB_HSTRING (strAParametr)) == iNum)
                    {
                        iPosition = iPos - 2 - strAParametr.length ();
                        
                        strAParametr.free ();
                        iPos ++;
                        while ((! (MB_AT (m_strADescriptionTarget, iPos) == 'L')) && (iPos < m_strADescriptionTarget.length ()))
                            iPos ++;
                                                            
                        iPos ++;
                        while ((! (MB_AT (m_strADescriptionTarget, iPos) == '}')) && (iPos < m_strADescriptionTarget.length ()))
                            strAParametr.appendChar (MB_AT (m_strADescriptionTarget, iPos ++));
                            
                        iLength = atoi (MB_HSTRING (strAParametr));
						iPos ++;
                        break;
                    }
                }
            }
        }
        
        /*if (iPos < m_strADescriptionTarget.length ())
            ;
        else
            ;*/
    }
    else
        iPos = -1;
        
    if ((iPosition == -1) || (iLength == -1))
        return -1;
    else
        return iPos - iPosition;
}

//Изменение 'm_strADescription' - ЗАМЕНа {#..=L..} на ЗНАЧения в соответствии с 'L'
int HOnceLineTemplate::ApplyParametrData (int iNum, VARIANT val)
{
    return 0;
}

int HOnceLineTemplate::ApplyParametrData (int iNum, HString &val)
{
    int iRes = -1, iPos, iLength;
    iRes = GetParametrPosLength (iNum, iPos, iLength);
	if (iRes > 0)
	{//iRes == 0 ТОЖЕ ошибка
		m_strADescriptionTarget.cut (iPos, iRes);
		//Здесь исправляется ошибка в длине дата/время, напр. 7->10
		//Вырезается длина реальная 00:00:0=7
		//Вставляется ПРАВильная 00:00:00=8
		m_strADescriptionTarget.insert (iPos, val);
	}
	else
		;
    return 0;
}

//int HOnceLineTemplate::ApplyParametrData (int iNum, CString val)
//{
//    return 0;
//}

//Вызов одной из слндующих функций - значение параметра внутри КЛАССа
int HOnceLineTemplate::ApplyParametrData (int iNum)
{
	//По хорошему НЕОБХОДИМо просуммировать ВСЕ [i] [0]
	if (iNum < m_ardblParametrData.GetSize ())
	    ApplyParametrData (iNum, m_ardblParametrData [iNum] [0]);
	else
	    ;

	return 0;
}

int HOnceLineTemplate::ApplyParametrData (int iNum, double &val)
{
    int dec, sign,
		iRes = -1, iPos, iLength,
		iPrecision = 3;
    HString strATmp;
    BOOL negative;
    
    val *= GetCoeffMeasure ();
    	    
    //val = roundToPrecision (val, 2);
        
    iRes = GetParametrPosLength (iNum, iPos, iLength);
	if (iRes > 0) {//iRes == 0 ТОЖЕ ошибка
        if (m_strADescriptionTarget.length () > 6) {//Почему '6' ???
            m_strADescriptionTarget.cut (iPos, iRes);
			
            if (! (val == 0)) {
                //if (m_ardblParametrsTotalData [i - 5] >= 0) // Почему 'i - 5' ???
                if (val >= 0)
	                negative = false;
                else
	                negative = true;
	            
			    strATmp = HString (_fcvt (roundToPrecision (val, iPrecision), iPrecision, &dec, &sign));
                
                if (dec) {
	                if (dec < 0) {
						while (! (dec == 0)) {
			                //strATmp.insert (0, 48);
			                strATmp.insert (0, "0");
			                dec ++;
		                }
		                strATmp.insert (dec, "0.");
	                }
	                else
		                strATmp.insert (dec, ".");
                }
                else
	                strATmp.insert (dec, "0.");
                
                if  (negative)
	                strATmp.insert (0, "-");
				else
					;
            }
			else {
                //НУЛЕВОое значение
				strATmp = "0.";
				while (strATmp.length () < (2 + iPrecision))
					//strATmp.appendChar ('0');
					strATmp.append ("0");
			}

            while (strATmp.length () < iLength)
                strATmp.insert (0, " ");
            
            //iTmp = m_strADescriptionTarget.GetLength (); //Для отладки
            m_strADescriptionTarget.insert (iPos, strATmp);
            //iTmp = m_strADescriptionTarget.GetLength (); //Для отладки

            strATmp.free ();
        }
        else
            ; //ПОЧТИ ПУСТАЯ СТРОКА
    }
    else
        ; //Не найден ПАРАМЕТР
    
    return 0;
}

int HOnceLineTemplate::ApplyParametrData (int iNum, int &val)
{
    return 0;
}

void HOnceLineTemplate::SetCoeffMeasure ()
{
    m_dblCoeffMeasure = 1;

    HString strAMeasure;
    int iLen = m_strADescriptionTemplate.length (), iPosMeasure = iLen - 5;
    unsigned char *pchMeasure = NULL;
    
    while (iPosMeasure > 59)
    {
        strAMeasure = m_strADescriptionTemplate.mid (iPosMeasure, 5);
		if (strAMeasure.length () == 5) {
			pchMeasure = (unsigned char *) MB_HSTRING (strAMeasure);
			if (((pchMeasure [0] == 242) && (pchMeasure [1] == 251) && (pchMeasure [2] == 241)) ||
				((pchMeasure [0] == 226) && (pchMeasure [1] == 235) && (pchMeasure [2] == 225))
				)
			{//Найдено слово 'тыс'
				if (((pchMeasure [4]) == 0xf2) ||
					((pchMeasure [4]) == 0xe2))
					//Тепло (найдено 'т' - тонны):
					m_dblCoeffMeasure /= 1000;
				else
					;

				if (((pchMeasure [4]) == 0xea) ||
					((pchMeasure [4]) == 0xaa))
					//Электричество (найдено 'к' - кило):
					m_dblCoeffMeasure /= 1000000;
				else
					;
	         
				break;
			}
		}
		else
			;
	        
		if (! (pchMeasure == NULL))
		{
			//delete strAMeasure;
			pchMeasure = NULL;
		}
		else
			;
        
		iPosMeasure --;
		strAMeasure.free ();
    }
}

double HOnceLineTemplate::GetCoeffMeasure ()
{
    return m_dblCoeffMeasure;
}

int HOnceLineTemplate::SetSizeArrayParametrData ()
{
    int i, c = 0, sz = 0;
    
    //for (i = 0; i < m_arstrAParametrValue.GetSize (); i ++)
    for (i = 0; i < m_arstrAParametrType.GetSize (); i ++)
    {
        c = 1;
        //while (! (GetParametrValueDetail (i, c).IsEmpty ()))
        while (GetParametrTypeDetail (i, c).isUses () == 1)
            c++;
        
        sz += c;
        
        m_ardblParametrData.Add (DEBUG_NEW double [c]);
        memset (m_ardblParametrData.GetAt (m_ardblParametrData.GetSize () - 1), 0x0, c * sizeof (double));
    }
            
    return sz;
}

HOnceLineTemplate::~HOnceLineTemplate(void)
{
    m_strAPosition.free ();
    m_strAPlace.free();
    m_strADescriptionTemplate.free ();
    m_strADescriptionTarget.free ();

    int i;
    for (i = 0; i < m_arstrAParametrType.GetSize (); i ++)
    {
        m_arstrAParametrType.GetAt (i).free ();
    }
    m_arstrAParametrType.RemoveAll ();
    
    for (int i = 0; i < m_arstrAParametrValue.GetSize (); i ++)
    {
        m_arstrAParametrValue.GetAt (i).free ();
    }
    m_arstrAParametrValue.RemoveAll ();

	for (int i = 0; i < m_ardblParametrData.GetSize (); i ++)
    {
        delete m_ardblParametrData.GetAt (i);
    }
    m_ardblParametrData.RemoveAll ();
}

void HOnceLineTemplate::CopyTo (HOnceLineTemplate *dest)
{
    int i = 0;
    
    dest->m_strAPosition = m_strAPosition;
    dest->m_strAPlace = m_strAPlace;
    dest->m_strADescriptionTemplate = m_strADescriptionTemplate;
    dest->m_strADescriptionTarget = m_strADescriptionTarget;
    dest->m_dblCoeffMeasure = m_dblCoeffMeasure;
    
    dest->m_arstrAParametrType.SetSize (m_arstrAParametrType.GetSize ());
    for (i = 0; i < m_arstrAParametrType.GetSize (); i ++)
        dest->m_arstrAParametrType.SetAt (i, m_arstrAParametrType.GetAt (i));
    dest->m_arstrAParametrValue.SetSize (m_arstrAParametrValue.GetSize ());
    for (i = 0; i < m_arstrAParametrValue.GetSize (); i ++)
        dest->m_arstrAParametrValue.SetAt (i, m_arstrAParametrValue.GetAt (i));
    
    dest->SetSizeArrayParametrData (); //m_ardblParametrData
    //ЗНАЧения оставить НУЛевые ???
}
void HOnceLineTemplate::CopyFrom (HOnceLineTemplate *src)
{
    int i = 0;
    
    m_strAPosition = src->m_strAPosition;
    m_strAPlace = src->m_strAPlace;
    m_strADescriptionTemplate = src->m_strADescriptionTemplate;
    m_strADescriptionTarget = src->m_strADescriptionTarget;
    m_dblCoeffMeasure = src->m_dblCoeffMeasure;
    
    m_arstrAParametrType.SetSize (src->m_arstrAParametrType.GetSize ());
    for (i = 0; i < src->m_arstrAParametrType.GetSize (); i ++)
        m_arstrAParametrType.SetAt (i, src->m_arstrAParametrType.GetAt (i));
    m_arstrAParametrValue.SetSize (src->m_arstrAParametrValue.GetSize ());
    for (i = 0; i < src->m_arstrAParametrValue.GetSize (); i ++)
        m_arstrAParametrValue.SetAt (i, src->m_arstrAParametrValue.GetAt (i));
    
    SetSizeArrayParametrData (); //m_ardblParametrData
    //ЗНАЧения оставить НУЛевые ???
}

//Добавление во 2-ую строку параметров 'tm_beg', 'dt_beg', 'tm_end', 'dt_end' - МОЙ ФОРМАТ 2-ой строки!!!
int HOnceLineTemplate::AddParametrsDT (void)
{
    HString strATmp;
    int iPosParametrDate = FindDate (0, MB_HSTRING (m_strADescriptionTemplate)),
        iPosParametrTime = FindTime (0, MB_HSTRING (m_strADescriptionTemplate)),
        iPosParametrDT = 0,
        iLengthParametr = 0;
        
    if ((iPosParametrTime > -1) && (iPosParametrDate > -1))
    {//Найдены И ВРЕМЯ И ДАТА 
        if (iPosParametrTime == iPosParametrDate)
            iPosParametrDT = iPosParametrTime;
        else
        {
            if (iPosParametrTime < iPosParametrDate)
                iPosParametrDT = iPosParametrTime;
            else
                iPosParametrDT = iPosParametrDate;
        }
    }
    else
    {
        if (iPosParametrTime > -1)
            iPosParametrDT = iPosParametrTime;
            
        if (iPosParametrDate > -1)
            iPosParametrDT = iPosParametrDate;
    }
        
    if (iPosParametrDT > -1)
    {
        m_strADescriptionTemplate.cut (iPosParametrDT, m_strADescriptionTemplate.length () - iPosParametrDT);
    
        iLengthParametr = AddParametr (iPosParametrDT, "tm_beg", "", 5);
        
        strATmp = " ";
        m_strADescriptionTemplate.append (strATmp);
        
        iPosParametrDT += (iLengthParametr + strATmp.length ());
        iLengthParametr = AddParametr (iPosParametrDT, "dt_beg", "", 10);
        
        strATmp.free ();
        strATmp = " - ";
        m_strADescriptionTemplate.append (strATmp);
        
        iPosParametrDT += (iLengthParametr + strATmp.length ());
        iLengthParametr = AddParametr (iPosParametrDT, "tm_end", "", 5);
        
        strATmp.free ();
        strATmp = " ";
        m_strADescriptionTemplate.append (strATmp);
        
        iPosParametrDT += (iLengthParametr + 1);
        iLengthParametr = AddParametr (iPosParametrDT, "dt_end", "", 10);
    }
    else
    {
    }
    
    return 0;
}

int HOnceLineTemplate::AddParametr (int iPos, HString strAParametrType, HString strAParametrValue, int iLen)
{
    HString strATmp;
    
    if (iLen == 0)
        // 'iLen == -1', если КОНЕЦ строкИ
        iLen = GetParametrLength (iPos, m_strADescriptionTemplate);
    
    if (iPos < m_strADescriptionTemplate.length ())
    {
        if (! (MB_AT (m_strADescriptionTemplate, iPos) == '{'))
        {
            m_strADescriptionTemplate.cut (iPos, iLen);
            strATmp.format ("{#%i=L%i}", m_arstrAParametrType.GetSize (), iLen);
            m_strADescriptionTemplate.insert (iPos, strATmp);
            
            m_arstrAParametrType.Add (strAParametrType);
            //if (! strAParametrValue.IsEmpty ())
                m_arstrAParametrValue.Add (strAParametrValue);
        }
        else
        {
            iPos += 2; //ПереПРЫГНем через '{' и '#'
            iLen = 0;
            //НЕОБХодимо определить НОМЕР параметра (номер ПОСЛЕ '#' и ДО НЕ ЦИФРы)
            while ((! (MB_AT (m_strADescriptionTemplate, iPos + iLen) == '#')) && (! (MB_AT (m_strADescriptionTemplate, iPos + iLen) == '=')))
                iLen ++;

            strATmp = m_strADescriptionTemplate.mid (iPos, iLen);

            m_arstrAParametrType.GetAt (atoi (MB_HSTRING (strATmp))).appendChar (',');
            m_arstrAParametrType [atoi (MB_HSTRING (strATmp))].append (strAParametrType);
            //if (! strAParametrValue.IsEmpty ())
                m_arstrAParametrValue.GetAt (atoi (MB_HSTRING (strATmp))).appendChar (',');
                m_arstrAParametrValue [atoi (MB_HSTRING (strATmp))].append (strAParametrValue);
        }
    }
    else
    {//Длина параметра НЕ М.Б. == 0 ??? Либо добавляем НОВЫЙ в КОНЕЦ строкИ
        strATmp.format ("{#%i=L%i}", m_arstrAParametrType.GetSize (), 10);
        m_strADescriptionTemplate.insert (m_strADescriptionTemplate.length (), strATmp);

        m_arstrAParametrType.Add (strAParametrType);
        //if (! strAParametrValue.IsEmpty ())
            m_arstrAParametrValue.Add (strAParametrValue);
    }

    iLen = strATmp.length ();
    strATmp.free ();
    return iLen;
}

//Возврат СТРОКИ-ЦИФРы ЗАЧЕМ ??? - УБЕЖДАЕМСЯ, что ТАКой параметр ЕСТЬ и ВОЗВРАЩАЕМ его ТИП
HString HOnceLineTemplate::GetParametrType (int iNum)
{
    HString strAParametr;
    int iPos = 0;
    if (m_arstrAParametrType.GetSize ())
    {
        while ((iPos < m_strADescriptionTemplate.length ()) && (iPos > -1))
        {
            while ((! (MB_AT (m_strADescriptionTemplate, iPos) == '{')) && (iPos < m_strADescriptionTemplate.length ()))
                iPos ++;

            if (! (iPos < m_strADescriptionTemplate.length ()))
                iPos = -1;
            else
            {
                iPos ++;
                while ((! (MB_AT (m_strADescriptionTemplate, iPos) == '#')) && (iPos < m_strADescriptionTemplate.length ()))
                    iPos ++;
                    
                if (! (iPos < m_strADescriptionTemplate.length ()))
                    iPos = -1;
                else
                {
                    iPos ++;
                    while ((! (MB_AT (m_strADescriptionTemplate, iPos) == '#')) && (! (MB_AT (m_strADescriptionTemplate, iPos) == '=')) && (iPos < m_strADescriptionTemplate.length ()))
                        strAParametr.appendChar (MB_AT (m_strADescriptionTemplate, iPos ++));
                        
                    if (atoi (MB_HSTRING (strAParametr)) == iNum)
                    {
                        //Можно было ограничиться ЭТОЙ строкой
                        strAParametr = m_arstrAParametrType.GetAt (iNum);
                        break;
                    }
                    
                    strAParametr.free ();
                }
            }
        }
        
        if ((iPos < m_strADescriptionTemplate.length ()) && (iPos > -1))
            ; //Успешное выполнение
        else
            ; //Параметр  выполнение
    }
    else
        iPos = -1;
        
    return strAParametr;
}

HString HOnceLineTemplate::GetParametrTypeOfIdentifier (int iPos)
{
    HString strARes;
    
    if (iPos < 0)
#ifdef PAR_TORNADO
        strARes = TORNADO_SIGNAL
#endif
        ;
    else
    {
        iPos += 12;
            
        switch (MB_AT (m_strADescriptionTemplate, iPos))
        {
            case 0xCA:
            case 0x8A:
            case 0xEA:
            case 0xAA:
            case 0x4B:
            case 0x6B:
#ifdef PAR_ENERG
                strARes = ENERG_CHANNEL
#endif
                ;
                break;
            case 0xC3:
            case 0x83:
            case 0xE3:
            case 0xA3:
#ifdef PAR_ENERG
                strARes = ENERG_GROUP
#endif
                ;
                break;
            default:
                ;
        }
    }
    
    return strARes;
}

HString HOnceLineTemplate::FindParametrValue (int iPos)
{
    iPos += 12;
    
    HString strARes;
    
    char ch = MB_AT (m_strADescriptionTemplate, ++ iPos);
    if ((ch < 48) || (ch > 57))
    {
        while ((ch < 48) || (ch > 57))
        {//Должны нить НАЙТИ ЦИФРУ !!!
            ch = MB_AT (m_strADescriptionTemplate, ++ iPos);
        }
    }
    
    ch = MB_AT (m_strADescriptionTemplate, iPos);
    while ((ch > 47) && (ch < 58))
    {
        strARes.appendChar (ch);
        ch = MB_AT (m_strADescriptionTemplate, ++ iPos);
    }
        
    return strARes;
}

int HOnceLineTemplate::FindIdentifierParametrType (int iPos)
{
    int j;
    char ch, chPrev, chNext;
    //Цикл по символам каждой строки (интересны только с 13-го длиной 46 символов
    //for (j = (12 + (iPos - 12)); j < 12 + 47; j ++)
    for (j = 12 + iPos; j < 12 + 47; j ++)
    {
        chPrev = MB_AT (m_strADescriptionTemplate, j - 1);
        ch = MB_AT (m_strADescriptionTemplate, j);
        chNext = MB_AT (m_strADescriptionTemplate, j + 1);
        if ((ch == 0xca) ||
            (ch == 0x8a) ||
            (ch == 0xea) ||
            (ch == 0xaa) ||
            (ch == 0xc3) ||
            (ch == 0x83) ||
            (ch == 0xe3) ||
            (ch == 0xa3) ||
            (ch == 0x4b) ||
            (ch == 0x6b)
            )
            if ((((chNext > 47) && (chNext < 58)) || (chNext == 46)) && //ЦИФРА или '.'
                (! (chNext == 45)) && //Не == '-'
                //(m_strADescriptionTemplate.GetAt (j - 1) != -111) &&
                (! ((chPrev) == 145)) && //Не == 'ТГ'
                (chNext > 0) &&
                (chPrev > 0))
                return j - 12;
    }

    return -1;
}

#ifdef PTR_DB
int HOnceLineTemplate::SetParametrsType (HString &strAVal)
{
    HString strARes;
    int p = -1;
    int iPosParametrType = 0;
    
    
    if (strAVal.length ())
    {
        p = 0;
        strARes = strAVal.tokenize (";", iPosParametrType);
        while (iPosParametrType > -1)
        {
            if (m_arstrAParametrType.Add (strARes) < 0)
            {
                p = -1;
                break;
            }
            
            strARes = strAVal.tokenize (";", iPosParametrType);
            p ++;
        }
    }
    
    if (p < 0)
        return -1; //Ошибка ???
    else
        return 0;
}

int HOnceLineTemplate::SetParametrsValue (HString &strAVal)
{
    HString strARes;
    int с = -1;
    int iPosParametrType = 0;
    
    
    if (strAVal.length ())
    {
        с = 0;
        strARes = strAVal.tokenize (";", iPosParametrType);
        while (iPosParametrType > -1)
        {
            if (m_arstrAParametrValue.Add (strARes) < 0) {
                с = -1;
                break;
            }
			else
				;
            
            strARes = strAVal.tokenize (";", iPosParametrType);
            с ++;
        }
    }
    
    if (с < 0)
        return -1; //Ошибка ???
    else
        return 0;
}
#endif

HString HOnceLineTemplate::GetParametrTypeDetail (int iNum, int iNumDetail)
{
    HString strARes;
    int iPosParametrType = 0;
    
    if (iNum < m_arstrAParametrType.GetSize ())
    {
        strARes = m_arstrAParametrType [iNum].tokenize (",", iPosParametrType);
        while ((iNumDetail > 0) && (iPosParametrType > -1))
        {
            strARes = m_arstrAParametrType [iNum].tokenize (",", iPosParametrType);
            iNumDetail --;
        }
    }
    
    return strARes;
}

HString HOnceLineTemplate::GetParametrValueDetail (int iNum, int iNumDetail)
{
    HString strARes;
    int iPosParametrValue = 0;
    
    if (iNum < m_arstrAParametrValue.GetSize ())
    {
        strARes = m_arstrAParametrValue [iNum].tokenize (",", iPosParametrValue);
        while ((iNumDetail > 0) && (iPosParametrValue > -1))
        {
            strARes = m_arstrAParametrValue [iNum].tokenize (",", iPosParametrValue);
            iNumDetail --;
        }
    }
    
    return strARes;
}

HString HOnceLineTemplate::strAPosition ()
{
    return m_strAPosition;
}

HString HOnceLineTemplate::strAPlace ()
{
    return m_strAPlace;
}