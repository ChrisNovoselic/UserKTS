
// PseudoKTSView.cpp : реализаци€ класса CPseudoKTSView
//

#include "stdafx.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позвол€ет совместно использовать код документа в данным проекте.
#ifndef SHARED_HANDLERS
#include "PseudoKTS.h"
#endif

#include "PseudoKTSDoc.h"
#include "PseudoKTSView.h"

// CPseudoKTSView

IMPLEMENT_DYNCREATE(CPseudoKTSView, CEditView)

BEGIN_MESSAGE_MAP(CPseudoKTSView, CEditView)
	// —тандартные команды печати
	ON_COMMAND(ID_FILE_PRINT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

// создание/уничтожение CPseudoKTSView

CPseudoKTSView::CPseudoKTSView () {
	// TODO: добавьте код создани€

}

CPseudoKTSView::~CPseudoKTSView () {
}

BOOL CPseudoKTSView::PreCreateWindow (CREATESTRUCT& cs) {
	// TODO: изменить класс Window или стили посредством изменени€
	//  CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow (cs);
	cs.style &= ~(ES_AUTOHSCROLL | WS_HSCROLL);	// –азрешить перенос слов

	return bPreCreated;
}


// печать CPseudoKTSView

BOOL CPseudoKTSView::OnPreparePrinting(CPrintInfo* pInfo) {
	// подготовка CEditView по умолчанию
	return CEditView::OnPreparePrinting (pInfo);
}

void CPseudoKTSView::OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo) {
	// Ќачало печати CEditView по умолчанию
	CEditView::OnBeginPrinting (pDC, pInfo);
}

void CPseudoKTSView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) {
	// «авершение печати CEditView по умолчанию
	CEditView::OnEndPrinting (pDC, pInfo);
}


// диагностика CPseudoKTSView

#ifdef _DEBUG
void CPseudoKTSView::AssertValid () const {
	CEditView::AssertValid ();
}

void CPseudoKTSView::Dump (CDumpContext& dc) const {
	CEditView::Dump (dc);
}

CPseudoKTSDoc* CPseudoKTSView::GetDocument() const {
// встроена неотлаженна€ верси€
	ASSERT(m_pDocument->IsKindOf (RUNTIME_CLASS(CPseudoKTSDoc)));
	return (CPseudoKTSDoc* )m_pDocument;
}
#endif //_DEBUG


// обработчики сообщений CPseudoKTSView
