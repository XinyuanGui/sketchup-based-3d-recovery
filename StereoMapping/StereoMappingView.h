
// StereoMappingView.h : CStereoMappingView ��Ľӿ�
//

#pragma once


class CStereoMappingView : public CView
{
protected: // �������л�����
	CStereoMappingView();
	DECLARE_DYNCREATE(CStereoMappingView)

// ����
public:
	CStereoMappingDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CStereoMappingView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void On32771();
};

#ifndef _DEBUG  // StereoMappingView.cpp �еĵ��԰汾
inline CStereoMappingDoc* CStereoMappingView::GetDocument() const
   { return reinterpret_cast<CStereoMappingDoc*>(m_pDocument); }
#endif

