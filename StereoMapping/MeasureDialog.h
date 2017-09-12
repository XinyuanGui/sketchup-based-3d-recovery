#pragma once

#include "GLee.h" //�����һ�䣬��� error C1189: #error :  gl.h included before glee.h

#include "SSplitter.h"
#include "StereoView.h"
#include "StereoViewDlg.h"
#include "AerialBase.h"
#include "afxcmn.h"

#include "childDialog.h"

// CMeasureDialog dialog

class CMeasureDialog : public CDialog
{
	DECLARE_DYNAMIC(CMeasureDialog)

public:
	CMeasureDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMeasureDialog();

// Dialog Data
	enum { IDD = IDD_MEASURE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


public:
	CSSplitter m_MainSplitterPane;
	//CSSplitter m_LeftSplitterPane;
	CTreeCtrl m_blockTree;
	//CListCtrl m_ListCtl;
	CStereoView m_stereoView;//������ʾ����
	bool m_bstrip; ///�Ƿ��պ����Զ������������

	CStereoViewDlg m_viewer;//20160612 ԭ��Ϊ˽��


	//��׼�����
	


private:
	CString m_MapsPath;
	CString m_proPath;
	HTREEITEM m_hRoot;


	CAerialBlock m_block;

	//��ģ����״̬��ʶ
	BOOL m_bPolygon;//�Ƿ��ڶ���ν�ģ״̬
	TOOLS m_tool;

public:
	void AddBlockToTree(CAerialBlock* block);


private:
	void SetViewPosition();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();
	//���οؼ���굥����Ӧ����
	afx_msg void OnTvnSelchangedBlockTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCreatePolygon();
	afx_msg void OnUpdateCreatePolygon(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);//�ý�ģ����ѡ����Ӧ�Ĳ˵�ǰ���
//	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	

//////////////////////////////////////////////////////////////////////////
//20160608 Li

	CTabCtrl m_TabImgView; //ѡ�

	//�����ǵ����measureDlg�еĺ�������������ʾ�Ի������

	bool m_isFileOpen; //��OnInit�����г�ʼ��

	childDialog m_childDlg; //������ʾ�ĶԻ��򣬷ŵ�ѡ����л���

	//������ʾӰ�����С�õ�
	void ReSizeImg(void);  
	POINT old; 
	bool m_bIsCtrlInit;
	//OnNcLButtonDown��Ϣ��ʹ��
	bool m_bDragFullWindow; //�ж�ϵͳ���ã��ı䴰���Сʱ�Ƿ�ʵʱ��ʾ�������ݣ�Ϊ�˼�����˸

	//������ά�������ȡ����Ӱ��
	void GetMultiViewImg(double X, double Y, double Z);

	//20160628 ����Ӱ��
	CPoint3D m_MVHomoPnt; //����Ӱ���ͬ����
	std::vector<CPoint2D> m_vMVPxlCent; //�������꣬Ӱ����ʾ����������
	std::vector<int> m_vMVStripIndex; //�������
	std::vector<int> m_vMVPhotoIndex; //������Ӱ�����
	int m_nMultiViewImgIndex; //Ӱ����vector�е����

//////////////////////////////////////////////////////////////////////////


//	afx_msg void OnNMClickTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMove(int x, int y);
	//afx_msg void OnHorizonDatum();
	afx_msg void OnHorizonDatum();
	afx_msg void OnEnlargeGrid();
	afx_msg void OnDenseGrid();
	afx_msg void OnVerticalDatum();
	afx_msg void OnRandomDatum();
	afx_msg void OnMultiViewImg();
	afx_msg void OnRefreshMultiViewImg();
	afx_msg void OnPolyline();
	afx_msg void OnCircle();
	afx_msg void On32815();
	afx_msg void OnCloseDatumGrid();
	afx_msg void OnPolygon();
	afx_msg void OnArc();
	afx_msg void OnVerticalSwitch();
	afx_msg void OnShowToolState();
	afx_msg void OnUpdatePolygon(CCmdUI *pCmdUI);
};
