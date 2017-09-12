// singleImageView.cpp : ʵ���ļ�
//

#include "stdafx.h"
//#include "StereoMapping.h"
//#include "DocView2.h"

#include "singleImageView.h"
#include "GraphicBase.h"
#include "GraphicLayer.h"
#include "AerialBase.h"
#include "MeasureDialog.h"
#include "polylineSing.h"
#include "arcSing.h"
#include "circleSing.h"
#include "polygonSing.h"

//#include <iostream>
#define defaultPointSize 20
#define defaultLineSize 5
//Ĭ�ϴ���һ�������ĵ�
// singleImageView

IMPLEMENT_DYNCREATE(singleImageView, CPixelViewer)

singleImageView::singleImageView()
:m_pPointLayer(NULL),
m_pPolyline(NULL),
//m_pAerialPhoto(NULL),
m_point0(0),
m_pPolylineLayer(NULL),

//0712 ��
m_pReprojectPointLayer(NULL),
m_pReprojectLineLayer(NULL),
m_pRePreviewPtLayer(NULL),
m_pRePreviewLineLayer(NULL),
//�޸Ľ���


//m_vtxs(NULL),
//m_plines(NULL),
//imageInitialized(false)
m_Initialized(false),
m_t(5),
m_selVtx(-1),
m_procedure(DRAWSing),
//m_firstPoint(TRUE),
//m_tmpPointSize(0),
//m_tmpLinePointSize(0),
//m_drawingLine(false),
//m_firstLine(TRUE),
m_pAerialPhoto(NULL),
m_verticalState(OFFSing),
//m_isCurrentDirectionSet(false),
m_isMainCurrentDirectionSet(false),
m_isPlaneDecided(false),
m_modifyState(NEAUTRALSing),
m_editedEntity(NULL),
m_step(NoneSing),
m_polygonMode(POLYGONOFFSing)
//m_lineNum(0)
{
}

singleImageView::~singleImageView()
{
	/*if (m_vtxs!=NULL)
	{
		delete[] m_vtxs;
		m_vtxs=NULL;
	}
	if (m_plines!=NULL)
	{
		delete[] m_plines;
		m_plines=NULL;
	}*/
	for (int i = 0; i < m_entities.size();i++)
	{
		delete m_entities[i];
		m_entities[i] = NULL;
	}
}

BEGIN_MESSAGE_MAP(singleImageView, CPixelViewer)
	ON_WM_CREATE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// singleImageView ��ͼ

void singleImageView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// singleImageView ���

#ifdef _DEBUG
void singleImageView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void singleImageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// singleImageView ��Ϣ�������
afx_msg bool singleImageView::LoadImages(CString path)
{
	if (CPixelViewer::LoadImages(path))
	{
		ZoomToCenter(50);
		m_pPolylineLayer->ClearContent();
		m_pPointLayer->ClearContent();

		//0712 ��
		m_pReprojectPointLayer->ClearContent();
		m_pReprojectLineLayer->ClearContent();
		m_pRePreviewPtLayer->ClearContent();
		m_pRePreviewLineLayer->ClearContent();
		AddLayer(m_pReprojectPointLayer);
		AddLayer(m_pReprojectLineLayer);
		AddLayer(m_pRePreviewPtLayer);
		AddLayer(m_pRePreviewLineLayer);
		//�޸Ľ���


		/*if (m_vtxs!=NULL)
		{
			delete[] m_vtxs;
			m_vtxs=NULL;
		}*/
		AddLayer(m_pPolylineLayer);
		AddLayer(m_pPointLayer);
		m_Initialized=true;/*
		m_firstPoint=true;
		m_firstLine=true;
		m_tmpPointSize=0;
		m_drawingLine=false;*/
		return TRUE;
	}
	return false;
}

int singleImageView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPixelViewer::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	m_pPolylineLayer	= (GraphicLayer*)CreateLayer(GRAPHIC_LAYER);
	m_pPointLayer		=  (GraphicLayer*)CreateLayer(GRAPHIC_LAYER);

	//0712 ��
	m_pReprojectPointLayer=(GraphicLayer*)CreateLayer(GRAPHIC_LAYER);
	m_pReprojectLineLayer=(GraphicLayer*)CreateLayer(GRAPHIC_LAYER);
	m_pRePreviewPtLayer=(GraphicLayer*)CreateLayer(GRAPHIC_LAYER);
	m_pRePreviewLineLayer=(GraphicLayer*)CreateLayer(GRAPHIC_LAYER);
	//�޸Ľ���



	return 0;
}


void singleImageView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	SetCapture();
	CRect rc;
	GetClientRect(&rc);
	point.y =rc.Height()-point.y;

	m_point0 = point;

	CPixelViewer::OnMButtonDown(nFlags, point);
}


void singleImageView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ReleaseCapture();
	CPixelViewer::OnMButtonUp(nFlags, point);
}


void singleImageView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rc;
	GetClientRect(&rc);
	point.y=rc.Height()-point.y;
	double x, y;
	TranslateScreenToWorld(double(point.x), double(point.y), x, y);

	if (nFlags == MK_MBUTTON)
	{
		int dx = point.x - m_point0.x;
		int dy = point.y - m_point0.y;
		PanBy(-dx, -dy);
		m_point0 = point;
		Invalidate();
	}

	switch(m_procedure)
	{
	case(DRAWSing):
		if (m_editedEntity)
		{
			m_editedEntity->moveMouse(x, y);
			//0712 ��
			if (POLYGONONSing==m_polygonMode)
			{
				m_editedPolygon->calculateCloseLine();
			}
			//�޸Ľ���

			Invalidate(FALSE);

			/*double x,y;
			TranslateScreenToWorld(double(point.x),double(point.y),x,y);
			setProperPoint(x,y,x,y);
			m_pPolyline->DeleteVertex(m_tmpLinePointSize);
			m_pPolyline->AddVertex(m_tmpLinePointSize,x,y);
			Invalidate(FALSE);*/
		}
		break;

	case (MODIFYSing):
		switch(m_modifyState)
		{
		case(DRAGGINGSing):
			m_newPoint=point;
			if (sqrt((double(m_newPoint.x-m_beforePoint.x))*(double(m_newPoint.x-m_beforePoint.x))
				+(double(m_newPoint.y-m_beforePoint.y))*(double(m_newPoint.y-m_beforePoint.y)))>10)
			{
				double beforex,beforey;
				TranslateScreenToWorld(double(m_beforePoint.x),double(m_beforePoint.y),
					beforex,beforey);
				double minDist;
				CPoint ref=getNearestRef(beforex,beforey,minDist);
				if(minDist<BIGTHRESH)
				{
					m_beforePoint = ref;
					m_modifyState = MOVEREFSing;
				}
				else
				{
					m_modifyState=NEAUTRALSing;
				}
			}
			Invalidate(FALSE);
			break;
		case(MOVEREFSing):
			m_newPoint=point;
			double beforex,beforey,newx,newy;
			TranslateScreenToWorld(double(m_beforePoint.x),double(m_beforePoint.y),
				beforex,beforey);
			TranslateScreenToWorld(double(m_newPoint.x),double(m_newPoint.y),
				newx,newy);
			moveref(beforex,beforey,newx,newy);
			m_beforePoint=m_newPoint;
			Invalidate(FALSE);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	CPixelViewer::OnMouseMove(nFlags, point);
}

//˼�룺ÿ���߶εĵ�һ���㹹��������;
//����ƶ�ʱ���µĵ��������;
void singleImageView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rc;
	GetClientRect(&rc);
	point.y=rc.Height()-point.y;

	switch(m_procedure)
	{
	case (MODIFYSing):
		switch(m_modifyState)
		{
		case(NEAUTRALSing):
			m_modifyState=DRAGGINGSing;
			m_beforePoint=point;
			break;
		case(MOVEREFSing):
			m_newPoint=point;
			double beforex,beforey,newx,newy;
			TranslateScreenToWorld(double(m_beforePoint.x),double(m_beforePoint.y),
				beforex,beforey);
			TranslateScreenToWorld(double(m_newPoint.x),double(m_newPoint.y),
				newx,newy);
			moveref(beforex,beforey,newx,newy);
			m_modifyState=NEAUTRALSing;
			m_beforePoint=m_newPoint;
			Invalidate(FALSE);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	CPixelViewer::OnLButtonDown(nFlags, point);
}

/*
afx_msg void singleImageView::selectVtx(CPoint point)
{
	double x, y;
	TranslateScreenToWorld(double(point.x), double(point.y), x, y);
	//Point2 min = m_BBox.min, max = m_BBox.max;
	//
	/ *if(x < min.x || y < min.y || x > max.x || y > max.y)
	{
		m_selVtx = -1;
		return;
	}* /
	double  xpos = 0.0, ypos = 0.0;
	double  dist = 0.0, min_dist = 9000.0*9000.0;
	int n = m_pPolyline->GetVertexSum();

	for (int i = 0; i < n; i++)
	{
		m_vtxs[i]->GetPos(xpos, ypos);
		dist = (x-xpos)*(x-xpos) + (y-ypos)*(y-ypos);

		if (dist < min_dist)
		{
			min_dist = dist;
			m_selVtx = i;
		}
	}

	m_vtxs[m_selVtx]->GetPos(xpos, ypos);
	dist = (x-xpos)*(x-xpos) + (y-ypos)*(y-ypos);
	if (dist >= (m_t*m_t))
		m_selVtx = -1;
	else
	{
		GraphicPoint* pGraphicPoint=NULL;
		pGraphicPoint=m_vtxs[m_selVtx];
		pGraphicPoint->SetPos(x,y);
		m_pPolyline->ModifyVertex(m_selVtx,x,y);

		m_pPointLayer->ModifyGraphicObject(pGraphicPoint);
		m_pPolylineLayer->ModifyGraphicObject(m_pPolyline);
		Invalidate(FALSE);
	}
}*/

/*
afx_msg void singleImageView::moveVtx(CPoint point1, CPoint point0)
{
	if (m_selVtx < 0)
		return;
	double x0 = 0.0, y0	= 0.0;
	double x1 = 0.0, y1	= 0.0;
	double dx = 0.0, dy	= 0.0;
	double pos_x = 0.0, pos_y = 0.0;
	GraphicPoint* pGraphicPoint = NULL;

	TranslateScreenToWorld(double(point1.x), double(point1.y), x1, y1);
	TranslateScreenToWorld(double(point0.x), double(point0.y), x0, y0);

	dx = x1 - x0;
	dy = y1 - y0;
	pGraphicPoint = m_vtxs[m_selVtx];
	pGraphicPoint->GetPos(pos_x, pos_y);

	pos_x += dx;		pos_y += dy;

	pGraphicPoint->SetPos(pos_x, pos_y);
	//m_pPolyline->ModifyVertex(m_selVtx, pos_x, pos_y);

	//m_pPointLayer->ModifyGraphicObject(pGraphicPoint);
	m_pPolylineLayer->ModifyGraphicObject(m_pPolyline);

	Invalidate(FALSE);
}*/

//������Ӵ��룬�������n����֪��ά����ĵ�;
afx_msg void singleImageView::addPoint(int n,double *x,double *y,double *z)
{

}

void singleImageView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rc;
	GetClientRect(&rc);
	point.y=rc.Height()-point.y;
	double x, y;
	TranslateScreenToWorld(double(point.x), double(point.y), x, y);

	if (!m_isPlaneDecided)
	{
		MessageBox(_T("ƽ��δ��ʼ��"));
		CPixelViewer::OnLButtonUp(nFlags, point);
		return;
	}

	if (m_Initialized&&m_procedure == DRAWSing)
	{
		switch (m_step)
		{
		case NoneSing:
			break;
		case NewVectorSing:
			switch (m_tool)
			{
			case POLYLINESing:
				//0712 ��
				m_editedEntity = new polylineSing(m_pPointLayer, m_pPolylineLayer, this,
					&m_mainDirection,m_verticalState);
				if (POLYGONONSing==m_polygonMode)
				{
					m_editedPolygon=new polygonSing(m_pPointLayer, m_pPolylineLayer, this,
						&m_mainDirection);
					m_editedPolygon->addEntity(m_editedEntity);
					m_entities.push_back(m_editedPolygon);
				}
				else
				{
					m_entities.push_back(m_editedEntity);
				}
				m_editedEntity->leftButtonUp(x, y);
				m_editedEntity->setLastPoint(&m_lastPoint);
				m_step = InEntitySing;
				break;
				//����

			case ARCSing:
				//0712 ��
				m_editedEntity = new arcSing(m_pPointLayer, m_pPolylineLayer, this,
					&m_mainDirection);
				if (POLYGONONSing==m_polygonMode)
				{
					m_editedPolygon=new polygonSing(m_pPointLayer, m_pPolylineLayer, this,
						&m_mainDirection);
					m_editedPolygon->addEntity(m_editedEntity);
					m_entities.push_back(m_editedPolygon);
				}
				else
				{
					m_entities.push_back(m_editedEntity);
				}
				//m_editedEntity->addDominantPoint(x, y);
				m_editedEntity->leftButtonUp(x, y);
				m_editedEntity->setLastPoint(&m_lastPoint);
				m_step = InEntitySing;
				break;
				//����

			case CIRCLESing:
				m_editedEntity = new circleSing(m_pPointLayer, m_pPolylineLayer, this,
					&m_mainDirection);
				m_entities.push_back(m_editedEntity);
				//m_editedEntity->addDominantPoint(x, y);
				m_editedEntity->leftButtonUp(x, y);
				m_editedEntity->setLastPoint(&m_lastPoint);
				m_step = InEntitySing;
				break;
			default:
				break;
			}
			break;
		case NewEntitySing:
			if (!m_lastPoint.m_isLastPointSet)
			{
				break;
			}
			else
			{
				switch (m_tool)
				{
				case POLYLINESing:
					//0712 ��
					m_editedEntity = new polylineSing(m_pPointLayer, m_pPolylineLayer, this,
						&m_lastPoint,&m_mainDirection,m_verticalState);
					//m_editedEntity->addDominantPoint(x, y);
					m_editedEntity->leftButtonUp(x, y);
					m_editedEntity->setLastPoint(&m_lastPoint);
					if (POLYGONONSing==m_polygonMode)
					{
						m_editedPolygon->addEntity(m_editedEntity);
						m_editedPolygon->calculateCloseLine();
					}
					else
					{
						m_entities.push_back(m_editedEntity);
					}
					m_step = InEntitySing;
					break;
					//�޸Ľ���

				case ARCSing:
					//0712 ��
					m_editedEntity = new arcSing(m_pPointLayer, m_pPolylineLayer, this,
						&m_lastPoint,&m_mainDirection);
					//m_editedEntity->addDominantPoint(x, y);
					m_editedEntity->leftButtonUp(x, y);
					m_editedEntity->setLastPoint(&m_lastPoint);
					if (POLYGONONSing==m_polygonMode)
					{
						m_editedPolygon->addEntity(m_editedEntity);
						m_editedPolygon->calculateCloseLine();
					}
					else
					{
						m_entities.push_back(m_editedEntity);
					}
					m_step = InEntitySing;
					break;
					//�޸Ľ���

				case CIRCLESing:
					m_editedEntity = new circleSing(m_pPointLayer, m_pPolylineLayer, this,
						&m_lastPoint, &m_mainDirection);
					m_entities.push_back(m_editedEntity);
					//m_editedEntity->addDominantPoint(x, y);
					m_editedEntity->leftButtonUp(x, y);
					m_editedEntity->setLastPoint(&m_lastPoint);
					m_step = InEntitySing;
					break;
				default:
					break;
				}
			}
			break;
		case InEntitySing:
			//m_editedEntity->addDominantPoint(x, y);
			//0712 ��
			m_editedEntity->leftButtonUp(x, y);
			m_editedEntity->setLastPoint(&m_lastPoint);
			if (POLYGONONSing==m_polygonMode)
			{
				m_editedPolygon->calculateCloseLine();
			}
			break;
			//�޸Ľ���

		default:
			break;
		}
		Invalidate(FALSE);
	}
/*

		if (m_tmpPointSize+1>=defaultPointSize||m_lineNum>=defaultLineSize)
		{
			return;
		}
		if (m_firstPoint)
		{
			if (m_firstLine)
			{
				m_vtxs=new GraphicPoint*[defaultPointSize];
				m_plines=new GraphicPolyline*[defaultLineSize];
				m_firstLine=false;
			}
			m_pPolyline=
				(GraphicPolyline*)m_pPolylineLayer->CreateGraphicObject(GO_POLYLINE);
			m_pPolyline->SetId(UINT_PTR(m_pPolyline));
			m_plines[m_lineNum]=m_pPolyline;
			m_firstPoint=false;
			m_pPolyline->SetClosed(true);
			m_pPolylineLayer->AddGraphicObject(m_pPolyline,RGB(255,255,0));
			m_tmpLineIndex.m_firstIndex=m_tmpPointSize;
			double x,y;
			TranslateScreenToWorld(double(point.x), double(point.y), x, y);
			setProperPoint(x,y,x,y);
			m_twoPoints.setNextPoint(x,y);
			m_pPolyline->AddVertex(m_tmpLinePointSize,x,y);
			m_pPolyline->AddVertex(m_tmpLinePointSize+1,x,y);
			m_vtxs[m_tmpPointSize]=
				(GraphicPoint*)m_pPointLayer->CreateGraphicObject(GO_POINT);
			m_vtxs[m_tmpPointSize]->SetId(UINT_PTR(m_vtxs[m_tmpPointSize]));
			m_vtxs[m_tmpPointSize]->SetPos(x,y);
			m_pPointLayer->AddGraphicObject(m_vtxs[m_tmpPointSize],RGB(255,255,0));
			m_tmpPointSize++;
			m_drawingLine=true;
			Invalidate(FALSE);
		}
		else
		{
			double x,y;
			TranslateScreenToWorld(double(point.x), double(point.y), x, y);
			m_pPolyline->DeleteVertex(m_tmpLinePointSize);
			setProperPoint(x,y,x,y);
			m_twoPoints.setLastPoint(x,y);
			m_pPolyline->AddVertex(m_tmpLinePointSize,x,y);
			m_pPolyline->AddVertex(m_tmpLinePointSize+1,x,y);
			m_vtxs[m_tmpPointSize]=
				(GraphicPoint*)m_pPointLayer->CreateGraphicObject(GO_POINT);
			m_vtxs[m_tmpPointSize]->SetId(UINT_PTR(m_vtxs[m_tmpPointSize]));
			m_vtxs[m_tmpPointSize]->SetPos(x,y);
			m_pPointLayer->AddGraphicObject(m_vtxs[m_tmpPointSize],RGB(255,255,0));
			double lastx,lasty;
			m_vtxs[m_tmpPointSize-1]->GetPos(lastx,lasty);
			double X,Y,Z,lastX,lastY,lastZ;
			singleImageCalculate(x,y,
				m_mainDirection.m_dx,m_mainDirection.m_dy,m_mainDirection.m_dz,
				m_x0,m_y0,m_z0,
				X,Y,Z);
			singleImageCalculate(lastx,lasty,
				m_mainDirection.m_dx,m_mainDirection.m_dy,m_mainDirection.m_dz,
				m_x0,m_y0,m_z0,
				lastX,lastY,lastZ);
			//m_currentDirection.setDirection(X-lastX,Y-lastY,Z-lastZ);
			setCurrentDirection(X-lastX,Y-lastY,Z-lastZ);
			m_currDireIndex[0]=m_tmpPointSize-1;
			m_currDireIndex[1]=m_tmpPointSize;
			m_tmpPointSize++;
			Invalidate(FALSE);
		}
	}*/
	CPixelViewer::OnLButtonUp(nFlags, point);
}


void singleImageView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_procedure == DRAWSing&&m_editedEntity)
	{
		//0712 ��
		switch(m_step)
		{
		case NoneSing:
			break;
		case NewVectorSing:
			break;
		case NewEntitySing:
			m_editedEntity=NULL;
			m_step=NewVectorSing;
			if (POLYGONONSing==m_polygonMode)
			{
				m_editedPolygon->endDrawing();
				m_editedPolygon=NULL;
			}
			break;
		case InEntitySing:
			if (!m_editedEntity->canEndDrawing())
			{
				MessageBox(_T("����ɻ滭"));
				return;
			}
			m_editedEntity->rightButtonDown(0, 0);
			if (POLYGONONSing==m_polygonMode)
			{
				m_editedPolygon->endDrawing();
				m_editedPolygon=NULL;
			}
			m_editedEntity = NULL;
			m_step = NewVectorSing;
		}
		//����

		Invalidate(FALSE);
		

		//////////////////////////////////////////////////////////////////////////
		//0712�������ͼ�ε����� Li
		DrawEntityInSU(); //�������ж����������


		//////////////////////////////////////////////////////////////////////////

	}

/*
	if (m_procedure==DRAW&&m_drawingLine==true)
	{
		m_pPolyline->DeleteVertex(m_tmpLinePointSize);
		m_drawingLine=false;
		m_firstPoint=true;
		m_tmpLineIndex.m_lastIndex=m_tmpPointSize-1;
		m_linesIndex.push_back(m_tmpLineIndex);
		m_lineNum++;
		m_tmpLinePointSize=0;
		m_isCurrentDirectionSet=false;
		m_twoPoints.initial();
		Invalidate(FALSE);
	}*/
	CPixelViewer::OnRButtonUp(nFlags, point);
}

void singleImageView::changeMode()
{
	if (m_procedure==DRAWSing)
	{
		m_procedure=MODIFYSing;
	}
	else
	{
		m_procedure=DRAWSing;
	}
}

//������֪ƽ�淽�̼���������ά����;
//x,yΪ�������������;
//A,B,C,x0,y0,z0Ϊƽ�淽�̲���;
//X,Y,ZΪ�������ά����;
void singleImageView::singleImageCalculate(double x,double y,
	double A,double B,double C, double x0,double y0,double z0,
	double &X,double&Y,double &Z)
{
	//double ix,iy;
	if (!m_pAerialPhoto)
	{
		return;
	}
/*
	m_pAerialPhoto->PixelCS2ImageCS(x,y,ix,iy);
	const CAerialPhoto::EXTORI* ext=m_pAerialPhoto->GetExtOri();
	double Xs=ext->POS[0];
	double Ys=ext->POS[1];
	double Zs=ext->POS[2];
	double a1=ext->matrix[0];
	double b1=ext->matrix[1];
	double c1=ext->matrix[2];
	double a2=ext->matrix[3];
	double b2=ext->matrix[4];
	double c2=ext->matrix[5];
	double a3=ext->matrix[6];
	double b3=ext->matrix[7];
	double c3=ext->matrix[8];
	double f=m_pAerialPhoto->GetCamera()->GetFocalLength()/1000;
	double tmp1=(a1*ix+a2*iy-a3*f)/(c1*ix+c2*iy-c3*f);
	double tmp2=(b1*ix+b2*iy-b3*f)/(c1*ix+c2*iy-c3*f);
	Z=(A*tmp1*Zs-A*Xs+A*x0+B*tmp2*Zs-B*Ys+B*y0+C*z0)/
		(A*tmp1+B*tmp2+C);
	X=(Z-Zs)*tmp1+Xs;
	Y=(Z-Zs)*tmp2+Ys;*/
	double X1,Y1,Z1,X2,Y2,Z2;
	Z1=z0;
	Z2=z0+100;
	m_pAerialPhoto->PixelRayIntersect(x,y,Z1,X1,Y1);
	m_pAerialPhoto->PixelRayIntersect(x,y,Z2,X2,Y2);
	double a=X2-X1;
	double b=Y2-Y1;
	double c=Z2-Z1;
	double tmp1=a/c;
	double tmp2=b/c;
	double Zs=Z1;
	double Xs=X1;
	double Ys=Y1;
	Z=(A*tmp1*Zs-A*Xs+A*x0+B*tmp2*Zs-B*Ys+B*y0+C*z0)/
		(A*tmp1+B*tmp2+C);
	X=(Z-Zs)*tmp1+Xs;
	Y=(Z-Zs)*tmp2+Ys;
}

void singleImageView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//���ţ���ݼ���������ͬ ����������PixelViewerView.cpp��
	if(nChar == 'Z'||nChar == 'X')
	{
		double Zoom = 1.0;

		if (nChar == 'Z')
		{
			Zoom = 8;
		}
		else
		{
			Zoom = -8;
		}

		ZoomBy(Zoom);
		Invalidate(FALSE);
	}


// 	if (nChar=='N')
// 	{
// 		deleteAll();
// 	}
// 	if (nChar=='I')
// 	{
// 		setPlane(0, 0, 1, 222139.0460, 2528339.1768, 66.2458);
// 	}
// 	if (nChar=='V')
// 	{
// 		changeVertical();
// 	}
// 	if(nChar=='M')
// 	{
// 		changeMode();
// 	}
// 	if (nChar=='R')
// 	{
// 		changeTool(ARCSing);
// 	}
// 	if (nChar=='P')
// 	{
// 		changeTool(POLYLINESing);
// 	}
// 	if (nChar=='C')
// 	{
// 		changeTool(CIRCLESing);
// 	}
	CPixelViewer::OnKeyDown(nChar, nRepCnt, nFlags);
}

afx_msg void singleImageView::deleteAll()
{
	if (!(m_pPointLayer&&m_pPolylineLayer))
	{
		return;
	}
	m_pPointLayer->ClearContent();
	m_pPolylineLayer->ClearContent();
/*
	m_firstLine=true;
	m_firstPoint=true;
	m_tmpPointSize=0;
	m_tmpLinePointSize=0;
	m_drawingLine=false;*/
	Invalidate(FALSE);
}

/*
void singleImageView::setProperPoint(double x,double y,double &properX,double &properY)
{
	switch(m_virticalState)
	{
	case(OFF):
		properX=x;
		properY=y;
		break;
	case(ONEDIRECTION):
		if (m_isCurrentDirectionSet&&
			m_isPlaneDecided&&m_twoPoints.m_isLastPointSet)
		{
			double dx,dy,dz;
			m_currentDirection.crossProduct(m_mainDirection.m_dx,
				m_mainDirection.m_dy,m_mainDirection.m_dz,
				dx,dy,dz);
			double length=sqrt(dx*dx+dy*dy+dz*dz);
			dx=dx/length;
			dy=dy/length;
			dz=dz/length;
			double X,Y,Z,lastX,lastY,lastZ;
			singleImageCalculate(x,y,
				m_mainDirection.m_dx,m_mainDirection.m_dy,m_mainDirection.m_dz,
				m_x0,m_y0,m_z0,
				X,Y,Z);
			singleImageCalculate(m_twoPoints.m_lastx,m_twoPoints.m_lasty,
				m_mainDirection.m_dx,m_mainDirection.m_dy,m_mainDirection.m_dz,
				m_x0,m_y0,m_z0,
				lastX,lastY,lastZ);
			double dX=X-lastX;
			double dY=Y-lastY;
			double dZ=Z-lastZ;
			double projection=dX*dx+dY*dy+dZ*dz;
			dX=dx*projection;
			dY=dy*projection;
			dZ=dz*projection;
			X=lastX+dX;
			Y=lastY+dY;
			Z=lastZ+dZ;
			m_pAerialPhoto->ObjectCS2PixelCS(X,Y,Z,properX,properY);

		}
		else
		{
			properX=x;
			properY=y;
		}
		break;
	case(TWOPOINT):
		properX=x;
		properY=y;
		break;
	}
}*/


void singleImageView::moveref(double oldx,double oldy,double newx,double newy)
{
	for (int i = 0; i < m_entities.size();i++)
	{
		m_entities[i]->moveRef(oldx, oldy, newx, newy);
	}

/*
	for (int i=0;i<m_tmpPointSize;i++)
	{
		double x,y;
		GraphicPoint*pt= m_vtxs[i];
		pt->GetPos(x,y);

		if (sqrt((oldx-x)*(oldx-x)+(oldy-y)*(oldy-y))<MOVEREFTHRESH)
		{
			//������ڻ滭����ֻ�ܶԵ�ǰ�����޸�;
			if (m_drawingLine)
			{
				pt->SetPos(newx,newy);
				int lineIndex=m_tmpPointSize-i;
				if (lineIndex==0)
				{
					m_twoPoints.setNextPoint(newx,newy);
				}
				m_pPolyline->ModifyVertex(lineIndex,newx,newy);
				m_pPolylineLayer->ModifyGraphicObject(m_pPolyline);
				m_pPointLayer->ModifyGraphicObject(pt);
				if (i==m_currDireIndex[0])
				{
					double newX,newY,newZ;
					singleImageCalculate(newx,newy,
						m_mainDirection.m_dx,m_mainDirection.m_dy,m_mainDirection.m_dz,
						m_x0,m_y0,m_z0,
						newX,newY,newZ);
					GraphicPoint* lastPt=m_vtxs[m_currDireIndex[1]];
					double lastx,lasty,lastX,lastY,lastZ;
					lastPt->GetPos(lastx,lasty);
					singleImageCalculate(lastx,lasty,
						m_mainDirection.m_dx,m_mainDirection.m_dy,m_mainDirection.m_dz,
						m_x0,m_y0,m_z0,
						lastX,lastY,lastZ);
					m_currentDirection.setDirection(newX-lastX,newY-lastY,newZ-lastZ);
				}
				else if (i==m_currDireIndex[1])
				{
					m_twoPoints.setLastPoint(newx,newy);
					double newX,newY,newZ;
					singleImageCalculate(newx,newy,
						m_mainDirection.m_dx,m_mainDirection.m_dy,m_mainDirection.m_dz,
						m_x0,m_y0,m_z0,
						newX,newY,newZ);
					GraphicPoint* lastPt=m_vtxs[m_currDireIndex[0]];
					double lastx,lasty,lastX,lastY,lastZ;
					lastPt->GetPos(lastx,lasty);
					singleImageCalculate(lastx,lasty,
						m_mainDirection.m_dx,m_mainDirection.m_dy,m_mainDirection.m_dz,
						m_x0,m_y0,m_z0,
						lastX,lastY,lastZ);
					m_currentDirection.setDirection(newX-lastX,newY-lastY,newZ-lastZ);
				}
			}
			else
			{
				pt->SetPos(newx,newy);
				GraphicPolyline* pline=NULL;
				int lineIndex=0;
				for (int j=0;j<m_lineNum;j++)
				{
					if (i>=m_linesIndex[j].m_firstIndex&&
						i<=m_linesIndex[j].m_lastIndex)
					{
						pline=m_plines[j];
						//����������ע��;
						lineIndex=m_linesIndex[j].m_lastIndex-i;
						break;
					}
				}
				pline->ModifyVertex(lineIndex,newx,newy);
				m_pPolylineLayer->ModifyGraphicObject(pline);
				m_pPointLayer->ModifyGraphicObject(pt);
			}
		}
	}*/
}

CPoint singleImageView::getNearestRef(double x,double y,double &minDist)
{
	minDist = 999;
	CPoint newPt;
	for (int i = 0; i < m_entities.size();i++)
	{
		double tprx, tpry, tpDist;
		m_entities[i]->getNearestRef(x, y, tprx, tpry,tpDist);
		if (tpDist<minDist)
		{
			minDist = tpDist;
			double sx, sy;
			TranslateWorldToScreen(tprx, tpry, sx, sy);
			newPt.x = sx;
			newPt.y = sy;
		}
	}
	return newPt;
/*

	double minDist=100000;
	for (int i=0;i<m_tmpPointSize;i++)
	{
		GraphicPoint* ppt=m_vtxs[i];
		double ptx,pty;
		ppt->GetPos(ptx,pty);
		double dist=sqrt((x-ptx)*(x-ptx)+(y-pty)*(y-pty));
		if (dist<minDist)
		{
			minDist=dist;
			index=i;
		}
	}
	return minDist;*/
}

void singleImageView::changeTool(TOOLSing tool)
{
	//0715 Li ��Ӷ���ι���״̬���û�����ֱ�ӵ��ù��濪��switch
// 	switch (tool)
// 	{
// 	case POLYLINESing:
// 		if (m_polygonMode == POLYGONONSing && (m_step ==  NoneSing|| m_step == NewVectorSing))
// 		{
// 			switchPolygonMode();
// 		}
// 		else if (m_polygonMode == POLYGONOFFSing)
// 		{
// 
// 		}
// 		else
// 		{
// 			MessageBox(L"��δ��ɻ��ƣ��������л����ߣ�");
// 		}
// 		break;
// 
// 	case POLYGONSing: //�������棬�����Ϳ����޸�tool
// 		//δ�������� �� ����û��ʼ���� �� ������ϣ� �ſ��Կ���
// 		if (m_polygonMode == POLYGONOFFSing && (m_step ==  NoneSing|| m_step == NewVectorSing))
// 		{
// 			switchPolygonMode();
// 			tool = POLYLINESing;
// 		}
// 		else if (m_polygonMode == POLYGONONSing)
// 		{
// 			tool = POLYLINESing;
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
	if (m_editedEntity)
	{
		if (!m_editedEntity->canEndDrawing())
		{
			MessageBox(_T("������ɻ滭"));
			return;
		}
		m_editedEntity->endDrawing();

		if (POLYGONONSing==m_polygonMode)
		{
			m_editedPolygon->calculateCloseLine();
		}
		m_editedEntity = NULL;
		Invalidate(FALSE);
	}
	if (TOOLSing::CIRCLESing==m_tool&&POLYGONONSing==m_polygonMode)
	{
		MessageBox(_T("Բ��ƽ�治��ͬʱʹ��"));
		return;
	}
	m_tool = tool;
	switch (m_step)
	{
	case NoneSing:
		m_step = NewVectorSing;
		break;
	case NewVectorSing:
		break;
	case NewEntitySing:
		break;
	case InEntitySing:
		m_step = NewEntitySing;
		break;
	default:
		break;
	}
	//����
}

void singleImageView::changeVertical()
{
	switch (m_verticalState)
	{
	case OFFSing:
		m_verticalState = ONSing;
		if (m_tool==POLYLINESing&&m_editedEntity)
		{
			polylineSing* line = static_cast<polylineSing*>(m_editedEntity);
			line->setVerticalState(ONSing);
		}
		break;
	case ONSing:
		m_verticalState = OFFSing;
		if (m_tool == POLYLINESing&&m_editedEntity)
		{
			polylineSing* line = static_cast<polylineSing*>(m_editedEntity);
			line->setVerticalState(OFFSing);
		}
		break;
	default:
		break;
	}
}


void singleImageView::getRealCoor(double x, double y, 
	double &X, double &Y, double &Z)
{
	if (!m_isPlaneDecided)
	{
		return;
	}
	singleImageCalculate(x,y,
		m_mainDirection.m_dx, m_mainDirection.m_dy, m_mainDirection.m_dz,
		m_x0, m_y0, m_z0,
		X, Y, Z);
}

void singleImageView::getPointCoor(double X, double Y, double Z, double &x, double &y)
{
	m_pAerialPhoto->ObjectCS2PixelCS(X, Y, Z, x, y);
}

//0712 �� �¼ӵĺ���
//switchPolygonMode  getInformation  clearEntities  getRayByMouse
afx_msg void singleImageView::switchPolygonMode()
{
	if (CIRCLESing==m_tool&&POLYGONOFFSing==m_polygonMode)
	{
		MessageBox(_T("polygonģʽ�������û�Բ"));
		return;
	}
	if (POLYGONONSing==m_polygonMode)
	{
		if (m_editedPolygon)
		{
			MessageBox(_T("���������Ļ滭"));
		}
		else
		{
			m_polygonMode=POLYGONOFFSing;
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
			m_polygonMode=POLYGONONSing;
		}
	}
}

afx_msg void singleImageView::getInformation()
{
	CString tool,procedure,verticalM,polygonM;
	switch(m_tool)
	{
	case POLYLINESing:
		tool="tool: polyline\n";
		break;
	case ARCSing:
		tool="tool: arc\n";
		break;
	case CIRCLESing:
		tool="tool: circle\n";
		break;
	default:
		tool="tool: undefined\n";
		break;
	}

	switch(m_procedure)
	{
	case DRAWSing:
		procedure="procedure: draw\n";
		break;
	case MODIFYSing:
		procedure="procedure: modify\n";
		break;
	}

	switch(m_polygonMode)
	{
	case POLYGONONSing:
		polygonM="polygonMode: on\n";
		break;
	case POLYGONOFFSing:
		polygonM="polygonMode: off\n";
		break;
	}

	switch(m_verticalState)
	{
	case ONSing:
		verticalM="verticalState: on\n";
		break;
	case OFFSing:
		verticalM="verticalState: off\n";
		break;
	}
	CString cst=tool+procedure+verticalM+polygonM;
	MessageBox(cst);
}

afx_msg void singleImageView::clearEntities(LAYERSing layer)
{
	switch(layer)
	{
	case DrawLayer:
		m_pPointLayer->ClearContent();
		m_pPolylineLayer->ClearContent();
		for (int i=0;i<m_entities.size();i++)
		{
			delete m_entities[i];
		}
		m_entities.clear();
		break;
	case ReprojectLayer:
		m_pReprojectLineLayer->ClearContent();
		m_pReprojectPointLayer->ClearContent();
		break;
	case ReprojectPreviewLayer:
		m_pRePreviewLineLayer->ClearContent();
		m_pRePreviewPtLayer->ClearContent();
		break;
	}
	Invalidate(false);
}

afx_msg void singleImageView::getRayByMouse(double &dX,double &dY,double &dZ, 
	double &sourceX,double& sourceY,double& sourceZ)
{
	if (!m_Initialized)
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

	CRect rc;
	GetClientRect(&rc);
	mousePoint.y=rc.Height()-mousePoint.y;
	double x, y;
	TranslateScreenToWorld(double(mousePoint.x), double(mousePoint.y),
		x, y);

	const CAerialPhoto::EXTORI*ext=m_pAerialPhoto->GetExtOri();
	sourceX=ext->POS[0];
	sourceY=ext->POS[1];
	sourceZ=ext->POS[2];
	double newX,newY,newZ;
	newZ=sourceZ/2;
	m_pAerialPhoto->PixelRayIntersect(x,y,newZ,newX,newY);
	dX=newX-sourceX;
	dY=newY-sourceY;
	dZ=newZ-sourceZ;
}





//0712 ����CA����SU�л���ģ��
void singleImageView::DrawEntityInSU()
{
	//����Ӧ�����ж��Ƿ���
	//Բ�����������
	//����ߡ����β�����

	if (m_tool == POLYLINESing) //�����
	{
		//��Ҫ���ж��Ƿ��棬��ʵ�Ѿ�û��ʵ���ϵ�polygon������
		if (m_polygonMode == POLYGONONSing) //����
		{

		}



		//polylineSing* pPLine = (polylineSing*)m_entities.back();

		//polylineSingData * pPlineData = (polylineSingData*)pPLine->getData();


		//����SUCAssist�⽫���������SU���л���


		//delete pPlineData;


	}
	else if (m_tool == ARCSing)
	{
		//Ҳ��Ҫ�ж��Ƿ���


	}
	else if (m_tool == CIRCLESing)
	{

	}
// 	else if (m_tool == POLYGONSing)
// 	{
// 
// 	}

}


//0714 ��ʾ�����ص�ǰ�Ļ�׼�����
void singleImageView::ShowCurrentDatum(std::vector<double> & gridStartCoor, 
	std::vector<double> & gridEndCoor)
{
	if (gridStartCoor.size() == 0)
	{
		return;
	}

	//���֮ǰ��
	for (int i=0;i<m_vDatumGird.size();i++)
	{
		m_vDatumGird[i]->DeleteVertexs(0,1);//��Щ����ʵֻ��������
	}

	m_vDatumGird.clear();

	for (int i=0;i<(gridStartCoor.size()/3);i++)
	{
		double xt ,yt, xf, yf;
		double fromPnt_X, fromPnt_Y, fromPnt_Z;
		double toPnt_X, toPnt_Y, toPnt_Z;

		//ע�⣺������﷽���������������ֱ�Ӽ������������Ҫ��ƫ����
		fromPnt_X = gridStartCoor[3*i]; fromPnt_Y = gridStartCoor[3*i+1]; fromPnt_Z = gridStartCoor[3*i+2];
		toPnt_X = gridEndCoor[3*i]; toPnt_Y = gridEndCoor[3*i+1]; toPnt_Z = gridEndCoor[3*i+2] ;

		//����world ��������
		m_pAerialPhoto->ObjectCS2PixelCS(fromPnt_X,fromPnt_Y,fromPnt_Z,xf,yf);
		m_pAerialPhoto->ObjectCS2PixelCS(toPnt_X,toPnt_Y,toPnt_Z, xt, yt);

		//�����
		GraphicPolyline *pDatumLine;

		//��Ҫ��ʼ���������ͼ���Ժ���Ҫ�ģ�����
		pDatumLine = (GraphicPolyline*)m_pPolylineLayer->CreateGraphicObject(GO_POLYLINE);
		pDatumLine->SetId(UINT_PTR(pDatumLine));
		m_pPolylineLayer->AddGraphicObject(pDatumLine,RGB(0,255,0));

		pDatumLine->AddVertex(xf,yf);
		pDatumLine->AddVertex(xt,yt);
		m_vDatumGird.push_back(pDatumLine);
	}

}

void singleImageView::CloseCurrentDatum()
{
	//���֮ǰ��
	for (int i=0;i<m_vDatumGird.size();i++)
	{
		m_vDatumGird[i]->DeleteVertexs(0,1);//��Щ����ʵֻ��������
	}

	m_vDatumGird.clear();
}