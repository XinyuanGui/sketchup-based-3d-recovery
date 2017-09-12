// StereoViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StereoMapping.h"
#include "StereoViewDlg.h"
#include "afxdialogex.h"
#include "dpoint3.h"
#include "trig.h"
//#include "entity.h"
#include "polyline.h"
#include "polygonBuilding.h"
#include "arc.h"
#include "circle.h"
#include "polygon.h"

// CStereoViewDlg dialog

IMPLEMENT_DYNAMIC(CStereoViewDlg, CDialog)

CStereoViewDlg::CStereoViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStereoViewDlg::IDD, pParent)
{
	m_iniScale=1.0;

	//////////////////////////////////////////////////////////////////////////
	//��׼��
	m_nImgScale = 0; //�������ż�������¥�϶�������ű�����û�㶮
	m_bShowDatum = false;
	m_nClickMode = 0;

	m_dGridSize = 1;
	m_dGridRange = 50;

	//ͬʱΪ0��û�л�ȡ��׼��
	m_dNormX = 0;
	m_dNormY = 0;
	m_dNormZ = 0;
	m_dX = 0;
	m_dY = 0;
	m_dZ = 0;

	//��������ʶ�Ƿ������˻�׼��
	m_DatumType = NODATUM;

	//////////////////////////////////////////////////////////////////////////


	isLoadImage=false;
	m_panStep=0.5;//�ֶ����ò���
	m_panMulti=1.0;
	m_LViewer.EnableParallel(true);
	m_RViewer.EnableParallel(true);

	m_LLineLayer.m_bEnabled = FALSE;
	m_RLineLayer.m_bEnabled = FALSE;
	m_pLLine=NULL;
	m_pRLine=NULL;
	m_editedEntity=NULL;
	m_isDrawingBuilding=false;
	m_lineState=ROOF;
	m_editedBuilding=NULL;
	m_sideState=ROOFPOINT;
	m_procedure = DRAW;
	m_verticalMode = OFF;
	m_modifyState = NEAUTRAL;

	//0712 ��
	m_polygonMode=POLYGONOFF;
	//�Ķ����� 0712

	m_flowNum=-1;
}

CStereoViewDlg::~CStereoViewDlg()
{
	//0712 A Gui ��
	for (int i=0;i<m_entities.size();i++)
	{
		delete m_entities[i];
	}
	//end �޸Ľ���
}

void CStereoViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStereoViewDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()
//	ON_WM_MOVE()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	//ON_COMMAND(ID_32772, &CStereoViewDlg::OnHorizonDatum)
END_MESSAGE_MAP()


// CStereoViewDlg message handlers


void CStereoViewDlg::SetAerialPhoto(CAerialPhoto* lpLeftPhoto, CAerialPhoto* lpRightPhoto)
{
	m_lpAerialPhoto[0] = lpLeftPhoto;
	m_lpAerialPhoto[1] = lpRightPhoto;
	m_AerialGeoModel.LoadModel(lpLeftPhoto, lpRightPhoto);
}

void CStereoViewDlg::LoadImages()
{
	double lcetrX, lcetrY, rcetrX, rcetrY , x, y, z = 0.0;
	DPoint3 axis;

	m_LViewer.SetGeoModel(&m_AerialGeoModel, true);
	m_RViewer.SetGeoModel(&m_AerialGeoModel, false);

	m_LViewer.SetImagePathFile(m_lpAerialPhoto[0]->GetURL());
	m_RViewer.SetImagePathFile(m_lpAerialPhoto[1]->GetURL());

	m_Render.EnableGL(true);

	//��ȡ�ⷽλԪ��
	const CAerialPhoto::EXTORI* eOri =  m_lpAerialPhoto[0]->GetExtOri();


	//�ⷽλ��Ԫ��
	lcetrX = eOri->POS[0]; lcetrY = eOri->POS[1]; z =0.0; 
	eOri = m_lpAerialPhoto[1]->GetExtOri();
	rcetrX = eOri->POS[0]; rcetrY = eOri->POS[1];

	m_LViewer.SetCenter(lcetrX, lcetrY);
	m_RViewer.SetCenter(rcetrX,rcetrY);
	m_LViewer.SetZ(z);
	m_RViewer.SetZ(z);

	//axis.Set(rcetrX-lcetrX, rcetrY-lcetrY, 0.0);
	//����
	axis.x = rcetrX-lcetrX;
	axis.y = rcetrY-lcetrY;
	axis.z = 0.0;

	DPoint3 cosProd = axis^DPoint3(1.0, 0.0, 0.0);
	double ang = acos((axis*Point3::XAxis)/Length(axis));
	if(cosProd.z < 0) 
		ang = -ang;
	ang = (double)RadToDeg(ang);
	//ang = -178.443;
	//˳ʱ����תΪ��
	m_LViewer.RotateTo(ang);
	m_RViewer.RotateTo(ang);

	z = m_lpAerialPhoto[0]->GetTerrainHeight();
	m_lpAerialPhoto[0]->ImageRayIntersect(0.0,0.0, z, lcetrX, lcetrY);

	m_lpAerialPhoto[0]->ObjectCS2PixelCS(lcetrX, lcetrY, z, x, y);
	m_LViewer.PanToImage(x, y);
	m_lpAerialPhoto[1]->ObjectCS2PixelCS(lcetrX, lcetrY,  z, x, y);
	m_RViewer.PanToImage(x, y);

	//����Ӱ�����ű���
	m_LViewer.ZoomTo(m_iniScale);
	m_RViewer.ZoomTo(m_iniScale);
	m_iniScale=1.0;//��Ҫ���ں��ڼ�¼Ӱ�����ű��������Զ� �л��������ʱ���ָñ���

	//��¼���ż����˶��ٴ�
	m_nImgScale = 0;

	m_LViewer.SetLevelScale(1.3f);
	m_RViewer.SetLevelScale(1.3f);

	m_LViewer.RemoveAllLayers();
	m_RViewer.RemoveAllLayers();
	m_LViewer.AddLayer(&m_LLineLayer);
	m_RViewer.AddLayer(&m_RLineLayer);

	m_LLineLayer.clear();
	m_RLineLayer.clear();


	m_Render.EnableGL(false);
	isLoadImage=true;
	Invalidate(FALSE);
}


void CStereoViewDlg::CleanAllContent( void )
{
	m_Render.EnableGL(true);

	m_LViewer.SetImageIO(NULL);
	m_RViewer.SetImageIO(NULL);

	m_LViewer.RemoveAllLayers();
	m_RViewer.RemoveAllLayers();

	m_LViewer.LimitTiles(0);
	m_RViewer.LimitTiles(0);

	m_Render.EnableGL(false);
	isLoadImage = false;
	OnPaint();
}

void CStereoViewDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	m_LLineLayer.ClearObject();
	m_RLineLayer.ClearObject();


	m_Render.EnableGL(true);
	::glEnable(GL_TEXTURE_2D);

	m_LViewer.LimitTiles(0);
	m_RViewer.LimitTiles(0);

	m_Render.EnableGL(false);
}


void CStereoViewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	m_Render.EnableGL(true);
	::glDrawBuffer(GL_BACK);
	::glClearColor(0.0f,0.0f,0.0f,1.0f);
	::glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	//::glDisable(GL_LIGHTING);
	if(isLoadImage)
	{
		m_LViewer.Render();
		m_RViewer.Render();
	}
	m_Render.PostRender();
	m_Render.EnableGL(false);
}


int CStereoViewDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	bool bstereo= false;


	m_Render.Create(m_hWnd, 32, true);
	bstereo = m_Render.SupportStereo();

	m_LViewer.UseIndex(0);
	m_RViewer.UseIndex(1);

	m_Render.EnableGL(true);
	::glEnable(GL_TEXTURE_2D);

	m_LViewer.EnableQuadBuffersStereo(bstereo);
	m_RViewer.EnableQuadBuffersStereo(bstereo);

	m_LViewer.EnableCursor(true);
	m_RViewer.EnableCursor(true);

	m_LViewer.SetCursorSize(8.0f);
	m_RViewer.SetCursorSize(8.0f);

	m_LViewer.SetCursorWidth(1.0f);
	m_RViewer.SetCursorWidth(1.0f);

	m_LViewer.SetCursorColor(RGB(255, 255, 0));
	m_RViewer.SetCursorColor(RGB(255, 255, 0));


	m_Render.EnableGL(false);

	return 0;
}


void CStereoViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.bottom = cy;
	rect.right = cx;
	m_Render.EnableGL(true);
	m_Render.SetViewPort(this->m_hWnd,rect,true);
	m_LViewer.SetViewPort(rect);
	m_RViewer.SetViewPort(rect);
	m_LViewer.PanByClient(0,0);
	m_RViewer.PanByClient(0,0);
	m_Render.EnableGL(false);
}


BOOL CStereoViewDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if(m_LViewer.GetImageIO() == NULL || m_RViewer.GetImageIO() == NULL)		
	{
		SetCursor(::LoadCursor(NULL, IDC_ARROW));

		return TRUE;
	}
	else if(nHitTest==HTCLIENT)
		SetCursor(NULL);
	return TRUE;
	//return CDialog::OnSetCursor(pWnd, nHitTest, message);
}


BOOL CStereoViewDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	double lx = 0.0, ly = 0.0, rx = 0.0, ry = 0.0;
	//double x = 0.0, y = 0.0;
	double l_img[2], r_img[2];
	m_RViewer.PanClient(0.0, 0.0, m_panMulti*m_panStep*(zDelta/WHEEL_DELTA), 0.0);
	ScreenToClient(&pt);
	TranslateScreenToLocal(pt, lx, ly, rx, ry);
	m_LViewer.LocalToPixel(lx, ly, l_img[0], l_img[1]);
	m_LViewer.SetCursorPos(l_img[0], l_img[1]);
	m_RViewer.LocalToPixel(rx, ry, r_img[0], r_img[1]);
	m_RViewer.SetCursorPos(r_img[0], r_img[1]);

	Invalidate(FALSE);

	return TRUE;
	//return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


//void CStereoViewDlg::OnMove(int x, int y)
//{
//	CDialog::OnMove(x, y);
//
//	// TODO: Add your message handler code here
//
//}


void CStereoViewDlg::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnMButtonUp(nFlags, point);
}


void CStereoViewDlg::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_MouseLastPoint=point;


	CDialog::OnMButtonDown(nFlags, point);
}


void CStereoViewDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	switch(m_procedure)
	{
//////////////////////////////////////////////////////////////////////////
//��׼��0626
	case DATUM:
		switch(m_DatumType)
		{
		case HORIZON: //ˮƽ��׼
			ShowHorDatumGrid(point);
			break;
		case VERTICAL: //��������
			switch(m_nClickMode)
			{
			case 0: //δ���
				GetSpaceCoor(point,m_datumCent);
				m_nClickMode = m_nClickMode + 1;
				break;
			case 1: //��һ��
				ShowVerDatumGrid(point);
				m_nClickMode = 0;
				break;
			default:
				break;
			}
			break;
		case RANDOM: //��������
			switch(m_nClickMode)
			{
			case 0: //δ���
				GetSpaceCoor(point,m_datumCent);
				m_nClickMode = m_nClickMode + 1;
				break;
			case 1: //��һ��
				ShowVerDatumGrid(point);
				m_nClickMode = m_nClickMode + 1;
				break;
			case 2: //�����
				ShowRanDatumGrid(point);
				m_nClickMode = 0;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;
//��׼�����
//////////////////////////////////////////////////////////////////////////
	case DRAW:
		break;
	case MODIFY:
		switch (m_modifyState)
		{
		case NEAUTRAL:
			m_modifyState = DRAGGING;
			m_before = point;
			break;
		case MOVEREF:
			m_new = point;
			moveref(m_before, m_new);
			m_modifyState = NEAUTRAL;
			m_before = m_new;
			break;
		default:
			break;
		}
		break;
	}

	CDialog::OnLButtonDown(nFlags, point);
}


void CStereoViewDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	AddPolygonBuilding(4, point);
	Invalidate(FALSE);

	CDialog::OnRButtonDown(nFlags, point);
}

void CStereoViewDlg::TranslateScreenToLocal( CPoint p, double& lwx, double& lwy, double& rwx, double& rwy )
{
	m_LViewer.ClientToLocal(double(p.x), double(p.y), lwx, lwy);
	m_RViewer.ClientToLocal(double(p.x), double(p.y), rwx, rwy);
}


void CStereoViewDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	double lx=0.0, ly=0.0, rx=0.0, ry=0.0;
	double lImgx=0.0, lImgy=0.0, rImgx=0.0, rImgy=0.0;
	if (m_LViewer.GetImageIO() != NULL || m_RViewer.GetImageIO() != NULL)
	{
		TranslateScreenToLocal(point, lx, ly, rx, ry);
		m_LViewer.LocalToPixel(lx, ly, lImgx, lImgy);
		m_RViewer.LocalToPixel(rx, ry, rImgx, rImgy);
		m_LViewer.SetCursorPos(lImgx, lImgy);
		m_RViewer.SetCursorPos(rImgx, rImgy);

		if(nFlags == (MK_MBUTTON | MK_SHIFT) || nFlags == MK_MBUTTON)
		{
			int dx = point.x- m_MouseLastPoint.x; 

			if(m_AerialGeoModel.IsInitialized())
			{
				if((GetKeyState(VK_SHIFT) & 0x8000))//shift is not pressed ,  all views are paned, or just right view.
				{
					m_RViewer.PanClient(m_MouseLastPoint.x, m_MouseLastPoint.y, point.x, m_MouseLastPoint.y);
				}
				else
				{
					m_LViewer.PanClient(m_MouseLastPoint.x, m_MouseLastPoint.y, point.x, point.y);
					m_RViewer.PanClient(m_MouseLastPoint.x, m_MouseLastPoint.y, point.x, point.y);
				}
			}
		}
	}

	switch (m_procedure)
	{
//////////////////////////////////////////////////////////////////////////
		//20160627 ��׼������޸�
	case DATUM:
		switch(m_DatumType)
		{
// 		case HORIZON: //ˮƽ��׼
// 			ShowHorDatumGrid(point);
// 			break;
		case VERTICAL: //��������
			switch(m_nClickMode)
			{
			case 1: //��һ��
				ShowVerDatumGrid(point);
				break;
			default:
				break;
			}
			break;
		case RANDOM: //��������
			switch(m_nClickMode)
			{
			case 1: //��һ��
				ShowVerDatumGrid(point);
				break;
			case 2: //�����
				//ShowRanDatumGrid(point);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;
//////////////////////////////////////////////////////////////////////////
	case DRAW:
		AddPolygonBuilding(3, point);
		break;
	case MODIFY:
		switch (m_modifyState)
		{
		case DRAGGING:
			m_new = point;
			if (pointDist(m_before.x,m_before.y,m_new.x,m_new.y)>3)
			{
				double dist;
				CPoint ref=getNearestRef(m_before, dist);
				if (dist<DraggingThresh)
				{
					m_before = ref;
					m_modifyState = MOVEREF;
				}
				else
				{
					m_modifyState = NEAUTRAL;
				}
			}
			break;
		case MOVEREF:
			m_new = point;
			moveref(m_before, m_new);
			m_before = m_new;
			break;
		case NEAUTRAL:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	Invalidate(FALSE);
	m_MouseLastPoint = point;
	CDialog::OnMouseMove(nFlags, point);
}


void CStereoViewDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	double zoom=1.0;
	CPoint pt;
	double lx =0.0, ly = 0.0, rx = 0.0, ry = 0.0;
	double cx = 0.0, cy = 0.0;
	double px[2], py[2];
	double x, y;

	if (nChar == 'Z'||nChar == 'X')
	{
		m_LViewer.GetCursorPos(px[0], py[0]);
		m_RViewer.GetCursorPos(px[1], py[1]);

		m_LViewer.PixelToLocal(px[0], py[0], lx, ly);
		m_RViewer.PixelToLocal(px[1], py[1], rx, ry);
		double pre_ix,pre_iy;//����δ����֮ǰ���������
		pre_ix=px[0];
		pre_iy=py[0];

		//Ӱ��Ŵ�\��С
		if (nChar == 'Z')
		{
			zoom+=0.1;
			m_nImgScale+=1;
		}
		else
		{
			zoom-=0.1;
			m_nImgScale-=1;
		}

		//ԭ��zoom֮��
		m_LViewer.LocalToClient(lx, ly, x, y);
		pt.SetPoint(int(x), int(y));

		m_LViewer.Zoom(zoom);
		m_RViewer.Zoom(zoom);

		double tempcx,tempcy;
		m_LViewer.PixelToLocal(pre_ix,pre_iy,tempcx,tempcy);
		m_LViewer.LocalToClient(tempcx,tempcy,x,y);
		m_LViewer.PanClient(x,y,pt.x,pt.y);
		m_RViewer.PanClient(x,y,pt.x,pt.y);

		TranslateScreenToLocal(pt, lx, ly, rx, ry);

		m_LViewer.LocalToPixel(lx, ly, px[0], py[0]);
		m_RViewer.LocalToPixel(rx, ry, px[1], py[1]);


		m_LViewer.SetCursorPos(px[0], py[0]);
		m_RViewer.SetCursorPos(px[1], py[1]);
		Invalidate(FALSE);

	}

	/*

	if (nChar=='P')
	{
		SetTool(POLYGON);
	}
	if (nChar=='A')
	{
		SetTool(ARC);
	}
	if (nChar=='C')
	{
		SetTool(CIRCLE);
	}
	if (nChar=='M')
	{
		switchProcedure();
	}
	if (nChar=='V')
	{
		switchVerticalMode();
	}
	if (nChar=='B')
	{
		if (m_flowNum!=-1&&m_flowNum!=0)
		{
			MessageBox(_T("������ɵ�ǰ����"));
		}
		else
		{
			m_isDrawingBuilding = true;
		}
	}
	*/

/*
	if (nChar == 'G')
	{
		if (m_entities.size()>0)
		{
			double X, Y, Z;
			m_entities[0]->outputFirstPoint(X, Y, Z, this);
			int c = 20;
		}
	}*/

	//if(GetKeyState(VK_SHIFT) & 0x8000)
	//{
	//	m_panMulti=10.0;
	//}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CStereoViewDlg::SetTool( TOOLS tool )
{
	//TODO:
	//�л���ģ����֮ǰ��Ҫ�����ϴν�ģδ��ɵ��ƺ��������ں�����

	//0715 Li ��Ӷ���ι���״̬���û�����ֱ�ӵ��ù��濪��switch
// 	switch (tool)
// 	{
// 	case POLYLINE:
// 		if (m_polygonMode == POLYGONON && (m_flowNum == -1 || m_flowNum == 0))
// 		{
// 			switchPolygonMode();
// 		}
// 		else if (m_polygonMode == POLYGONOFF)
// 		{
// 
// 		}
// 		else
// 		{
// 			MessageBox(L"��δ��ɻ��ƣ��������л����ߣ�");
// 		}
// 		break;
// 
// 	case POLYGON: //�������棬�����Ϳ����޸�tool
// 		//δ�������� �� ����û��ʼ���� �� ������ϣ� �ſ��Կ���
// 		if (m_polygonMode == POLYGONOFF && (m_flowNum == -1 || m_flowNum == 0))
// 		{
// 			switchPolygonMode();
// 			tool = POLYLINE;
// 		}
// 		else if (m_polygonMode == POLYGONON)
// 		{
// 			tool = POLYLINE;
// 		}
// 		else
// 		{
// 			MessageBox(L"��δ��ɻ��ƣ��������л����ߣ�");
// 		}
// 		break;
// 
// 	}
	//�޸Ľ���

	//0712 ��
	if (CIRCLE==tool&&POLYGONON==m_polygonMode)
	{
		MessageBox(_T("polygonģʽ�������û�Բ"));
		return;
	}
	if (m_flowNum == -1)
	{
		m_flowNum = 0;
	}
	else if (m_flowNum == 2)
	{
		if (m_editedEntity)
		{

			if (!m_editedEntity->canEntityFinish())
			{
				MessageBox(_T("������ɻ滭"));
				return;
			}
			m_editedEntity->endDrawing();
			if (POLYGONON==m_polygonMode&&CIRCLE!=m_tool)
			{
				m_editedPolygon->calculateCloseLine();
			}

			Invalidate(FALSE);
		}
		m_flowNum = 1;
	}
	m_tool=tool;
	//�޸����

	//0716 �� ����Ȳ��׼�棬Ȼ����Ҫ������״̬�Ĺ���
	m_procedure = DRAW;

}


//0712 ע���˺����ǻ�������ͼ�εģ��������Ƕ���η���
void CStereoViewDlg::AddPolygonBuilding( UINT nFlags, CPoint point )
{
	double lx = 0.0, ly = 0.0, rx =0.0, ry = 0.0;
	CPoint3D lpt, rpt;
	//��Ļ����ת��Ϊ�ֲ�����
	TranslateScreenToLocal(point, lx, ly, rx, ry);

	lpt.m_X = lx;	lpt.m_Y = ly; lpt.m_Z = 0.0;
	rpt.m_X = rx; rpt.m_Y= ry; rpt.m_Z = 0.0;
	switch(m_procedure)
	{
	case DRAW:
		{
			switch(nFlags)
			{
			case 1://LButtonDown:��drawʱû���κβ���;
				break;


			case 2://LButtonUp
				switch(m_flowNum)
				{
				case 0:
					if (!m_pLLine)
					{
						m_pLLine = m_LLineLayer.AddLine();
					}
					if (!m_pRLine)
					{
						m_pRLine=m_RLineLayer.AddLine();
					}


			//0712 ��
					if (!m_editedEntity)
					{
						switch(m_tool) //ԭ��ΪPOLYGON��m_tool��Ҫ��ǰ�Ĺ���
						{
							//�������Ϊ polyline
						case POLYLINE: 
							/*if (POLYGONON==m_polygonMode)
							{
								m_editedEntity=new polyline(m_pLLine,m_pRLine,this,m_verticalMode);
							}
							else
							{
								m_editedEntity=new polyline(m_pLLine,m_pRLine,this,m_verticalMode);
							}*/
							m_editedEntity=new polyline(m_pLLine,m_pRLine,this,m_verticalMode);
							break;
						case ARC:
							/*if (POLYGONON==m_polygonMode)
							{
								m_editedEntity=new arc(m_pLLine,m_pRLine,this);
							}
							else
							{
								m_editedEntity=new arc(m_pLLine,m_pRLine,this);
							}*/
							m_editedEntity=new arc(m_pLLine,m_pRLine,this);
							break;
						case CIRCLE:
							m_editedEntity = new circle(m_pLLine, m_pRLine, this);
							break;
						default:
							break;
						}
					}
					if (POLYGONON==m_polygonMode&&CIRCLE!=m_tool)
					{
						CEditLine* pLCloseLine=m_LLineLayer.AddLine();
						CEditLine* pRCloseLine=m_RLineLayer.AddLine();
						m_editedPolygon=new polygon(this,pLCloseLine,pRCloseLine);
						m_editedPolygon->addEntity(m_editedEntity);
						m_entities.push_back(m_editedPolygon);
						m_editedEntity->leftButtonUp(lx,ly,rx,ry);
						(static_cast<entityAlone*>(m_editedEntity))->
							setLastPoint(&m_lastPt);
						m_flowNum=2;
					}
					else//��֮ǰdrawBuilding�����������˷�polygon��
					{
						if (m_isDrawingBuilding&&(!m_editedBuilding))
						{
							m_editedBuilding= new polygonBuilding(this);
							m_editedBuilding->addRoofEntity(m_editedEntity);
							m_entities.push_back(m_editedBuilding);
						}
						else
						{
							m_entities.push_back(m_editedEntity);
						}
						m_editedEntity->leftButtonUp(lx,ly,rx,ry);
						(static_cast<entityAlone*>(m_editedEntity))->
							setLastPoint(&m_lastPt);
						//editedEntity->setLastPoint(&m_lastPt);
						//m_information.setLastPoint(lx,ly,rx,ry);
						m_flowNum=2;
					}
					break;
					//�Ķ����� 0712

					//0712 ��
				case 1:
					m_pLLine = m_LLineLayer.AddLine();
					m_pRLine= m_RLineLayer.AddLine();
					switch(m_tool)
					{
					case POLYLINE:
						/*if (POLYGONON==m_polygonMode)
						{
							m_editedEntity=new polyline(m_pLLine,m_pRLine,this,
								&m_lastPt,m_verticalMode);
						}
						else
						{
							m_editedEntity=new polyline(m_pLLine,m_pRLine,this,
								&m_lastPt,m_verticalMode);
						}*/
						m_editedEntity=new polyline(m_pLLine,m_pRLine,this,
							&m_lastPt,m_verticalMode);
						break;
					case ARC:
						/*if (POLYGONON==m_polygonMode)
						{
							m_editedEntity=new arc(m_pLLine,m_pRLine,this,
								&m_lastPt,&m_firstPoint,true);
						}
						else
						{
							m_editedEntity=new arc(m_pLLine,m_pRLine,this,
								&m_lastPt);
						}*/
						m_editedEntity=new arc(m_pLLine,m_pRLine,this,
							&m_lastPt);
						break;
					case CIRCLE:
						m_editedEntity = new circle(m_pLLine, m_pRLine, this,
							&m_lastPt);
						break;
					default:
						break;
					}
					
					if (POLYGONON==m_polygonMode&&CIRCLE!=m_tool)
					{
						m_editedPolygon->addEntity(m_editedEntity);
						m_editedEntity->leftButtonUp(lx,ly,rx,ry);
						(static_cast<entityAlone*>(m_editedEntity))->
							setLastPoint(&m_lastPt);
						m_editedPolygon->calculateCloseLine();
						//m_editedPolygon->deleteClose();
						//m_information.setLastPoint(lx,ly,rx,ry);
						m_flowNum=2;
					}
					else
					{
						if (m_isDrawingBuilding)
						{
							m_editedBuilding->addRoofEntity(m_editedEntity);
						} 
						else
						{
							m_entities.push_back(m_editedEntity);
						}
						m_editedEntity->leftButtonUp(lx,ly,rx,ry);
						(static_cast<entityAlone*>(m_editedEntity))->
							setLastPoint(&m_lastPt);
						//m_information.setLastPoint(lx,ly,rx,ry);
						m_flowNum=2;
					}
					
					break;

				case 2:
					m_editedEntity->leftButtonUp(lx,ly,rx,ry);
					(static_cast<entityAlone*>(m_editedEntity))->
						setLastPoint(&m_lastPt);
					if (POLYGONON==m_polygonMode&&CIRCLE!=m_tool)
					{
						m_editedPolygon->calculateCloseLine();
					}
					//m_information.setLastPoint(lx,ly,rx,ry);
					break;
					//�Ķ����� 0712

				case 3:
					if (!(m_isDrawingBuilding&&m_editedBuilding))
					{
						m_flowNum=0;
						return;
					}
					else
					{
						CEditLine* leftLine=NULL;
						CEditLine* rightLine=NULL;
						switch(m_sideState)
						{
						case ROOFPOINT:
							leftLine=m_LLineLayer.AddLine();
							rightLine=m_RLineLayer.AddLine();
							if (m_editedBuilding->setRoofPoint(lx,ly,rx,ry,
								leftLine,rightLine))
							{
								m_sideState=BOTTOMPOINT;
							}
							break;
						case BOTTOMPOINT:
							m_editedBuilding->setBottomPoint(lx,ly,rx,ry);
							m_sideState = ROOFPOINT;
							break;
						}
					}
					break;
				}
				break;


			case 3://����ƶ�
				switch(m_flowNum)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					m_editedEntity->moveMouse(lx,ly,rx,ry);

					//0712 ��
					if (POLYGONON==m_polygonMode&&CIRCLE!=m_tool)
					{
						m_editedPolygon->calculateCloseLine();
					}
					//�Ķ����� 0712

					break;
				case 3:
					switch(m_sideState)
					{
					case ROOFPOINT:
						break;
					case BOTTOMPOINT:
						m_editedBuilding->setBottomPtReview(lx,ly,rx,ry);
						break;
					}
					break;
				}
				break;

			case 4://�Ҽ�;
				if (POLYGONON==m_polygonMode&&CIRCLE!=m_tool)
				{
					switch(m_flowNum)
					{
					case 1:
						m_editedPolygon->endDrawing();
						m_editedPolygon=NULL;
						m_editedEntity=NULL;
						m_pLLine = NULL;
						m_pRLine = NULL;
						m_flowNum=0;
						break;
					case 2:
						//0712 ��
						if (!m_editedEntity->canEntityFinish())
						{
							MessageBox(_T("����ɻ滭"));
							return;
						}
						//�޸����
						m_editedEntity->rightButtonDown(lx,ly,rx,ry);
						m_editedPolygon->endDrawing();
						m_editedPolygon=NULL;
						m_editedEntity=NULL;
						m_pLLine = NULL;
						m_pRLine = NULL;
						m_flowNum=0;
						break;
					default:
						break;
					}
				}
				else
				{
					switch(m_flowNum)
					{
					case 0:
						break;
					case 1:
						if (m_isDrawingBuilding)
						{
							m_editedBuilding->endDrawing();
							m_editedEntity=NULL;
							m_pLLine = NULL;
							m_pRLine = NULL;
							m_flowNum=3;
						}
						else
						{
							if (m_editedEntity)
							{
								m_editedEntity->endDrawing();
							}
							m_editedEntity=NULL;
							m_pLLine = NULL;
							m_pRLine = NULL;
							m_flowNum=0;
						}
						break;
					case 2:
						if (m_isDrawingBuilding)
						{
							m_editedEntity->rightButtonDown(lx,ly,rx,ry);
							m_editedEntity=NULL;
							m_pLLine = NULL;
							m_pRLine = NULL;
							m_flowNum=3;
						}
						else
						{
							//0712 ��
							if (!m_editedEntity->canEntityFinish())
							{
								MessageBox(_T("����ɻ滭"));
								return;
							}
							//�޸����
							m_editedEntity->rightButtonDown(lx,ly,rx,ry);
							m_editedEntity=NULL;
							m_pLLine = NULL;
							m_pRLine = NULL;
							m_flowNum=0;
						}
						break;
					case 3:
						switch (m_sideState)
						{
						case ROOFPOINT:
							if (m_editedBuilding->isBuildingFinish())
							{
								m_editedBuilding = NULL;
								m_pLLine = NULL;
								m_pRLine = NULL;
								m_sideState = ROOFPOINT;
								m_flowNum = 0;
								m_isDrawingBuilding = false;
							}
							break;
						case BOTTOMPOINT:
							m_editedBuilding->resetRoofPoint();
							m_sideState = ROOFPOINT;
							break;
						default:
							break;
						}
						break;
					}
				}
				break;
				//�Ķ�����
			}
		}
		break;
	}


/*
	switch(m_procedure)
	{
	case DRAW:
		{
			switch(m_flowNum)
			{
			case 0:
				if (!m_pLLine)
				{
					m_pLLine = m_LLineLayer.AddLine();
				}
				if (!m_pRLine)
				{
					m_pRLine=m_RLineLayer.AddLine();
				}
				if (!m_editedEntity)
				{
					switch(m_tool)
					{
					case POLYGON:
						m_editedEntity=new polyline(m_pLLine,m_pRLine);
						break;
					case ARC:
						m_editedEntity=new arc(m_pLLine,m_pRLine,this);
						break;
					default:
						break;
					}
				}
				if (m_isDrawingBuilding&&(!m_editedBuilding))
				{
					m_editedBuilding= new polygonBuilding;
					m_editedBuilding->addRoofEntity(m_editedEntity);
					m_entities.push_back(m_editedBuilding);
				}
				else
				{
					m_entities.push_back(m_editedEntity);
				}
				m_editedEntity->leftButtonUp(lx,ly,rx,ry);
				m_information.setLastPoint(lx,ly,rx,ry);
				m_flowNum=2;
				break;
			case 1:
				m_pLLine = m_LLineLayer.AddLine();
				m_pRLine= m_RLineLayer.AddLine();
				switch(m_tool)
				{
				case POLYGON:
					m_editedEntity=new polyline(m_pLLine,m_pRLine,
						m_information.m_lastLx,m_information.m_lastLy,
						m_information.m_lastRx,m_information.m_lastRy);
					break;
				case ARC:
					m_editedEntity=new arc(m_pLLine,m_pRLine,this,
						m_information.m_lastLx,m_information.m_lastLy,
						m_information.m_lastRx,m_information.m_lastRy);
					break;
				default:
					break;
				}
				if (m_isDrawingBuilding)
				{
					m_editedBuilding->addRoofEntity(m_editedEntity);
				} 
				else
				{
					m_entities.push_back(m_editedEntity);
				}
				switch(nFlags)
				{
				case 1:
					m_editedEntity->leftButtonUp(lx,ly,rx,ry);
					m_information.setLastPoint(lx,ly,rx,ry);
					break;
				case 0:
					m_editedEntity->moveMouse(lx,ly,rx,ry);
					//m_editedEntity->addPreviewPoint(lx,ly,rx,ry);
					break;
				}
				break;
			case 2:
				switch(nFlags)
				{
				case 1:
					m_editedEntity->leftButtonUp(lx,ly,rx,ry);
					//m_editedEntity->addDominantPoint(lx,ly,rx,ry);
					m_information.setLastPoint(lx,ly,rx,ry);
					break;
				case 0:
					m_editedEntity->moveMouse(lx,ly,rx,ry);
					break;
				case 3:
					m_editedEntity->rightButtonDown(lx,ly,rx,ry);
					m_editedEntity=NULL;
					m_pLLine=NULL;
					m_pRLine=NULL;
					m_flowNum=0;
					break;
				}
				break;
			}
			break;
		}
	}*/



	/*switch(m_flowNum)
	{
	case 0:
		{
			if (MK_LBUTTON == nFlags)
			{
				//��ͼ�������ʸ����
				if(m_pLLine == NULL )
					m_pLLine = m_LLineLayer.AddLine();
				if(m_pRLine==NULL)
					m_pRLine = m_RLineLayer.AddLine();

				//��ӵ�
				m_pLLine->push_back(lpt);
				m_pRLine->push_back(rpt);

				//Ϊ������β���ߣ������һ����
				m_pLLine->push_back(lpt);
				m_pRLine->push_back(rpt);

				//��һ��
				m_pLLine->Normalize();
				m_pRLine->Normalize();
				//���ñպ�
				m_pLLine->m_bClosed=true;
				m_pRLine->m_bClosed=true;

				m_flowNum=1;
			}

			break;
		}
	case 1:
		{
			CEditLine::reference Llast=m_pLLine->back();
			CEditLine::reference Rlast=m_pRLine->back();
			Llast=lpt;
			Rlast=rpt;

			if (MK_LBUTTON == nFlags)
			{
				m_pLLine->push_back(lpt);
				m_pRLine->push_back(rpt);
			}

			m_pLLine->m_bClosed=true;
			m_pRLine->m_bClosed=true;
			m_pRLine->Normalize();
			m_pLLine->Normalize();


			break;
		}
	case 2://�Ҽ������ݶ�����
		{
			if (m_pLLine->size()<4)
			{
				MessageBox(_T("���ٲ������㣡"),_T("��ʾ"),MB_OK);
				m_flowNum=1;
				return;
			}

			//�����һ����ɾ�����������Ϊ������β���߶���ӵ�
			m_pLLine->pop_back();
			m_pRLine->pop_back();

			m_pLLine->Normalize();
			m_pRLine->Normalize();

			m_flowNum=3;
			break;
		}
	case 3:
		{
			if (MK_LBUTTON == nFlags)
			{
				//ǰ�����������������
				//˵�������������� point(x,y) ���﷽�����ת��

				//(1)����Ļ����ת��Ϊ�ֲ�����
				//double lx = 0.0, ly = 0.0, rx =0.0, ry = 0.0;
				//CPoint3D lpt, rpt;
				////��Ļ����ת��Ϊ�ֲ�����
				//TranslateScreenToLocal(point, lx, ly, rx, ry);
				//lpt.m_X = lx;	lpt.m_Y = ly; lpt.m_Z = 0.0;
				//rpt.m_X = rx, rpt.m_Y= ry; rpt.m_Z = 0.0;

				//(2) �ֲ����굽��ƽ������
				//m_LViewer.LocalToPixel(lpt.m_X, lpt.m_Y, x, y);
				//px[0] = float(x); py[0] = float(y);
				//m_RViewer.LocalToPixel(rpt.m_X, rpt.m_Y, x, y);
				//px[1] = float(x); py[1] = float(y);

				//(3)ǰ������
				//CAerialBlock::SpaceIntersection(m_lpAerialPhoto, px, py, 2, &X, &Y, &Zmin);


				CPoint3D pt;
				float px[2], py[2];
				double x = 0.0, y = 0.0;
				double X = 0.0, Y = 0.0, Z = 0.0;
				double Zmin = 0.0, Zmax = 0.0;
				m_LViewer.LocalToPixel(lpt.m_X, lpt.m_Y, x, y);
				px[0] = float(x); py[0] = float(y);
				m_RViewer.LocalToPixel(rpt.m_X, rpt.m_Y, x, y);
				px[1] = float(x); py[1] = float(y);
				CAerialBlock::SpaceIntersection(m_lpAerialPhoto, px, py, 2, &X, &Y, &Zmin);


				//TODO: ȡ��m_pLLine��m_pRLine�еľֲ����������������ݶ�����������
				//TODO: ����ģ��
				//......


				m_pLLine = NULL;
				m_pRLine = NULL;
				m_flowNum = 0;
			}
			break;
		}
	}*/
}


void CStereoViewDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (m_procedure)
	{
	case DRAW:
		AddPolygonBuilding(2, point);
		Invalidate(FALSE);
		break;
	default:
		break;
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void CStereoViewDlg::moveref(CPoint pointBefore, CPoint pointNew)
{
	double lxBefore, lyBefore, rxBefore, ryBefore;
	double lxNew, lyNew, rxNew, ryNew;
	TranslateScreenToLocal(pointBefore, lxBefore, lyBefore, rxBefore, ryBefore);
	TranslateScreenToLocal(pointNew, lxNew, lyNew, rxNew, ryNew);
	for (int i = 0; i < m_entities.size();i++)
	{
		m_entities[i]->moveRef(lxBefore, lyBefore, rxBefore, ryBefore,
			lxNew, lyNew, rxNew, ryNew);
	}
}

CPoint& CStereoViewDlg::getNearestRef(CPoint pointBefore, double &dist)
{
	double lx, ly, rx, ry;
	double rlx, rly, rrx, rry;//resultLx,resultLy,resultRx,resultRy
	TranslateScreenToLocal(pointBefore, lx, ly, rx, ry);
	dist = 100;
	double minDist = 100;
	CPoint newPoint;
	for (int i = 0; i < m_entities.size();i++)
	{
		double tmpDist;
		NearestSide side;
		m_entities[i]->getNearestRef(lx, ly, rx, ry,
			rlx, rly, rrx, rry, tmpDist,side);
		if (tmpDist<minDist)
		{
			minDist = tmpDist;
			double newPx, newPy;
			switch (side)
			{
			case LEFT:
				m_LViewer.LocalToClient(rlx, rly, newPx, newPy);
				newPoint.x = newPx;
				newPoint.y = newPy;
				break;
			case RIGHT:
				m_LViewer.LocalToClient(rrx, rry, newPx, newPy);
				newPoint.x = newPx;
				newPoint.y = newPy;
				break;
			default:
				break;
			}
		}
	}
	dist = minDist;
	return newPoint;
}

void CStereoViewDlg::switchProcedure()
{
	switch (m_procedure)
	{
	case DRAW:
		m_procedure = MODIFY;
		break;
	case MODIFY:
		m_procedure = DRAW;
		break;
	default:
		break;
	}
}

void CStereoViewDlg::switchVerticalMode()
{
	switch (m_verticalMode)
	{
	case ON:
		m_verticalMode = OFF;
		if (m_editedEntity&&m_tool == POLYLINE)
		{
			polyline* line = static_cast<polyline*>(m_editedEntity);
			line->setVerticalMode(OFF);
		}
		break;
	case OFF:
		m_verticalMode = ON;
		if (m_editedEntity&&m_tool==POLYLINE)
		{
			polyline* line = static_cast<polyline*>(m_editedEntity);
			line->setVerticalMode(ON);
		}
		break;
	default:
		break;
	}
}

//0712 ����ӵ�4������
//switchPolygonMode getInformation deleteEntities getRayByMouse
afx_msg void CStereoViewDlg::switchPolygonMode()
{
	if (CIRCLE==m_tool&&POLYGONOFF==m_polygonMode)
	{
		MessageBox(_T("polygonģʽ�������û�Բ"));
		return;
	}
	if (POLYGONON==m_polygonMode)
	{
		if (m_editedPolygon)
		{
			MessageBox(_T("���������Ļ滭"));
		}
		else
		{
			m_polygonMode=POLYGONOFF;
		}
	}
	else
	{
		if (m_editedEntity)
		{
			MessageBox(_T("������ɵ�ǰ�滭"));
		}
		else
		{
			m_polygonMode=POLYGONON;
		}
	}
}

afx_msg void CStereoViewDlg::getInformation()
{
	CString tool;
	CString procedure;
	CString lineState;
	CString sideState;
	CString verticalM;
	CString polygonM;
	switch(m_tool)
	{
	case POLYLINE:
		tool="tool: polyline\n";
		break;
	case ARC:
		tool="tool: arc\n";
		break;
	case CIRCLE:
		tool="tool: circle\n";
		break;
	default:
		tool="tool: undefined\n";
		break;
	}

	switch(m_procedure)
	{
	case DRAW:
		procedure="procedure: draw\n";
		break;
	case MODIFY:
		procedure="procedure: modify\n";
		break;
	}

	switch(m_lineState)
	{
	case ROOF:
		lineState="lineState: roof\n";
		break;
	case SIDE:
		lineState="lineState: sideLine\n";
		break;
	}

	switch(m_sideState)
	{
	case ROOFPOINT:
		sideState="sideState: roofPoint\n";
		break;
	case BOTTOMPOINT:
		sideState="sideState: bottomPoint\n";
		break;
	}

	switch(m_verticalMode)
	{
	case ON:
		verticalM="verticalState: on\n";
		break;
	case OFF:
		verticalM="verticalState: off\n";
		break;
	}

	switch(m_polygonMode)
	{
	case POLYGONON:
		polygonM="polygonMode: on\n";
		break;
	case POLYGONOFF:
		polygonM="polygonMode: off\n";
		break;
	}
	/*std::string allInform=tool+procedure+lineState+sideState
		+verticalM+polygonM;*/
	CString cst=tool+procedure+lineState+sideState+verticalM+polygonM;
	MessageBox(cst);
}

afx_msg void CStereoViewDlg::deleteEntities(std::vector<entity*>&entVec)
{
	for (int i=0;i<entVec.size();i++)
	{
		entVec[i]->clearData();
		delete entVec[i];
	}
	entVec.clear();
	Invalidate(false);
}

afx_msg void CStereoViewDlg::getRayByMouse(double &dX,double &dY,double&dZ, 
	double &sourceX,double &sourceY,double &sourceZ)
{
	if (!m_lpAerialPhoto[0])
	{
		return;
	}

	CPoint mousePoint;
	GetCursorPos(&mousePoint);//�������Ļ������;
	CPoint windowPoint;
	CRect wrc;
	GetWindowRect(&wrc);
	windowPoint=wrc.TopLeft();
	mousePoint.x=mousePoint.x-windowPoint.x;
	mousePoint.y=mousePoint.y-windowPoint.y;

	double llx,lly,lrx,lry;
	TranslateScreenToLocal(mousePoint,llx,lly,lrx,lry);
	double plx,ply;
	m_LViewer.LocalToPixel(llx,lly,plx,ply);
	
	const CAerialPhoto::EXTORI*ext=m_lpAerialPhoto[0]->GetExtOri();
	sourceX=ext->POS[0];
	sourceY=ext->POS[1];
	sourceZ=ext->POS[2];
	double newX,newY,newZ;
	newZ=sourceZ/2;
	m_lpAerialPhoto[0]->PixelRayIntersect(plx,ply,newZ,newX,newY);
	dX=newX-sourceX;
	dY=newY-sourceY;
	dZ=newZ-sourceZ;
}
//�޸����




//20160626 ��ʾ��׼�����
//��mouseLdown��mousemove�¼��ж���
void CStereoViewDlg::ShowHorDatumGrid(CPoint mousePnt)
{
	//���ж����ų̶ȣ�Ӱ��̫С�Ͳ���ʾ
	if (m_nImgScale < 10)
	{
		return;
	}

	GetSpaceCoor(mousePnt,m_datumCent);

	//�Ȳ���ȡ��ǰ�ӽǷ�Χ
	int nLineNum = int(m_dGridRange/m_dGridSize + 0.5);

	CPoint3D gridStart, gridEnd;

	double* x1 = new double[nLineNum*2];
	double* y1 = new double[nLineNum*2];
	double* z1 = new double[nLineNum*2];
	double* x2 = new double[nLineNum*2];
	double* y2 = new double[nLineNum*2];
	double* z2 = new double[nLineNum*2];

	//��Y��ƽ��
	for (int i=0;i<nLineNum;i++)
	{
			gridStart.m_X = m_datumCent.m_X + m_dGridRange/2 - m_dGridSize * i;
			gridEnd.m_X = gridStart.m_X;

			gridStart.m_Y = m_datumCent.m_Y + m_dGridRange/2;
			gridEnd.m_Y = m_datumCent.m_Y - m_dGridRange/2;

			gridStart.m_Z = m_datumCent.m_Z;
			gridEnd.m_Z = m_datumCent.m_Z;

			x1[i] = gridStart.m_X;
			y1[i] = gridStart.m_Y;
			z1[i] = gridStart.m_Z;
			x2[i] = gridEnd.m_X;
			y2[i] = gridEnd.m_Y;
			z2[i] = gridEnd.m_Z;
	}

	//��X��ƽ��
	for (int j=0;j<nLineNum;j++)
	{
		gridStart.m_Y = m_datumCent.m_Y + m_dGridRange/2 - m_dGridSize * j;
		gridEnd.m_Y = gridStart.m_Y;

		gridStart.m_X = m_datumCent.m_X + m_dGridRange/2;
		gridEnd.m_X = m_datumCent.m_X - m_dGridRange/2;

		gridStart.m_Z = m_datumCent.m_Z;
		gridEnd.m_Z = m_datumCent.m_Z;

		x1[nLineNum+j] = gridStart.m_X;
		y1[nLineNum+j] = gridStart.m_Y;
		z1[nLineNum+j] = gridStart.m_Z;
		x2[nLineNum+j] = gridEnd.m_X;
		y2[nLineNum+j] = gridEnd.m_Y;
		z2[nLineNum+j] = gridEnd.m_Z;
	}

	//����ˢ�»��ƻ�׼��������
	DrawGrid(2*nLineNum,x1,y1,z1,x2,y2,z2);

	//���浱ǰ��׼��
	m_dNormX = 0;
	m_dNormY = 0;
	m_dNormZ = 1;
	m_dX = m_datumCent.m_X;
	m_dY = m_datumCent.m_Y;
	m_dZ = m_datumCent.m_Z;

	m_bShowDatum = true;

	//���
	m_vGridStartCoor.clear();
	m_vGridEndCoor.clear();

	for (int i=0;i<2*nLineNum;i++)
	{
		m_vGridStartCoor.push_back(x1[i]);
		m_vGridStartCoor.push_back(y1[i]);
		m_vGridStartCoor.push_back(z1[i]);

		m_vGridEndCoor.push_back(x2[i]);
		m_vGridEndCoor.push_back(y2[i]);
		m_vGridEndCoor.push_back(z2[i]);
	}

	delete x1;delete y1;delete z1;
	delete x2;delete y2;delete z2;
}

//�����׼
void CStereoViewDlg::ShowVerDatumGrid(CPoint mousePnt2)
{
	if (m_nImgScale < 10)
	{
		return;
	}

	GetSpaceCoor(mousePnt2,m_datumPnt2);

	double deltaX = m_datumPnt2.m_X - m_datumCent.m_X;
	double deltaY = m_datumPnt2.m_Y - m_datumCent.m_Y;

	m_dRangeX = m_dGridRange * deltaX/sqrt(deltaX*deltaX + deltaY*deltaY);
	m_dRangeY = m_dGridRange * deltaY/sqrt(deltaX*deltaX + deltaY*deltaY);

	m_dSizeX = m_dGridSize * deltaX/sqrt(deltaX*deltaX + deltaY*deltaY);
	m_dSizeY = m_dGridSize * deltaY/sqrt(deltaX*deltaX + deltaY*deltaY);

	//�Ȳ���ȡ��ǰ�ӽǷ�Χ
	int nLineNum = int(m_dGridRange/m_dGridSize + 0.5);

	CPoint3D gridStart, gridEnd;

	double* x1 = new double[nLineNum*2];
	double* y1 = new double[nLineNum*2];
	double* z1 = new double[nLineNum*2];
	double* x2 = new double[nLineNum*2];
	double* y2 = new double[nLineNum*2];
	double* z2 = new double[nLineNum*2];

	//��ˮƽ��ƽ��
	for (int i=0;i<nLineNum;i++)
	{
		//gridStart.m_X = m_datumCent.m_X + m_dRangeX/2;
		//gridEnd.m_X = m_datumCent.m_X - m_dRangeX/2;

		gridStart.m_X = m_datumCent.m_X; //�����ڽǵ�Ч����Σ�
		gridEnd.m_X = m_datumCent.m_X + m_dRangeX;

		//gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2;
		//gridEnd.m_Y = m_datumCent.m_Y - m_dRangeY/2;

		gridStart.m_Y = m_datumCent.m_Y;
		gridEnd.m_Y = m_datumCent.m_Y + m_dRangeY;

		//ZӦ�þ��У���Ϊ��֪�����ݳ��ϻ��ǳ���
		gridStart.m_Z = m_datumCent.m_Z + m_dGridRange/2 - m_dGridSize * i ;
		gridEnd.m_Z = gridStart.m_Z;

		x1[i] = gridStart.m_X;
		y1[i] = gridStart.m_Y;
		z1[i] = gridStart.m_Z;
		x2[i] = gridEnd.m_X;
		y2[i] = gridEnd.m_Y;
		z2[i] = gridEnd.m_Z;
	}

	//ƽ����Z��
	for (int j=0; j<nLineNum; j++)
	{
		//gridStart.m_X = m_datumCent.m_X + m_dRangeX/2 - m_dSizeX * j;
		gridStart.m_X = m_datumCent.m_X + m_dSizeX * j;
		gridEnd.m_X = gridStart.m_X;

		//gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2 - m_dSizeY * j;
		gridStart.m_Y = m_datumCent.m_Y + m_dSizeY * j;
		gridEnd.m_Y = gridStart.m_Y;

		gridStart.m_Z = m_datumCent.m_Z + m_dGridRange/2;
		gridEnd.m_Z = gridStart.m_Z - m_dGridRange;

		x1[nLineNum+j] = gridStart.m_X;
		y1[nLineNum+j] = gridStart.m_Y;
		z1[nLineNum+j] = gridStart.m_Z;
		x2[nLineNum+j] = gridEnd.m_X;
		y2[nLineNum+j] = gridEnd.m_Y;
		z2[nLineNum+j] = gridEnd.m_Z;
	}

	//����ˢ�»��ƻ�׼��������
	DrawGrid(2*nLineNum,x1,y1,z1,x2,y2,z2);

	//���浱ǰ��׼��
	m_dNormX = m_dRangeY;
	m_dNormY = -m_dRangeX;
	m_dNormZ = 0;
	m_dX = m_datumCent.m_X;
	m_dY = m_datumCent.m_Y;
	m_dZ = m_datumCent.m_Z;

	m_bShowDatum = true;

	//���
	m_vGridStartCoor.clear();
	m_vGridEndCoor.clear();

	for (int i=0;i<2*nLineNum;i++)
	{
		m_vGridStartCoor.push_back(x1[i]);
		m_vGridStartCoor.push_back(y1[i]);
		m_vGridStartCoor.push_back(z1[i]);

		m_vGridEndCoor.push_back(x2[i]);
		m_vGridEndCoor.push_back(y2[i]);
		m_vGridEndCoor.push_back(z2[i]);
	}

	delete x1;delete y1;delete z1;
	delete x2;delete y2;delete z2;
}

//�����׼��ǰ������ĵ���ShowVerDatumGrid�������
void CStereoViewDlg::ShowRanDatumGrid(CPoint mousePnt3)
{
	if (m_nImgScale < 10)
	{
		return;
	}

	GetSpaceCoor(mousePnt3,m_datumPnt3);

	double dX, dY, dZ; //ǰ2�����������
	dX = m_datumPnt2.m_X - m_datumCent.m_X;
	dY = m_datumPnt2.m_Y - m_datumCent.m_Y;
	dZ = m_datumPnt2.m_Z - m_datumCent.m_Z;

	//��ֱ�ģ���Ҫ����ƽ�淨����
	double normX = (dY)*(m_datumPnt3.m_Z-m_datumCent.m_Z)-(dZ)*(m_datumPnt3.m_Y-m_datumCent.m_Y);  
	double normY = (dZ)*(m_datumPnt3.m_X-m_datumCent.m_X)-(dX)*(m_datumPnt3.m_Z-m_datumCent.m_Z); 
	double normZ = (dX)*(m_datumPnt3.m_Y-m_datumCent.m_Y)-(dY)*(m_datumPnt3.m_X-m_datumCent.m_X);

	CPoint3D tempPnt; //��1��͵�2�������ϵĵ㣬���3������ƽ����ˮƽ�棬û���⣬������2��֮��
	tempPnt.m_Z = m_datumPnt3.m_Z;
	double lengthRatio = (tempPnt.m_Z - m_datumCent.m_Z)/(m_datumPnt2.m_Z - m_datumCent.m_Z);
	tempPnt.m_X = m_datumCent.m_X + lengthRatio * (m_datumPnt2.m_X - m_datumCent.m_X);
	tempPnt.m_Y = m_datumCent.m_Y + lengthRatio * (m_datumPnt2.m_Y - m_datumCent.m_Y);

	//����ˮƽ����������ȷ������
	double deltaX = tempPnt.m_X - m_datumCent.m_X;
	double deltaY = tempPnt.m_Y - m_datumCent.m_Y;
	
	//ˮƽ��
	m_dRangeX = m_dGridRange * deltaX/sqrt(deltaX*deltaX + deltaY*deltaY);
	m_dRangeY = m_dGridRange * deltaY/sqrt(deltaX*deltaX + deltaY*deltaY);
	
	m_dSizeX = m_dGridSize * deltaX/sqrt(deltaX*deltaX + deltaY*deltaY);
	m_dSizeY = m_dGridSize * deltaY/sqrt(deltaX*deltaX + deltaY*deltaY);

	//��ֱ��
	m_dRangeX2 = m_dGridRange * normX/sqrt(normX*normX + normY*normY);
	m_dRangeY2 = m_dGridRange * normY/sqrt(normX*normX + normY*normY);
	m_dRangeZ = m_dGridRange * sqrt(normX*normX + normY*normY)/
		sqrt(normX*normX + normY*normY + normZ*normZ);

	m_dSizeX2 = m_dGridSize * normX/sqrt(normX*normX + normY*normY);
	m_dSizeY2 = m_dGridSize * normY/sqrt(normX*normX + normY*normY);
	m_dSizeZ = m_dGridSize * sqrt(normX*normX + normY*normY)/
		sqrt(normX*normX + normY*normY + normZ*normZ);

	//�Ȳ���ȡ��ǰ�ӽǷ�Χ
	int nLineNum = int(m_dGridRange/m_dGridSize + 0.5);

	CPoint3D gridStart, gridEnd;

	double* x1 = new double[nLineNum*2];
	double* y1 = new double[nLineNum*2];
	double* z1 = new double[nLineNum*2];
	double* x2 = new double[nLineNum*2];
	double* y2 = new double[nLineNum*2];
	double* z2 = new double[nLineNum*2];

	//��ˮƽ��ƽ�У�����
	for (int i=0;i<nLineNum;i++)
	{
		//ȡ�����е�
		gridStart.m_X = m_datumCent.m_X + m_dRangeX/2 + m_dRangeX2/2 - m_dSizeX2*i;
		gridEnd.m_X = m_datumCent.m_X - m_dRangeX/2 + m_dRangeX2/2 - m_dSizeX2*i;

		gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2 + m_dRangeY2/2 - m_dSizeY2*i;
		gridEnd.m_Y = m_datumCent.m_Y - m_dRangeY/2 + m_dRangeY2/2 - m_dSizeY2*i;

		gridStart.m_Z = m_datumCent.m_Z + m_dRangeZ/2 - m_dSizeZ * i ;
		gridEnd.m_Z = gridStart.m_Z;

		x1[i] = gridStart.m_X;
		y1[i] = gridStart.m_Y;
		z1[i] = gridStart.m_Z;
		x2[i] = gridEnd.m_X;
		y2[i] = gridEnd.m_Y;
		z2[i] = gridEnd.m_Z;
	}

	//����ߴ�ֱ
	for (int j=0;j<nLineNum;j++)
	{
		gridStart.m_X = m_datumCent.m_X + m_dRangeX/2  - m_dRangeX2/2 - m_dSizeX * j;
		gridEnd.m_X = m_datumCent.m_X + m_dRangeX/2  + m_dRangeX2/2 - m_dSizeX * j;

		gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2  - m_dRangeY2/2 - m_dSizeY * j;
		gridEnd.m_Y = m_datumCent.m_Y + m_dRangeY/2  + m_dRangeY2/2 - m_dSizeY * j;

		gridStart.m_Z = m_datumCent.m_Z + m_dRangeZ/2;
		gridEnd.m_Z = gridStart.m_Z - m_dRangeZ/2;

		x1[nLineNum+j] = gridStart.m_X;
		y1[nLineNum+j] = gridStart.m_Y;
		z1[nLineNum+j] = gridStart.m_Z;
		x2[nLineNum+j] = gridEnd.m_X;
		y2[nLineNum+j] = gridEnd.m_Y;
		z2[nLineNum+j] = gridEnd.m_Z;
	}

	//����ˢ�»��ƻ�׼��������
	DrawGrid(2*nLineNum,x1,y1,z1,x2,y2,z2);

	//���浱ǰ��׼��
	m_dNormX = normX;
	m_dNormY = normY;
	m_dNormZ = normZ;
	m_dX = m_datumCent.m_X;
	m_dY = m_datumCent.m_Y;
	m_dZ = m_datumCent.m_Z;

	m_bShowDatum = true;

	//���
	m_vGridStartCoor.clear();
	m_vGridEndCoor.clear();

	for (int i=0;i<2*nLineNum;i++)
	{
		m_vGridStartCoor.push_back(x1[i]);
		m_vGridStartCoor.push_back(y1[i]);
		m_vGridStartCoor.push_back(z1[i]);

		m_vGridEndCoor.push_back(x2[i]);
		m_vGridEndCoor.push_back(y2[i]);
		m_vGridEndCoor.push_back(z2[i]);
	}

	delete x1;delete y1;delete z1;
	delete x2;delete y2;delete z2;
}


//����������������ռ����� 0627
void CStereoViewDlg::GetSpaceCoor(CPoint point, CPoint3D & resultPnt)
{
	double lx = 0.0, ly = 0.0, rx =0.0, ry = 0.0;
	CPoint3D lpt, rpt;

	//��Ļ����ת��Ϊ�ֲ�����
	TranslateScreenToLocal(point, lx, ly, rx, ry);

	lpt.m_X = lx;	lpt.m_Y = ly; lpt.m_Z = 0.0;
	rpt.m_X = rx; rpt.m_Y= ry; rpt.m_Z = 0.0;

	float px[2], py[2];
	double x = 0.0, y = 0.0;
	double X = 0.0, Y = 0.0, Z = 0.0;

	//ת����������
	m_LViewer.LocalToPixel(lpt.m_X, lpt.m_Y, x, y);
	px[0] = float(x); py[0] = float(y);
	m_RViewer.LocalToPixel(rpt.m_X, rpt.m_Y, x, y);
	px[1] = float(x); py[1] = float(y);

	//ǰ������
	CAerialBlock::SpaceIntersection(m_lpAerialPhoto, px, py, 2, &X, &Y, &Z);

	resultPnt.m_X = X;
	resultPnt.m_Y = Y;
	resultPnt.m_Z = Z;

}

//���ơ�ˢ�¸��� x1��㣬x2�յ�
void CStereoViewDlg::DrawGrid(int lineNum,double* x1,double* y1,double* z1,
	double* x2,double* y2,double* z2)
{
	//ÿ���ػ�ǰ��Ҫ����ɾ������
	for (int i=0;i<m_vLDatumGird.size();i++)
	{
		m_vLDatumGird[i]->clear();
		m_vRDatumGird[i]->clear();
	}
	m_vLDatumGird.clear();
	m_vRDatumGird.clear();

	for (int i=0;i<lineNum;i++)
	{
		CPoint3D lpt1,lpt2,rpt1,rpt2;
		CPoint3D toPnt,fromPnt;
		double x ,y;

		//�˺����ڲ���Ҫƫ��
// 		fromPnt.m_X = x1[i]+dXOffset; fromPnt.m_Y = y1[i]+dYOffset; fromPnt.m_Z = z1[i] + dZOffset;
// 		toPnt.m_X = x2[i]+dXOffset; toPnt.m_Y = y2[i]+dYOffset; toPnt.m_Z = z2[i] + dZOffset;

		fromPnt.m_X = x1[i]; fromPnt.m_Y = y1[i]; fromPnt.m_Z = z1[i];
		toPnt.m_X = x2[i]; toPnt.m_Y = y2[i]; toPnt.m_Z = z2[i];

		//��Ӱ��

		m_lpAerialPhoto[0]->ObjectCS2PixelCS(fromPnt.m_X,fromPnt.m_Y,fromPnt.m_Z,x,y);
		m_LViewer.PixelToLocal(x,y,x,y);
		lpt1.m_X = x;
		lpt1.m_Y = y;

		m_lpAerialPhoto[0]->ObjectCS2PixelCS(toPnt.m_X,toPnt.m_Y,toPnt.m_Z,x,y);
		m_LViewer.PixelToLocal(x,y,x,y);
		lpt2.m_X = x;
		lpt2.m_Y = y;

		CEditLine * objLLine = m_LLineLayer.AddLine();
		objLLine->push_back(lpt1);
		objLLine->push_back(lpt2);

		//��Ӱ��
		m_lpAerialPhoto[1]->ObjectCS2PixelCS(fromPnt.m_X,fromPnt.m_Y,fromPnt.m_Z,x,y);
		m_RViewer.PixelToLocal(x,y,x,y);
		rpt1.m_X = x;
		rpt1.m_Y = y;
		m_lpAerialPhoto[1]->ObjectCS2PixelCS(toPnt.m_X,toPnt.m_Y,toPnt.m_Z,x,y);
		m_RViewer.PixelToLocal(x,y,x,y);
		rpt2.m_X = x;
		rpt2.m_Y = y;

		CEditLine * objRLine = m_RLineLayer.AddLine();
		objRLine->push_back(rpt1);
		objRLine->push_back(rpt2);

		objLLine->Normalize();
		objRLine->Normalize();

		m_vLDatumGird.push_back(objLLine);
		m_vRDatumGird.push_back(objRLine);

	}

	Invalidate(FALSE);
}

void CStereoViewDlg::ChangeMode2Datum(int nDatumType)
{
	this->m_procedure = DATUM;

	if (nDatumType == 0)
	{
		this->m_DatumType = HORIZON;
	}
	if (nDatumType == 1)
	{
		this->m_DatumType = VERTICAL;
	}
	if (nDatumType == 2)
	{
		this->m_DatumType = RANDOM;
	}
}


void CStereoViewDlg::ChangeGridSize(bool isEnlarge)
{
	if (m_nImgScale < 10)
	{
		return;
	}

	if (isEnlarge) //������
	{
		if (m_dGridSize>100) //����
		{
			return;
		}
		m_dGridSize = m_dGridSize * 2;
		m_dSizeX = m_dSizeX * 2;
		m_dSizeY = m_dSizeY * 2;
		m_dSizeX2 = m_dSizeX2 * 2;
		m_dSizeY2 = m_dSizeY2 * 2;
		m_dSizeZ = m_dSizeZ * 2;
	}
	else
	{
		if (m_dGridSize<0.1) //����
		{
			return;
		}
		m_dGridSize = m_dGridSize / 2;
		m_dSizeX = m_dSizeX / 2;
		m_dSizeY = m_dSizeY / 2;
		m_dSizeX2 = m_dSizeX2 / 2;
		m_dSizeY2 = m_dSizeY2 / 2;
		m_dSizeZ = m_dSizeZ / 2;
	}

	//��������show������һ��
	//�Ȳ���ȡ��ǰ�ӽǷ�Χ
	int nLineNum = int(m_dGridRange/m_dGridSize + 0.5);

	CPoint3D gridStart, gridEnd;

	double* x1 = new double[nLineNum*2];
	double* y1 = new double[nLineNum*2];
	double* z1 = new double[nLineNum*2];
	double* x2 = new double[nLineNum*2];
	double* y2 = new double[nLineNum*2];
	double* z2 = new double[nLineNum*2];

	switch(m_DatumType)
	{
	case HORIZON:
		//��Y��ƽ��
		for (int i=0;i<nLineNum;i++)
		{
			gridStart.m_X = m_datumCent.m_X + m_dGridRange/2 - m_dGridSize * i;
			gridEnd.m_X = gridStart.m_X;

			gridStart.m_Y = m_datumCent.m_Y + m_dGridRange/2;
			gridEnd.m_Y = m_datumCent.m_Y - m_dGridRange/2;

			gridStart.m_Z = m_datumCent.m_Z;
			gridEnd.m_Z = m_datumCent.m_Z;

			x1[i] = gridStart.m_X;
			y1[i] = gridStart.m_Y;
			z1[i] = gridStart.m_Z;
			x2[i] = gridEnd.m_X;
			y2[i] = gridEnd.m_Y;
			z2[i] = gridEnd.m_Z;
		}

		//��X��ƽ��
		for (int j=0;j<nLineNum;j++)
		{
			gridStart.m_Y = m_datumCent.m_Y + m_dGridRange/2 - m_dGridSize * j;
			gridEnd.m_Y = gridStart.m_Y;

			gridStart.m_X = m_datumCent.m_X + m_dGridRange/2;
			gridEnd.m_X = m_datumCent.m_X - m_dGridRange/2;

			gridStart.m_Z = m_datumCent.m_Z;
			gridEnd.m_Z = m_datumCent.m_Z;

			x1[nLineNum+j] = gridStart.m_X;
			y1[nLineNum+j] = gridStart.m_Y;
			z1[nLineNum+j] = gridStart.m_Z;
			x2[nLineNum+j] = gridEnd.m_X;
			y2[nLineNum+j] = gridEnd.m_Y;
			z2[nLineNum+j] = gridEnd.m_Z;
		}
		break;

	case VERTICAL:

		//��ˮƽ��ƽ��
		for (int i=0;i<nLineNum;i++)
		{
			//gridStart.m_X = m_datumCent.m_X + m_dRangeX/2;
			//gridEnd.m_X = m_datumCent.m_X - m_dRangeX/2;

			gridStart.m_X = m_datumCent.m_X; //�����ڽǵ�Ч����Σ�
			gridEnd.m_X = m_datumCent.m_X + m_dRangeX;

			//gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2;
			//gridEnd.m_Y = m_datumCent.m_Y - m_dRangeY/2;

			gridStart.m_Y = m_datumCent.m_Y;
			gridEnd.m_Y = m_datumCent.m_Y + m_dRangeY;

			gridStart.m_Z = m_datumCent.m_Z + m_dGridRange/2 - m_dGridSize * i ;
			gridEnd.m_Z = gridStart.m_Z;

			x1[i] = gridStart.m_X;
			y1[i] = gridStart.m_Y;
			z1[i] = gridStart.m_Z;
			x2[i] = gridEnd.m_X;
			y2[i] = gridEnd.m_Y;
			z2[i] = gridEnd.m_Z;
		}

		//ƽ����Z��
		for (int j=0; j<nLineNum; j++)
		{
			//gridStart.m_X = m_datumCent.m_X + m_dRangeX/2 - m_dSizeX * j;
			gridStart.m_X = m_datumCent.m_X + m_dSizeX * j;
			gridEnd.m_X = gridStart.m_X;

			//gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2 - m_dSizeY * j;
			gridStart.m_Y = m_datumCent.m_Y + m_dSizeY * j;
			gridEnd.m_Y = gridStart.m_Y;

			gridStart.m_Z = m_datumCent.m_Z + m_dGridRange/2;
			gridEnd.m_Z = gridStart.m_Z - m_dGridRange;

			x1[nLineNum+j] = gridStart.m_X;
			y1[nLineNum+j] = gridStart.m_Y;
			z1[nLineNum+j] = gridStart.m_Z;
			x2[nLineNum+j] = gridEnd.m_X;
			y2[nLineNum+j] = gridEnd.m_Y;
			z2[nLineNum+j] = gridEnd.m_Z;
		}
		break;

	case RANDOM:

		//��ˮƽ��ƽ�У�����
		for (int i=0;i<nLineNum;i++)
		{
			//ȡ�����е�
			gridStart.m_X = m_datumCent.m_X + m_dRangeX/2 + m_dRangeX2/2 - m_dSizeX2*i;
			gridEnd.m_X = m_datumCent.m_X - m_dRangeX/2 + m_dRangeX2/2 - m_dSizeX2*i;

			gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2 + m_dRangeY2/2 - m_dSizeY2*i;
			gridEnd.m_Y = m_datumCent.m_Y - m_dRangeY/2 + m_dRangeY2/2 - m_dSizeY2*i;

			gridStart.m_Z = m_datumCent.m_Z + m_dRangeZ/2 - m_dSizeZ * i ;
			gridEnd.m_Z = gridStart.m_Z;

			x1[i] = gridStart.m_X;
			y1[i] = gridStart.m_Y;
			z1[i] = gridStart.m_Z;
			x2[i] = gridEnd.m_X;
			y2[i] = gridEnd.m_Y;
			z2[i] = gridEnd.m_Z;
		}

		//����ߴ�ֱ
		for (int j=0;j<nLineNum;j++)
		{
			gridStart.m_X = m_datumCent.m_X + m_dRangeX/2  - m_dRangeX2/2 - m_dSizeX * j;
			gridEnd.m_X = m_datumCent.m_X + m_dRangeX/2  + m_dRangeX2/2 - m_dSizeX * j;

			gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2  - m_dRangeY2/2 - m_dSizeY * j;
			gridEnd.m_Y = m_datumCent.m_Y + m_dRangeY/2  + m_dRangeY2/2 - m_dSizeY * j;

			gridStart.m_Z = m_datumCent.m_Z + m_dRangeZ/2;
			gridEnd.m_Z = gridStart.m_Z - m_dRangeZ/2;

			x1[nLineNum+j] = gridStart.m_X;
			y1[nLineNum+j] = gridStart.m_Y;
			z1[nLineNum+j] = gridStart.m_Z;
			x2[nLineNum+j] = gridEnd.m_X;
			y2[nLineNum+j] = gridEnd.m_Y;
			z2[nLineNum+j] = gridEnd.m_Z;
		}
		break;
	default:
		break;
	}

	//����ˢ�»��ƻ�׼��������
	DrawGrid(2*nLineNum,x1,y1,z1,x2,y2,z2);

	m_bShowDatum = true;

	delete x1;delete y1;delete z1;
	delete x2;delete y2;delete z2;
}

//��Ϣ�޷���Ӧ��
// void CStereoViewDlg::OnHorizonDatum()
// {
// 	// TODO: �ڴ���������������
// 	this->m_procedure = DATUM;
// 	this->m_DatumType = HORIZON;
// }


void CStereoViewDlg::GetSpaceCentPnt(CPoint3D & spaceCentPnt)
{
	CPoint screenCent;
	//�����ʾӰ��Ի������Ļ���꣬�ͶԻ����С
	CRect imgRect;
	this->GetWindowRect(&imgRect);
	screenCent = imgRect.CenterPoint();
	GetSpaceCoor(screenCent, spaceCentPnt);
}

//��ȡ��ǰ�Ļ�׼�棬����������һ��
bool CStereoViewDlg::GetDatumPlane(double & normX, double & normY, double & normZ, 
	double & X, double & Y, double & Z)
{
	//��׼�淨��������ֵ��0,0,0��

	//��û�����û�׼�棬�����ٵ����л�ͼ
	if (m_DatumType == NODATUM)
	{
		return false;
	}

	normX = m_dNormX;
	normY = m_dNormY;
	normZ = m_dNormZ;

	X = m_dX;
	Y = m_dY;
	Z = m_dZ;

	return true;
}


//0712 ��SU�л���ģ�ͣ��������Ҽ��������ƺ���øú���
void CStereoViewDlg::DrawEntityInSU()
{

	

	if (m_tool == POLYLINE)
	{
		 //m_entities.back()
	}
	else if(m_tool == ARC)
	{

	}
	else if (m_tool == CIRCLE)
	{

	}

	

}

//0714 �رյ�ǰ�Ļ�׼�����
void CStereoViewDlg::CloseCurrentDatum()
{
	if (!m_bShowDatum)
	{
		return;
	}

	for (int i=0;i<m_vLDatumGird.size();i++)
	{
		m_vLDatumGird[i]->clear();
		m_vRDatumGird[i]->clear();
	}
	m_vLDatumGird.clear();
	m_vRDatumGird.clear();

	m_bShowDatum = false;
}

void CStereoViewDlg::ShowCurrentDatum()
{
	if (m_bShowDatum)
	{
		return;
	}

	if (m_nImgScale < 10)
	{
		MessageBox(L"Ӱ��̫С���޷���ʾ���뽫Ӱ��Ŵ�");
		return;
	}

	//����ϲ����if�����ÿɶ��Ա�switch��Щ����ȻЧ�ʵ�һ���
	if (m_DatumType == HORIZON)
	{
		int nLineNum = int(m_dGridRange/m_dGridSize + 0.5);

		CPoint3D gridStart, gridEnd;

		double* x1 = new double[nLineNum*2];
		double* y1 = new double[nLineNum*2];
		double* z1 = new double[nLineNum*2];
		double* x2 = new double[nLineNum*2];
		double* y2 = new double[nLineNum*2];
		double* z2 = new double[nLineNum*2];

		//��Y��ƽ��
		for (int i=0;i<nLineNum;i++)
		{
			gridStart.m_X = m_datumCent.m_X + m_dGridRange/2 - m_dGridSize * i;
			gridEnd.m_X = gridStart.m_X;

			gridStart.m_Y = m_datumCent.m_Y + m_dGridRange/2;
			gridEnd.m_Y = m_datumCent.m_Y - m_dGridRange/2;

			gridStart.m_Z = m_datumCent.m_Z;
			gridEnd.m_Z = m_datumCent.m_Z;

			x1[i] = gridStart.m_X;
			y1[i] = gridStart.m_Y;
			z1[i] = gridStart.m_Z;
			x2[i] = gridEnd.m_X;
			y2[i] = gridEnd.m_Y;
			z2[i] = gridEnd.m_Z;
		}

		//��X��ƽ��
		for (int j=0;j<nLineNum;j++)
		{
			gridStart.m_Y = m_datumCent.m_Y + m_dGridRange/2 - m_dGridSize * j;
			gridEnd.m_Y = gridStart.m_Y;

			gridStart.m_X = m_datumCent.m_X + m_dGridRange/2;
			gridEnd.m_X = m_datumCent.m_X - m_dGridRange/2;

			gridStart.m_Z = m_datumCent.m_Z;
			gridEnd.m_Z = m_datumCent.m_Z;

			x1[nLineNum+j] = gridStart.m_X;
			y1[nLineNum+j] = gridStart.m_Y;
			z1[nLineNum+j] = gridStart.m_Z;
			x2[nLineNum+j] = gridEnd.m_X;
			y2[nLineNum+j] = gridEnd.m_Y;
			z2[nLineNum+j] = gridEnd.m_Z;
		}

		//����ˢ�»��ƻ�׼��������
		DrawGrid(2*nLineNum,x1,y1,z1,x2,y2,z2);

		delete x1;delete y1;delete z1;
		delete x2;delete y2;delete z2;
	}
	else if (m_DatumType == VERTICAL)
	{
		double deltaX = m_datumPnt2.m_X - m_datumCent.m_X;
		double deltaY = m_datumPnt2.m_Y - m_datumCent.m_Y;

		m_dRangeX = m_dGridRange * deltaX/sqrt(deltaX*deltaX + deltaY*deltaY);
		m_dRangeY = m_dGridRange * deltaY/sqrt(deltaX*deltaX + deltaY*deltaY);

		m_dSizeX = m_dGridSize * deltaX/sqrt(deltaX*deltaX + deltaY*deltaY);
		m_dSizeY = m_dGridSize * deltaY/sqrt(deltaX*deltaX + deltaY*deltaY);

		//�Ȳ���ȡ��ǰ�ӽǷ�Χ
		int nLineNum = int(m_dGridRange/m_dGridSize + 0.5);

		CPoint3D gridStart, gridEnd;

		double* x1 = new double[nLineNum*2];
		double* y1 = new double[nLineNum*2];
		double* z1 = new double[nLineNum*2];
		double* x2 = new double[nLineNum*2];
		double* y2 = new double[nLineNum*2];
		double* z2 = new double[nLineNum*2];

		//��ˮƽ��ƽ��
		for (int i=0;i<nLineNum;i++)
		{
			//gridStart.m_X = m_datumCent.m_X + m_dRangeX/2;
			//gridEnd.m_X = m_datumCent.m_X - m_dRangeX/2;

			gridStart.m_X = m_datumCent.m_X; //�����ڽǵ�Ч����Σ�
			gridEnd.m_X = m_datumCent.m_X + m_dRangeX;

			//gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2;
			//gridEnd.m_Y = m_datumCent.m_Y - m_dRangeY/2;

			gridStart.m_Y = m_datumCent.m_Y;
			gridEnd.m_Y = m_datumCent.m_Y + m_dRangeY;

			//ZӦ�þ��У���Ϊ��֪�����ݳ��ϻ��ǳ���
			gridStart.m_Z = m_datumCent.m_Z + m_dGridRange/2 - m_dGridSize * i ;
			gridEnd.m_Z = gridStart.m_Z;

			x1[i] = gridStart.m_X;
			y1[i] = gridStart.m_Y;
			z1[i] = gridStart.m_Z;
			x2[i] = gridEnd.m_X;
			y2[i] = gridEnd.m_Y;
			z2[i] = gridEnd.m_Z;
		}

		//ƽ����Z��
		for (int j=0; j<nLineNum; j++)
		{
			//gridStart.m_X = m_datumCent.m_X + m_dRangeX/2 - m_dSizeX * j;
			gridStart.m_X = m_datumCent.m_X + m_dSizeX * j;
			gridEnd.m_X = gridStart.m_X;

			//gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2 - m_dSizeY * j;
			gridStart.m_Y = m_datumCent.m_Y + m_dSizeY * j;
			gridEnd.m_Y = gridStart.m_Y;

			gridStart.m_Z = m_datumCent.m_Z + m_dGridRange/2;
			gridEnd.m_Z = gridStart.m_Z - m_dGridRange;

			x1[nLineNum+j] = gridStart.m_X;
			y1[nLineNum+j] = gridStart.m_Y;
			z1[nLineNum+j] = gridStart.m_Z;
			x2[nLineNum+j] = gridEnd.m_X;
			y2[nLineNum+j] = gridEnd.m_Y;
			z2[nLineNum+j] = gridEnd.m_Z;
		}

		//����ˢ�»��ƻ�׼��������
		DrawGrid(2*nLineNum,x1,y1,z1,x2,y2,z2);

		delete x1;delete y1;delete z1;
		delete x2;delete y2;delete z2;
	}
	else if (m_DatumType == RANDOM)
	{
		double dX, dY, dZ; //ǰ2�����������
		dX = m_datumPnt2.m_X - m_datumCent.m_X;
		dY = m_datumPnt2.m_Y - m_datumCent.m_Y;
		dZ = m_datumPnt2.m_Z - m_datumCent.m_Z;

		//��ֱ�ģ���Ҫ����ƽ�淨����
		double normX = (dY)*(m_datumPnt3.m_Z-m_datumCent.m_Z)-(dZ)*(m_datumPnt3.m_Y-m_datumCent.m_Y);  
		double normY = (dZ)*(m_datumPnt3.m_X-m_datumCent.m_X)-(dX)*(m_datumPnt3.m_Z-m_datumCent.m_Z); 
		double normZ = (dX)*(m_datumPnt3.m_Y-m_datumCent.m_Y)-(dY)*(m_datumPnt3.m_X-m_datumCent.m_X);

		CPoint3D tempPnt; //��1��͵�2�������ϵĵ㣬���3������ƽ����ˮƽ�棬û���⣬������2��֮��
		tempPnt.m_Z = m_datumPnt3.m_Z;
		double lengthRatio = (tempPnt.m_Z - m_datumCent.m_Z)/(m_datumPnt2.m_Z - m_datumCent.m_Z);
		tempPnt.m_X = m_datumCent.m_X + lengthRatio * (m_datumPnt2.m_X - m_datumCent.m_X);
		tempPnt.m_Y = m_datumCent.m_Y + lengthRatio * (m_datumPnt2.m_Y - m_datumCent.m_Y);

		//����ˮƽ����������ȷ������
		double deltaX = tempPnt.m_X - m_datumCent.m_X;
		double deltaY = tempPnt.m_Y - m_datumCent.m_Y;

		//ˮƽ��
		m_dRangeX = m_dGridRange * deltaX/sqrt(deltaX*deltaX + deltaY*deltaY);
		m_dRangeY = m_dGridRange * deltaY/sqrt(deltaX*deltaX + deltaY*deltaY);

		m_dSizeX = m_dGridSize * deltaX/sqrt(deltaX*deltaX + deltaY*deltaY);
		m_dSizeY = m_dGridSize * deltaY/sqrt(deltaX*deltaX + deltaY*deltaY);

		//��ֱ��
		m_dRangeX2 = m_dGridRange * normX/sqrt(normX*normX + normY*normY);
		m_dRangeY2 = m_dGridRange * normY/sqrt(normX*normX + normY*normY);
		m_dRangeZ = m_dGridRange * sqrt(normX*normX + normY*normY)/
			sqrt(normX*normX + normY*normY + normZ*normZ);

		m_dSizeX2 = m_dGridSize * normX/sqrt(normX*normX + normY*normY);
		m_dSizeY2 = m_dGridSize * normY/sqrt(normX*normX + normY*normY);
		m_dSizeZ = m_dGridSize * sqrt(normX*normX + normY*normY)/
			sqrt(normX*normX + normY*normY + normZ*normZ);

		//�Ȳ���ȡ��ǰ�ӽǷ�Χ
		int nLineNum = int(m_dGridRange/m_dGridSize + 0.5);

		CPoint3D gridStart, gridEnd;

		double* x1 = new double[nLineNum*2];
		double* y1 = new double[nLineNum*2];
		double* z1 = new double[nLineNum*2];
		double* x2 = new double[nLineNum*2];
		double* y2 = new double[nLineNum*2];
		double* z2 = new double[nLineNum*2];

		//��ˮƽ��ƽ�У�����
		for (int i=0;i<nLineNum;i++)
		{
			//ȡ�����е�
			gridStart.m_X = m_datumCent.m_X + m_dRangeX/2 + m_dRangeX2/2 - m_dSizeX2*i;
			gridEnd.m_X = m_datumCent.m_X - m_dRangeX/2 + m_dRangeX2/2 - m_dSizeX2*i;

			gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2 + m_dRangeY2/2 - m_dSizeY2*i;
			gridEnd.m_Y = m_datumCent.m_Y - m_dRangeY/2 + m_dRangeY2/2 - m_dSizeY2*i;

			gridStart.m_Z = m_datumCent.m_Z + m_dRangeZ/2 - m_dSizeZ * i ;
			gridEnd.m_Z = gridStart.m_Z;

			x1[i] = gridStart.m_X;
			y1[i] = gridStart.m_Y;
			z1[i] = gridStart.m_Z;
			x2[i] = gridEnd.m_X;
			y2[i] = gridEnd.m_Y;
			z2[i] = gridEnd.m_Z;
		}

		//����ߴ�ֱ
		for (int j=0;j<nLineNum;j++)
		{
			gridStart.m_X = m_datumCent.m_X + m_dRangeX/2  - m_dRangeX2/2 - m_dSizeX * j;
			gridEnd.m_X = m_datumCent.m_X + m_dRangeX/2  + m_dRangeX2/2 - m_dSizeX * j;

			gridStart.m_Y = m_datumCent.m_Y + m_dRangeY/2  - m_dRangeY2/2 - m_dSizeY * j;
			gridEnd.m_Y = m_datumCent.m_Y + m_dRangeY/2  + m_dRangeY2/2 - m_dSizeY * j;

			gridStart.m_Z = m_datumCent.m_Z + m_dRangeZ/2;
			gridEnd.m_Z = gridStart.m_Z - m_dRangeZ/2;

			x1[nLineNum+j] = gridStart.m_X;
			y1[nLineNum+j] = gridStart.m_Y;
			z1[nLineNum+j] = gridStart.m_Z;
			x2[nLineNum+j] = gridEnd.m_X;
			y2[nLineNum+j] = gridEnd.m_Y;
			z2[nLineNum+j] = gridEnd.m_Z;
		}

		//����ˢ�»��ƻ�׼��������
		DrawGrid(2*nLineNum,x1,y1,z1,x2,y2,z2);

		delete x1;delete y1;delete z1;
		delete x2;delete y2;delete z2;
	}
}
