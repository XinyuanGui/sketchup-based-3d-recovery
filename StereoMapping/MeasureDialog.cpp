// MeasureDialog.cpp : implementation file
//

#include "stdafx.h"
#include "StereoMapping.h"
#include "MeasureDialog.h"
#include "afxdialogex.h"
#include "InitSettingDlg.h"
#include "charaTrans.h"
#include "FxImage.h"
#include "location.h"
#include "singleImageView.h"

#include "polylineSing.h"


#ifdef _FILETYPE_EXPR
CString fileOpenType=_T("�����ļ�(*.xpr)|*.xpr|�����ļ�(*.xml)|*.xml|ģ���ļ�(*.mod)|*.mod||");
#else
CString fileOpenType=_T("�����ļ�(*.svp)|*.svp|�����ļ�(*.xml)|*.xml|ģ���ļ�(*.mod)|*.mod||");
#endif

// CMeasureDialog dialog

IMPLEMENT_DYNAMIC(CMeasureDialog, CDialog)

CMeasureDialog::CMeasureDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMeasureDialog::IDD, pParent)
{
	m_bstrip=false;
	m_bPolygon=false;
	m_tool=PAN;

	m_bIsCtrlInit = false;

	m_nMultiViewImgIndex = 0;

}

CMeasureDialog::~CMeasureDialog()
{
	//  m_stereoView.DestroyWindow();
}

void CMeasureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BLOCK_TREE, m_blockTree);
	//DDX_Control(pDX, IDC_LIST1, m_ListCtl);
	DDX_Control(pDX, IDC_TAB1, m_TabImgView);
}


BEGIN_MESSAGE_MAP(CMeasureDialog, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, &CMeasureDialog::OnFileOpen)
	ON_NOTIFY(TVN_SELCHANGED, IDC_BLOCK_TREE, &CMeasureDialog::OnTvnSelchangedBlockTree)
	ON_COMMAND(ID_CREATE_POLYGON, &CMeasureDialog::OnCreatePolygon)
	ON_UPDATE_COMMAND_UI(ID_CREATE_POLYGON, &CMeasureDialog::OnUpdateCreatePolygon)
	ON_WM_INITMENUPOPUP()

//ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMeasureDialog::OnTcnSelchangeTab1) 
ON_WM_NCLBUTTONDOWN()
ON_WM_MOVE()
//ON_COMMAND(ID_32772, &CMeasureDialog::OnHorizonDatum)
ON_COMMAND(ID_32772, &CMeasureDialog::OnHorizonDatum)
ON_COMMAND(ID_Menu, &CMeasureDialog::OnEnlargeGrid)
ON_COMMAND(ID_32776, &CMeasureDialog::OnDenseGrid)
ON_COMMAND(ID_32773, &CMeasureDialog::OnVerticalDatum)
ON_COMMAND(ID_32774, &CMeasureDialog::OnRandomDatum)
ON_COMMAND(ID_32777, &CMeasureDialog::OnMultiViewImg)
ON_COMMAND(ID_32778, &CMeasureDialog::OnRefreshMultiViewImg)
ON_COMMAND(ID_32779, &CMeasureDialog::OnPolyline)
ON_COMMAND(ID_32782, &CMeasureDialog::OnCircle)
ON_COMMAND(ID_32815, &CMeasureDialog::On32815)
ON_COMMAND(ID_32816, &CMeasureDialog::OnCloseDatumGrid)
ON_COMMAND(ID_32780, &CMeasureDialog::OnPolygon)
ON_COMMAND(ID_32781, &CMeasureDialog::OnArc)
ON_COMMAND(ID_32784, &CMeasureDialog::OnVerticalSwitch)
ON_COMMAND(ID_32817, &CMeasureDialog::OnShowToolState)
ON_UPDATE_COMMAND_UI(ID_32780, &CMeasureDialog::OnUpdatePolygon)
END_MESSAGE_MAP()


// CMeasureDialog message handlers


BOOL CMeasureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	//�趨���ⴰ�ڵ���ʼλ��
	this->SetWindowPos(NULL, 20, 40,650 , 
		700, SWP_SHOWWINDOW); 

	CRect rect;
	GetClientRect(&rect);


// 	CRect rec(0,0,0,0);
	//m_stereoView.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rec,this,3131);

	//20160608 �Ȳ���ʾ����
// 	m_viewer.Create(CStereoViewDlg::IDD,this);
// 	m_viewer.ShowWindow(SW_SHOW);


	//���������Ի��򴫵ݵ��ָ�������

	//20160609 m_TabImgView��ѡ��ؼ� 
	//ԭ����������ʾӰ���dialog ���ڽ���ʾӰ���dialog�ŵ�ѡ���
	m_MainSplitterPane.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN|SS_VERT,
		this, &m_blockTree, &m_TabImgView, IDC_VERT_SPLITTER,
		rect,100,200);


	//���ؼ����񱻷ŵ��ָ��������ˣ���ʾ�Ĳ����������Խ��
	m_blockTree.SetParent(m_TabImgView.GetParent()); //������ʾ�������ˣ����ǻ�����


	m_hRoot = m_blockTree.InsertItem(_T("����"));

	//ѡ����ѡ��20160609
	m_TabImgView.InsertItem(0,_T("������ͼ"));
	m_TabImgView.InsertItem(1,_T("������ͼ"));

	//��ʾӰ��Ի��򴴽�20160609
	m_childDlg.Create(IDD_ChildDlg, &m_TabImgView); //����
	m_viewer.Create(IDD_DIALOG_STEREOVIEW, &m_TabImgView); //����

	//����Ӱ����ʾ�Ի���20160609
	CRect tabRect;

	m_TabImgView.GetWindowRect(&tabRect); //��Ļ����

	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 25;
	tabRect.bottom -= 1;

	m_childDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), 
		tabRect.Height(), SWP_SHOWWINDOW); 
	m_viewer.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), 
		tabRect.Height(), SWP_HIDEWINDOW); 

	m_bIsCtrlInit = true;

	//ѡ����� Ӱ����ʾ�Ի��� ������� 20160609





	m_isFileOpen = false;

	SetViewPosition();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CMeasureDialog::SetViewPosition()
{
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	CRect rect;
	GetClientRect(&rect);
	//int nTreeWidth=300;
	//if (m_blockTree.m_hWnd!=NULL)
	//{
	//	m_blockTree.MoveWindow(0,0,nTreeWidth,rect.Height());
	//}
	//if (m_stereoView.m_hWnd!=NULL)
	//{
	//	m_stereoView.MoveWindow(nTreeWidth,0,rect.Width()-nTreeWidth,rect.Height());
	//}

	if (m_MainSplitterPane.m_hWnd)
	{
		m_MainSplitterPane.MoveWindow(&rect);
	}


	Invalidate(FALSE);
}

void CMeasureDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	SetViewPosition();

	//��Ҫ����ʾӰ��Ĵ���ͬʱ����
	ReSizeImg();

}


int CMeasureDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here


	return 0;
}


void CMeasureDialog::OnFileOpen()
{
	// TODO: Add your command handler code here
	CFileDialog fdlg(TRUE, NULL, NULL, 0, fileOpenType,  this);
	fdlg.m_ofn.lpstrFile = new TCHAR[1024*1024];   
	memset(fdlg.m_ofn.lpstrFile,0,1024*1024);  
	fdlg.m_ofn.nMaxFile =1024*1024; 
	if(fdlg.DoModal() == IDOK)
	{
		m_proPath = fdlg.GetPathName();
		//CAerialBlock block;
		CString strPathName;

		m_block.Reset();
		//�ַ�ת��
		unsigned int size=WideCharToMultiByte(CP_OEMCP,NULL,m_proPath.GetBuffer(),-1,NULL,0,NULL,false);
		char* pPathName=new char[size];
		bool re=WCharToMByte(m_proPath.GetBuffer(),pPathName,size);
		if(!re)
			MessageBox(_T("�ַ�ת������"));
		//��ȡ�����ļ�
		bool bSuc = m_block.LoadFromFile(pPathName);
		if (!bSuc)
		{
			TRACE("�ļ���ʧ�ܣ�", pPathName);
		}
		else
		{
			//��tree�ؼ���ʾ������Ϣ
			AddBlockToTree(&m_block);
			m_isFileOpen = true;
		}	

		delete []pPathName;
		pPathName=NULL;
	}
	delete [] fdlg.m_ofn.lpstrFile;

	//���´�������Ҫ���򿪹��̲���������洢·����ʱʹ��
	//CInitSettingDlg initDlg;
	//if (initDlg.DoModal() == IDOK)
	//{
	//	//CAerialBlock block;
	//	CString strPathName = initDlg.m_ImagesPath;
	//	m_proPath=initDlg.m_ImagesPath;
	//	m_MapsPath = initDlg.m_MapsPath;


	//	m_block.Reset();
	//	//�ַ�ת��
	//	unsigned int size=WideCharToMultiByte(CP_OEMCP,NULL,strPathName.GetBuffer(),-1,NULL,0,NULL,false);
	//	char* pPathName=new char[size];
	//	bool re=WCharToMByte(strPathName.GetBuffer(),pPathName,size);
	//	if(!re)
	//		MessageBox(_T("�ַ�ת������"));
	//	//��ȡ�����ļ�
	//	bool bSuc = m_block.LoadFromFile(pPathName);
	//	if (!bSuc)
	//	{
	//		TRACE("�ļ���ʧ�ܣ�", pPathName);
	//	}
	//	else
	//	{
	//		//��tree�ؼ���ʾ������Ϣ
	//		AddBlockToTree(&m_block);
	//	}	

	//	delete []pPathName;
	//	pPathName=NULL;
	//}
}

void CMeasureDialog::AddBlockToTree(CAerialBlock* block)
{
	int nstrips = block->GetNumStrip();

	/*HTREEITEMm_hRoot = m_blockTree.InsertItem(_T("����"));*/ 
	HTREEITEM hRoot = m_blockTree.GetRootItem();
	if(m_blockTree.ItemHasChildren(hRoot))
	{
		m_blockTree.DeleteItem(hRoot);
		hRoot = m_blockTree.InsertItem(_T("����"));
	}
	for (int i = 0; i < nstrips; i++)
	{
		CAerialStrip *pStrip = block->GetStrip(i);
		CString sroot = _T("���� ");
		sroot += pStrip->GetID();
		HTREEITEM hStrip = m_blockTree.InsertItem(sroot, hRoot);

		int nphotos = pStrip->GetNumPhoto();
		for (int j = 0; j < nphotos; j++)
		{
			CAerialPhoto* pPhoto = pStrip->GetPhoto(j);
			CAerialBlock* pBlock = pPhoto->GetBlock();
			WORD data = (WORD)pBlock->GetPhotoIndex(pPhoto);
			//�ַ�ת��
			const char* pUrl=pPhoto->GetURL();
			unsigned int size=MultiByteToWideChar(CP_ACP,0,pUrl, -1, NULL, 0);
			TCHAR* pWUrl=new TCHAR[size];
			MByteToWChar(pUrl,pWUrl,size);

			HTREEITEM hPhoto = m_blockTree.InsertItem(pWUrl,hStrip);
			delete[]pWUrl;
			pWUrl=NULL;

			m_blockTree.SetItemData(hPhoto, data);

			const CCamera* lpCam = pPhoto->GetCamera();
			if(lpCam->GetSensorType() == CCamera::SENSOR_CCD)
			{
				int  width, height;
				CCCDCamera* lpCCDCam = (CCCDCamera*)lpCam;			
				lpCCDCam->GetSensorSizeInPixel(width, height);
				pPhoto->SetDimension(width, height);
			}
			else
			{
				CFxImage sxImage;
				sxImage.Open(pPhoto->GetURL());
				pPhoto->SetDimension(sxImage.GetWidth(), sxImage.GetHeight());
				sxImage.Close();
			}			
		}

	}
	m_blockTree.Expand(hRoot,TVE_EXPAND);
	m_viewer.CleanAllContent();
}

//��������ؼ�
void CMeasureDialog::OnTvnSelchangedBlockTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CString sleft, sright, title;
	CAerialPhoto* pleftPhoto =NULL;
	CAerialPhoto* prightPhoto = NULL;
	if (!m_bstrip) 
	{
		HTREEITEM hSel = m_blockTree.GetSelectedItem();
		HTREEITEM hNext = m_blockTree.GetNextSiblingItem(hSel);

		if (m_blockTree.ItemHasChildren(hSel) || hSel == m_hRoot) return;
		sleft = m_blockTree.GetItemText(hSel);

		if (hNext == NULL)
			return;
		else
			sright = m_blockTree.GetItemText(hNext);

		int lImage=int(m_blockTree.GetItemData(hSel));
		int rImage=int(m_blockTree.GetItemData(hNext));
		//pleftPhoto = m_block.GetPhoto(int(m_blockTree.GetItemData(hSel)));
		//prightPhoto = m_block.GetPhoto(int(m_blockTree.GetItemData(hNext)));
		pleftPhoto = m_block.GetPhoto(lImage);
		prightPhoto = m_block.GetPhoto(rImage);
	}
	else
	{
		//HTREEITEM hSel = m_blockTree.GetSelectedItem();
		//if (m_blockTree.ItemHasChildren(hSel) || hSel == m_hRoot) return;
		//sleft = m_blockTree.GetItemText(hSel);
		//pleftPhoto = m_block.GetPhoto(int(m_blockTree.GetItemData(hSel)));
		//m_viewer.FindNearestPhotoInCamera(pleftPhoto, prightPhoto);

		//if (prightPhoto == NULL)
		//	return;
		//sright = prightPhoto->GetURL();
	}

// 	GetWindowText(title);
// 	title = (_T("SVSMPModeler��") + sleft + _T(" --- ") + sright);
// 	SetWindowText(title);

	//m_viewer.SetTools(PAN);


	m_viewer.SetAerialPhoto(pleftPhoto, prightPhoto); //�������������ʾӰ��
	m_viewer.LoadImages();
	m_viewer.SetFocus();


	//m_viewer.m_MapsPath = m_MapsPath;

	//m_viewer.SetTools(m_viewer.GetTool());

	*pResult = 0;
}


void CMeasureDialog::OnCreatePolygon()
{
	// TODO: Add your command handler code here
	//0715 ע�͵� ԭ���ָ�Ĵ���
// 	UpdateData(TRUE);
// 	m_tool=(POLYGON == m_tool?PAN:POLYGON);
// 	if (POLYGON==m_tool)
// 	{
// 		m_bPolygon=TRUE;
// 	}
// 	else
// 		m_bPolygon=FALSE;
// 	m_viewer.SetTool(m_tool);
// 	UpdateData(FALSE);
}


void CMeasureDialog::OnUpdateCreatePolygon(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	//0715 ע�͵� ԭ���ָ�Ĵ���
	//pCmdUI->SetCheck(m_bPolygon);
}


void CMeasureDialog::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here
	if(!bSysMenu && pPopupMenu)
	{
		CCmdUI cmdUI;
		cmdUI.m_pOther = NULL;
		cmdUI.m_pMenu = pPopupMenu;
		cmdUI.m_pSubMenu = NULL;

		UINT count = pPopupMenu->GetMenuItemCount();
		cmdUI.m_nIndexMax = count;
		for(UINT i=0; i<count; i++)
		{
			UINT nID = pPopupMenu->GetMenuItemID(i);
			if(-1 == nID || 0 == nID)
			{
				continue;
			}
			cmdUI.m_nID = nID;
			cmdUI.m_nIndex = i;
			cmdUI.DoUpdate(this, FALSE);
		}
	}

}




//20160612 ������ʾӰ����
void CMeasureDialog::ReSizeImg(void)
{
	CRect tabRect;

	if (m_bIsCtrlInit)
	{
		m_TabImgView.GetWindowRect(&tabRect); //��Ļ����
		tabRect.left += 1;
		tabRect.right -= 1;
		tabRect.top += 25;
		tabRect.bottom -= 1;	

		if (m_TabImgView.GetCurSel() == 0) //����
		{	
			/*m_childDlg.MoveWindow(tabRect,FALSE);*/
			HDWP hdwp = BeginDeferWindowPos(1);
			HWND hwnd = m_childDlg.m_hWnd;
			hdwp = DeferWindowPos( hdwp,hwnd , NULL, tabRect.left, tabRect.top,
				tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			EndDeferWindowPos( hdwp );
		}
		else if (m_TabImgView.GetCurSel() == 1) //����
		{
			/*m_viewer.MoveWindow(tabRect,FALSE);*/
			HDWP hdwp = BeginDeferWindowPos(1);
			HWND hwnd = m_viewer.m_hWnd;
			hdwp = DeferWindowPos( hdwp,hwnd , NULL, tabRect.left, tabRect.top,
				tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			EndDeferWindowPos( hdwp );
		}

	}


}



/*
void CMeasureDialog::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
		CRect tabRect;    // ��ǩ�ؼ��ͻ�����Rect  
		// ��ȡ��ǩ�ؼ��ͻ���Rect����������������ʺϷ��ñ�ǩҳ  
		m_TabImgView.GetWindowRect(&tabRect);  
		tabRect.left += 1;  
		tabRect.right -= 1;  
		tabRect.top += 25;  
		tabRect.bottom -= 1;   

		int temp = m_TabImgView.GetCurSel();

		switch (m_TabImgView.GetCurSel())
		{	
		case 0: //ѡ����
			m_childDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), 
				tabRect.Height(), SWP_SHOWWINDOW); 
			m_viewer.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), 
				tabRect.Height(), SWP_HIDEWINDOW); 
			break;

		case 1: //ѡ�������������
			m_childDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), 
				tabRect.Height(), SWP_HIDEWINDOW); 
			m_viewer.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), 
				tabRect.Height(), SWP_SHOWWINDOW); 
			break;

		default:
			break;
	
		}


	*pResult = 0;
}

*/



//201606����˴��룬ʹ�ñ��ֳ���ɫ����Ч�������岢��ʵʱ���ƣ�������
void CMeasureDialog::OnNcLButtonDown(UINT nHitTest, CPoint point) //20160613 �ı䴰���Сʱ��ˢ��
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	// 1����ѯ��ǰϵͳ���϶���ʾ�������ݡ����� m_bDragFullWindow�Զ���ı�������֮
	SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bDragFullWindow, NULL);  

	// 2�������Ҫ�޸����ã�����ÿ�ν���CDialog::OnNcLButtonDownĬ�ϴ���֮ǰ�޸�
	if(m_bDragFullWindow)
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, NULL);

	// 3��Ĭ�ϴ���ϵͳ���Զ��������
	CDialog::OnNcLButtonDown(nHitTest, point);

	// 4��Ĭ�ϴ�����Ϻ󣬻�ԭϵͳ����
	if(m_bDragFullWindow)
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, NULL);

}

//�ƶ����壬����ѡ���ǰ��ʾ��
void CMeasureDialog::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	// TODO: �ڴ˴������Ϣ����������
	CRect tabRect;

	if (m_bIsCtrlInit)
	{
		m_TabImgView.GetWindowRect(&tabRect); //��Ļ����
		tabRect.left += 1;
		tabRect.right -= 1;
		tabRect.top += 25;
		tabRect.bottom -= 1;	

		if (m_TabImgView.GetCurSel() == 0) //����
		{	
			/*m_childDlg.MoveWindow(tabRect,FALSE);*/
			HDWP hdwp = BeginDeferWindowPos(1);
			HWND hwnd = m_childDlg.m_hWnd;
			hdwp = DeferWindowPos( hdwp,hwnd , NULL, tabRect.left, tabRect.top,
				tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			EndDeferWindowPos( hdwp );
		}
		else if (m_TabImgView.GetCurSel() == 1) //����
		{
			/*m_viewer.MoveWindow(tabRect,FALSE);*/
			HDWP hdwp = BeginDeferWindowPos(1);
			HWND hwnd = m_viewer.m_hWnd;
			hdwp = DeferWindowPos( hdwp,hwnd , NULL, tabRect.left, tabRect.top,
				tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			EndDeferWindowPos( hdwp );
		}

	}


}


//ˮƽ��׼��˵��¼�0627
void CMeasureDialog::OnHorizonDatum()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		MessageBox(L"���л���˫�����������ȷ����׼��!");
	}
	else //˫��
	{
		m_viewer.ChangeMode2Datum(0);
	}
	

}

//����������
void CMeasureDialog::OnEnlargeGrid()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		
	}
	else //˫��
	{
		m_viewer.ChangeGridSize(true);
	}
}

//��С�������
void CMeasureDialog::OnDenseGrid()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		
	}
	else //˫��
	{
		m_viewer.ChangeGridSize(false);
	}
}

//�˵����������׼
void CMeasureDialog::OnVerticalDatum()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		MessageBox(L"���л���˫�����������ȷ����׼��!");
	}
	else //˫��
	{
		m_viewer.ChangeMode2Datum(1);
	}
}

//���⳯��Ļ�׼��
void CMeasureDialog::OnRandomDatum()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		MessageBox(L"���л���˫�����������ȷ����׼��!");
	}
	else //˫��
	{
		m_viewer.ChangeMode2Datum(2);
	}
}

//�л�����һ��Ӱ�����ŵ���ͬ����
void CMeasureDialog::OnMultiViewImg()
{
	// TODO: �ڴ���������������

	//˫�����
	if (m_TabImgView.GetCurSel() == 1)
	{
		return;
	}

	if (m_vMVStripIndex.size() == 0)
	{
		return;
	}

	//��ɾ����һ�ŵ���Ļ�׼����
	m_childDlg.getView()->CloseCurrentDatum();

	int i = m_vMVStripIndex[m_nMultiViewImgIndex];
	int j = m_vMVPhotoIndex[m_nMultiViewImgIndex];
	LocationType pt;
	pt.mX = m_vMVPxlCent[m_nMultiViewImgIndex] .m_X;
	pt.mY = m_vMVPxlCent[m_nMultiViewImgIndex].m_Y;

	m_nMultiViewImgIndex = m_nMultiViewImgIndex + 1; //ʹ��ż�1

	if (m_nMultiViewImgIndex == m_vMVStripIndex.size()) //���Խ�磬������0
	{
		m_nMultiViewImgIndex = 0;
	}

	CAerialStrip* pstrip = m_block.GetStrip(i);
	CAerialPhoto* pPhoto = pstrip->GetPhoto(j);
	const char* pcPhotoPath = pPhoto->GetURL();
	CString cstrPhotoPath;
	int len = strlen(pcPhotoPath);
	TCHAR* c1 = (TCHAR*)malloc(sizeof(TCHAR)*len);
	MultiByteToWideChar( CP_ACP , 0 , pcPhotoPath , len+1 , c1 , len+1);
	cstrPhotoPath.Format(L"%s",c1);


	m_childDlg.setAerialPhoto(pPhoto); //��Ҫ����һ�䣬��Ȼ�޷����м���
	m_childDlg.getView()->LoadImages(cstrPhotoPath);

	//���ŵ���Ӧλ�ã�������loadӰ�������ŵ���Ӧλ��
	m_childDlg.getView()->ZoomToPoint(pt, 50);
	m_childDlg.SetFocus();


}

//20160629 ��ȡ����Ӱ�� ���ǲ˵����������
void CMeasureDialog::GetMultiViewImg(double X, double Y, double Z)
{
	if (!m_isFileOpen)
	{
		MessageBox(_T("���ȴ�ͼ���ļ�"));
		return;
	}

	int nstrips = m_block.GetNumStrip();

	//���ԭ�ж���Ӱ��
	m_vMVStripIndex.clear();
	m_vMVPhotoIndex.clear();

	for (int i = 0; i < nstrips;i++)
	{
		CAerialStrip* pstrip = m_block.GetStrip(i);
		int nphotos = pstrip->GetNumPhoto();
		for (int j = 0; j < nphotos; j++)
		{
			int imageWidth;
			int imageHeight;
			CAerialPhoto* pPhoto = pstrip->GetPhoto(j);
			pPhoto->GetDimension(imageWidth, imageHeight);

			CFxImage img;
			BOOL bOpen = img.Open(pPhoto->GetURL());
			if (!bOpen)
			{
				continue;
			}
			img.Close();

			CPoint2D tempPixelPt;
			pPhoto->ObjectCS2PixelCS(X, Y, Z, tempPixelPt.m_X, tempPixelPt.m_Y);
			if (tempPixelPt.m_X >= imageWidth || tempPixelPt.m_Y > imageHeight ||
				tempPixelPt.m_X < 0 || tempPixelPt.m_Y < 0)
			{
				continue;
			}
			else
			{ //Ѱ�ҵ��˶���Ӱ�񣬺ÿ��� (^_^)
				m_vMVStripIndex.push_back(i);
				m_vMVPhotoIndex.push_back(j);
				m_vMVPxlCent.push_back(tempPixelPt);
			}
		}
	}

	m_nMultiViewImgIndex = 0;

}

//ˢ�¶���Ӱ�����������
void CMeasureDialog::OnRefreshMultiViewImg()
{
	// TODO: �ڴ���������������

	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		return;
	}

	m_viewer.GetSpaceCentPnt(m_MVHomoPnt);
	GetMultiViewImg(m_MVHomoPnt.m_X, m_MVHomoPnt.m_Y, m_MVHomoPnt.m_Z);
	
	//����һ���¼�����
	OnMultiViewImg();
}


//��ʾ��׼���������ү�ģ�������û��Ӻã���˫�񶼵��ܿ���
void CMeasureDialog::On32815()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		m_childDlg.getView()->ShowCurrentDatum(m_viewer.m_vGridStartCoor,m_viewer.m_vGridEndCoor);
	}
	else //˫�����
	{
		m_viewer.ShowCurrentDatum();

	}

}

//�رջ�׼���������˫�񶼿���
void CMeasureDialog::OnCloseDatumGrid()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		m_childDlg.getView()->CloseCurrentDatum();
	}
	else //˫�����
	{
		m_viewer.CloseCurrentDatum();

	}	

}


//////////////////////////////////////////////////////////////////////////
//����ͼ��

//���ƶ����0711
void CMeasureDialog::OnPolyline()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		//ÿ�β�֮ǰ��ȡһ��ƽ����Ϣ
		double normX, normY, normZ, X,Y,Z;

		//����true�ſ����ڵ����л�ͼ
		if(m_viewer.GetDatumPlane(normX,normY,normZ,X,Y,Z))
		{
			m_childDlg.getView()->setPlane(normX,normY,normZ,X,Y,Z);
			m_childDlg.getView()->changeTool(POLYLINESing);
		}
		else
		{
			MessageBox(L"������˫����������û�׼��");
		}

	}
	else //�����������ҲҪ���Ի���
	{
		m_viewer.SetTool(POLYLINE);
	}
}

//0715 �ڵ�˫��������Բ�Σ�ע���Ȼ����
void CMeasureDialog::OnCircle()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		//ÿ�β�֮ǰ��ȡһ��ƽ����Ϣ
		double normX, normY, normZ, X,Y,Z;

		//����true�ſ����ڵ����л�ͼ
		if(m_viewer.GetDatumPlane(normX,normY,normZ,X,Y,Z))
		{

			if (m_viewer.m_DatumType != HORIZON)
			{
				MessageBox(L"�ٺ٣���ʱֻ֧��ˮƽ��׼");
				return;
			}

			m_childDlg.getView()->setPlane(normX,normY,normZ,X,Y,Z);
			m_childDlg.getView()->changeTool(CIRCLESing);

		}
		else
		{
			MessageBox(L"������˫����������û�׼��");
		}

	}
	else //�����������ҲҪ���Ի���
	{
		m_viewer.SetTool(CIRCLE);
	}



}

//0715 �ڵ�˫�������ƶ����
void CMeasureDialog::OnPolygon()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		//ÿ�β�֮ǰ��ȡһ��ƽ����Ϣ
		double normX, normY, normZ, X,Y,Z;

		//����true�ſ����ڵ����л�ͼ
		if(m_viewer.GetDatumPlane(normX,normY,normZ,X,Y,Z))
		{
			m_childDlg.getView()->setPlane(normX,normY,normZ,X,Y,Z);
			m_childDlg.getView()->switchPolygonMode();
		}
		else
		{
			MessageBox(L"������˫����������û�׼��");
		}

	}
	else //�����������ҲҪ���Ի���
	{
		m_viewer.switchPolygonMode();
	}


}

//0715 �ڵ�˫�������ƻ���
void CMeasureDialog::OnArc()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		//ÿ�β�֮ǰ��ȡһ��ƽ����Ϣ
		double normX, normY, normZ, X,Y,Z;

		//����true�ſ����ڵ����л�ͼ
		if(m_viewer.GetDatumPlane(normX,normY,normZ,X,Y,Z))
		{
			if (m_viewer.m_DatumType != HORIZON)
			{
				MessageBox(L"�ٺ٣���ʱֻ֧��ˮƽ��׼");
				return;
			}


			m_childDlg.getView()->setPlane(normX,normY,normZ,X,Y,Z);
			m_childDlg.getView()->changeTool(ARCSing);
		}
		else
		{
			MessageBox(L"������˫����������û�׼��");
		}

	}
	else //�����������ҲҪ���Ի���
	{
		m_viewer.SetTool(ARC);
	}

}


//0715 ֱ�ǻ��Ŀ���
void CMeasureDialog::OnVerticalSwitch()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		m_childDlg.getView()->changeVertical();//�л���ֱģʽ
	}
	else //�������
	{
		m_viewer.switchVerticalMode();
	}

}


void CMeasureDialog::OnShowToolState()
{
	// TODO: �ڴ���������������
	//�������
	if (m_TabImgView.GetCurSel() == 0)
	{
		m_childDlg.getView()->getInformation();
	}
	else //�������
	{
		m_viewer.getInformation();
	}
}

//0716  �����Ƿ��濪�صĶԹ�Ч�����л���˫��ʱҪ��
void CMeasureDialog::OnUpdatePolygon(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	if (m_TabImgView.GetCurSel() == 0) //����
	{
		if (m_childDlg.getView()->m_polygonMode == POLYGONONSing)
		{
			pCmdUI->SetCheck(true);
		}
		else
		{
			pCmdUI->SetCheck(false);
		}
	}
	else //����
	{
		if (m_viewer.m_polygonMode == POLYGONON)
		{
			pCmdUI->SetCheck(true);
		}
		else
		{
			pCmdUI->SetCheck(false);
		}
	}
}
