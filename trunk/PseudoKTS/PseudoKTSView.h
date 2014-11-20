
// PseudoKTSView.h : интерфейс класса CPseudoKTSView
//

#pragma once


class CPseudoKTSView : public CEditView
{
protected: // создать только из сериализации
	CPseudoKTSView();
	DECLARE_DYNCREATE(CPseudoKTSView)

// Атрибуты
public:
	CPseudoKTSDoc* GetDocument() const;

// Операции
public:

// Переопределение
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Реализация
public:
	virtual ~CPseudoKTSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // отладочная версия в PseudoKTSView.cpp
inline CPseudoKTSDoc* CPseudoKTSView::GetDocument() const
   { return reinterpret_cast<CPseudoKTSDoc*>(m_pDocument); }
#endif

