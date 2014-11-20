
// PseudoKTSView.h : ��������� ������ CPseudoKTSView
//

#pragma once


class CPseudoKTSView : public CEditView
{
protected: // ������� ������ �� ������������
	CPseudoKTSView();
	DECLARE_DYNCREATE(CPseudoKTSView)

// ��������
public:
	CPseudoKTSDoc* GetDocument() const;

// ��������
public:

// ���������������
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ����������
public:
	virtual ~CPseudoKTSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ��������� ������� ����� ���������
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ���������� ������ � PseudoKTSView.cpp
inline CPseudoKTSDoc* CPseudoKTSView::GetDocument() const
   { return reinterpret_cast<CPseudoKTSDoc*>(m_pDocument); }
#endif

