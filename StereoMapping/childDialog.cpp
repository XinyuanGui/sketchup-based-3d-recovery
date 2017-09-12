// childDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
//#include "DocView2.h"
//#include "docView_dll.h"
#include "childDialog.h"
#include "afxdialogex.h"
#include "singleImageView.h"

// childDialog �Ի���

IMPLEMENT_DYNAMIC(childDialog, CDialogEx)

childDialog::childDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(childDialog::IDD, pParent),
	m_singleView(NULL),m_pAerialPhoto(NULL),
	m_pwnd(NULL)
{

}

childDialog::~childDialog()
{
}

void childDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(childDialog, CDialogEx)
	ON_WM_SIZE()
//	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// childDialog ��Ϣ�������


BOOL childDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CRect rectWndClient;
	GetClientRect(&rectWndClient);
	m_pwnd= new CFrameWnd();
	m_pwnd->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rectWndClient,this);
	CRuntimeClass *pViewRuntimeClass=RUNTIME_CLASS(singleImageView);
	//m_singleView=(m_singleView*)pViewRuntimeClass->CreateObject();
	m_singleView=(singleImageView*)pViewRuntimeClass->CreateObject();
	m_singleView->Create(NULL,NULL,WS_VISIBLE|WS_CHILD, rectWndClient, m_pwnd,123,NULL);
	m_singleView->OnInitialUpdate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void childDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if(m_singleView)
	{
		m_pwnd->SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER);
		m_singleView->SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER);
	}
}

void childDialog::loadImage()
{
	if (!(m_pAerialPhoto&&m_singleView))
	{
		return;
	}
	const char* c;
	c = m_pAerialPhoto->GetURL();
	CString s;
	int len = strlen(c);
	TCHAR* c1 = (TCHAR*)malloc(sizeof(TCHAR)*len);
	MultiByteToWideChar( CP_ACP , 0 , c , len+1 , c1 , len+1);
	s.Format(L"%s",c1);

	m_singleView->LoadImages(s);
}

afx_msg void childDialog::setAerialPhoto(CAerialPhoto* photo)
{
	m_pAerialPhoto=photo;
	m_singleView->setAerialPhoto(photo);
}

//void childDialog::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//	if (m_pAerialPhoto&&m_singleView)
//	{
//		double x,y;
//		m_singleView->TranslateScreenToWorld(double(point.x),double(point.y),x,y);
//		int c=10;
//	}
//	CDialogEx::OnLButtonDown(nFlags, point);
//}
