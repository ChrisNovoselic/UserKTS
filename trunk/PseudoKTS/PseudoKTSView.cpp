
// PseudoKTSView.cpp : ���������� ������ CPseudoKTSView
//

#include "stdafx.h"
// SHARED_HANDLERS ����� ���������� � ������������ �������� ��������� ���������� ������� ATL, �������
// � ������; ��������� ��������� ������������ ��� ��������� � ������ �������.
#ifndef SHARED_HANDLERS
#include "PseudoKTS.h"
#endif

#include "PseudoKTSDoc.h"
#include "PseudoKTSView.h"

// CPseudoKTSView

IMPLEMENT_DYNCREATE(CPseudoKTSView, CEditView)

BEGIN_MESSAGE_MAP(CPseudoKTSView, CEditView)
	// ����������� ������� ������
	ON_COMMAND(ID_FILE_PRINT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

// ��������/����������� CPseudoKTSView

CPseudoKTSView::CPseudoKTSView () {
	// TODO: �������� ��� ��������

}

CPseudoKTSView::~CPseudoKTSView () {
}

BOOL CPseudoKTSView::PreCreateWindow (CREATESTRUCT& cs) {
	// TODO: �������� ����� Window ��� ����� ����������� ���������
	//  CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow (cs);
	cs.style &= ~(ES_AUTOHSCROLL | WS_HSCROLL);	// ��������� ������� ����

	return bPreCreated;
}


// ������ CPseudoKTSView

BOOL CPseudoKTSView::OnPreparePrinting(CPrintInfo* pInfo) {
	// ���������� CEditView �� ���������
	return CEditView::OnPreparePrinting (pInfo);
}

void CPseudoKTSView::OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo) {
	// ������ ������ CEditView �� ���������
	CEditView::OnBeginPrinting (pDC, pInfo);
}

void CPseudoKTSView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) {
	// ���������� ������ CEditView �� ���������
	CEditView::OnEndPrinting (pDC, pInfo);
}


// ����������� CPseudoKTSView

#ifdef _DEBUG
void CPseudoKTSView::AssertValid () const {
	CEditView::AssertValid ();
}

void CPseudoKTSView::Dump (CDumpContext& dc) const {
	CEditView::Dump (dc);
}

CPseudoKTSDoc* CPseudoKTSView::GetDocument() const {
// �������� ������������ ������
	ASSERT(m_pDocument->IsKindOf (RUNTIME_CLASS(CPseudoKTSDoc)));
	return (CPseudoKTSDoc* )m_pDocument;
}
#endif //_DEBUG


// ����������� ��������� CPseudoKTSView
