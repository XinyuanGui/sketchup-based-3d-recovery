
// StereoMappingView.cpp : CStereoMappingView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "StereoMapping.h"
#endif

#include "StereoMappingDoc.h"
#include "StereoMappingView.h"
#include "MeasureDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStereoMappingView

IMPLEMENT_DYNCREATE(CStereoMappingView, CView)

BEGIN_MESSAGE_MAP(CStereoMappingView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CStereoMappingView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_32771, &CStereoMappingView::On32771)
END_MESSAGE_MAP()

// CStereoMappingView ����/����

CStereoMappingView::CStereoMappingView()
{
	// TODO: �ڴ˴���ӹ������

}

CStereoMappingView::~CStereoMappingView()
{
}

BOOL CStereoMappingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CStereoMappingView ����

void CStereoMappingView::OnDraw(CDC* /*pDC*/)
{
	CStereoMappingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CStereoMappingView ��ӡ


void CStereoMappingView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CStereoMappingView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CStereoMappingView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CStereoMappingView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CStereoMappingView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CStereoMappingView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CStereoMappingView ���

#ifdef _DEBUG
void CStereoMappingView::AssertValid() const
{
	CView::AssertValid();
}

void CStereoMappingView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CStereoMappingDoc* CStereoMappingView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStereoMappingDoc)));
	return (CStereoMappingDoc*)m_pDocument;
}
#endif //_DEBUG


// CStereoMappingView ��Ϣ�������


void CStereoMappingView::On32771()
{
	// TODO: Add your command handler code here
	CMeasureDialog dlg;
	dlg.DoModal();
	//dlg.ShowWindow(SW_SHOWNORMAL);IDD_MEASURE_DIALOG
}
