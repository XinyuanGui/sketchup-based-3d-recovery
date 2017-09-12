#pragma once
#include "PixelViewerView.h"
#include "charaTrans.h"
#include <vector>
//#include "AerialBase.h"
// singleImageView ��ͼ
#define MOVEREFTHRESH 5
#define BIGTHRESH 10
//����ѡ��ʱ����ֵ�������ʵ�ѡ��һЩ��
class GraphicPolyline;
class GraphicPoint;
class GraphicLayer;
class CAerialPhoto;
class polygonSing;
//NoneΪ��ǰû�н����κβ���;
//NewVectorΪ�����µ�Vector�εĲ���;
//NewEntityΪ�л������µ�Tool��ǰ���Ѿ��е�;
//InEntityΪ���ڽ��л滭����;
enum DRAWSTEPSing{NoneSing,NewVectorSing,NewEntitySing,InEntitySing};
enum TOOLSing{ POLYLINESing, ARCSing, CIRCLESing };
enum procedureSing{DRAWSing,MODIFYSing};//��ʱ���ڻ滭�����޸�״̬;
enum verticalStateSing{OFFSing,ONSing};
enum DRAGSing{DRAGGINGSing,MOVEREFSing,NEAUTRALSing};//��ǰ��modify״̬;

//0712 ��
enum LAYERSing{DrawLayer,ReprojectLayer,ReprojectPreviewLayer};
enum POLYGONMODESing{POLYGONONSing,POLYGONOFFSing};
//�Ķ����� 0712

class entitySing;

/*
struct linePointIndex
{
	int m_firstIndex;
	int m_lastIndex;
};*/

//����һ�����������;
struct directionSing
{
	double m_dx;
	double m_dy;
	double m_dz;
	void setDirection(double dx,double dy,double dz)
	{
		m_dx=dx;
		m_dy=dy;
		m_dz=dz;
	}

	//���в�����㲢���й�һ��;
	void crossProduct(double dx,double dy,double dz,
		double &resultX,double &resultY,double &resultZ)
	{
		resultX=m_dy*dz-m_dz*dy;
		resultY=-(m_dx*dz-m_dz*dx);
		resultZ=m_dx*dy-m_dy*dx;
		double length = sqrt(resultX*resultX + resultY*resultY + resultZ*resultZ);
		resultX /= length;
		resultY /= length;
		resultZ /= length;
	}
};

/*//���ڽ���������ݴ�ֱ��ϵȷ���м��Ĳ���;
//����nextPointһ����ÿ���߶εĵ�һ����;
//lastPointΪ�ոյ��µĵ�;

struct lastNextPoint
{
	lastNextPoint()
	{
		m_isLastPointSet=false;
		m_isNextPointSet=false;
		m_isAllSet=false;
	}
	//CPoint m_lastPoint;
	//CPoint m_nextPoint;
	double m_lastx;
	double m_lasty;
	double m_nextx;
	double m_nexty;
	bool m_isLastPointSet;
	bool m_isNextPointSet;
	bool m_isAllSet;
	void setLastPoint(double x,double y)
	{
		//m_lastPoint=point;
		m_lastx=x;
		m_lasty=y;
		m_isLastPointSet=true;
		if (m_isNextPointSet)
		{
			m_isAllSet=true;
		}
	}
	void setNextPoint(double x,double y)
	{
		//m_nextPoint=point;
		m_nextx=x;
		m_nexty=y;
		m_isNextPointSet=true;
		if (m_isLastPointSet)
		{
			m_isAllSet=true;
		}
	}
	void initial()
	{
		m_isLastPointSet=false;
		m_isNextPointSet=false;
		m_isAllSet=false;
	}
};
*/

struct lastPointSing
{
	lastPointSing()
	{
		m_lastPoint = NULL;
		m_isLastPointSet = false;
	}
	GraphicPoint* m_lastPoint;
	bool m_isLastPointSet;
	void getLastPoint(double &x, double &y)
	{
		m_lastPoint->GetPos(x, y);
	}
};

class singleImageView : public CPixelViewer
{
	DECLARE_DYNCREATE(singleImageView)

public:
	singleImageView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~singleImageView();

public:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
private:
	//CString m_proPath;
	GraphicLayer* m_pPointLayer; //ԭ���Ķ����ͼ�������棬���ĳ�public��

	//0712 ��
	GraphicLayer* m_pReprojectPointLayer;
	GraphicLayer* m_pReprojectLineLayer;
	GraphicLayer* m_pRePreviewPtLayer;
	GraphicLayer* m_pRePreviewLineLayer;
	//�޸Ľ���


	GraphicPolyline* m_pPolyline;
	CAerialPhoto* m_pAerialPhoto;
	entitySing* m_editedEntity;
	CPoint m_point0;
	//CAerialBlock m_block;
	CString filePath;
	bool m_Initialized;//�ж��Ƿ��Ѿ���ͼ��·����ʼ��;
	int m_selVtx;//ѡ��Ķ˵��index,-1Ϊ��Ч;
	float m_t;//������ݲ�;
	procedureSing m_procedure;
	TOOLSing m_tool;
	DRAWSTEPSing m_step;
	verticalStateSing m_verticalState;
	lastPointSing m_lastPoint;//���ڴ�һ��tool�л�����һ��tool
	directionSing m_mainDirection;//��ƽ��ķ��߷���;
	bool m_isMainCurrentDirectionSet;//�Ƿ����ƽ�淨�߷���;
	double m_x0;
	double m_y0;
	double m_z0;
	bool m_isPlaneDecided;//��ƽ�淽���Ƿ��Ѿ�ȷ��;
	CPoint m_beforePoint;
	CPoint m_newPoint;
	DRAGSing m_modifyState;//����������״̬;

public:
	//0712 ��
	POLYGONMODESing m_polygonMode;
	polygonSing* m_editedPolygon;
	//�Ķ�����



	/*bool m_firstPoint;//�Ƿ��Ǹ����ߵĵ�һ����;
	bool m_firstLine;//�Ƿ��Ǹ�ͼ��ĵ�һ����;
	int m_tmpPointSize;//Ŀǰ�Ѿ����˼�����;
	int m_tmpLinePointSize;//��¼���������е���;

	//�����봹ֱ���й�;
	bool m_drawingLine;//���ڻ��㣨��Ի���ʱ����ƶ������ã�;
	int m_virticalState;//�жϴ�ֱ��������״̬;
	direction m_currentDirection;//��ǰ�����÷���(����ֱ�ǻ�);
	int m_currDireIndex[2];//��ǰ���÷�����������index;
	direction m_mainDirection;//��ƽ��ķ��߷���;
	lastNextPoint m_twoPoints;//��һ������һ�㣬��������ȷ��һ�����;
	bool m_isCurrentDirectionSet;//�Ƿ���ڵ�ǰ���÷���;
	bool m_isMainCurrentDirectionSet;//�Ƿ����ƽ�淨�߷���;
	double m_x0;
	double m_y0;
	double m_z0;
	bool m_isPlaneDecided;//��ƽ�淽���Ƿ��Ѿ�ȷ��;
	int m_modifyState;//��ǰ��modify����ʲô״̬;
	int m_lineNum;//����ʱע���Ǽ���idΪm_lineNum+1����;
	CPoint m_beforePoint;
	CPoint m_newPoint;
	std::vector<linePointIndex> m_linesIndex;
	linePointIndex m_tmpLineIndex;*/

/*
	void setCurrentDirection(double dx,double dy,double dz)
	{
		m_currentDirection.setDirection(dx,dy,dz);
		m_isCurrentDirectionSet=true;
	}*/
	void setMainDirection(double dx,double dy,double dz)
	{
		m_mainDirection.setDirection(dx,dy,dz);
		m_isMainCurrentDirectionSet=true;
	}

public:
	void setPlane(double A,double B,double C,double x0,double y0,double z0)
	{
		setMainDirection(A,B,C);
		m_x0=x0;
		m_y0=y0;
		m_z0=z0;
		m_isPlaneDecided=true;
	}

	//����x,y������Ŀǰ��verticalState�����ȷ��properX,properY;
	//����������㴹ֱ���һ��������...���Ժ�����ţ�ٵ�������������;
	//void setProperPoint(double x,double y,double &properX,double &properY);

public:
	GraphicLayer* m_pPolylineLayer;
	//GraphicPoint** m_vtxs;
	//GraphicPolyline** m_plines;
	std::vector<entitySing*> m_entities;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void setAerialPhoto(CAerialPhoto* photo)
	{
		m_pAerialPhoto=photo;
	}
	afx_msg bool LoadImages(CString path);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void selectVtx(CPoint point);//ѡ����point����ĵ�;
	//afx_msg void moveVtx(CPoint point1, CPoint point0);
	afx_msg void addPoint(int n,double *x,double *y,double *z);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void changeMode();
	afx_msg void changeTool(TOOLSing tool);
	afx_msg void changeVertical();

	//0712 ��
	afx_msg void switchPolygonMode();
	afx_msg void getInformation();
	//���ڽ�ĳһ��vector�е�entitiesȫ��ɾ��;
	afx_msg void clearEntities(LAYERSing layer);
	afx_msg void getRayByMouse(double &dX,double &dY,double &dZ,
		double &sourceX,double& sourceY,double& sourceZ);
	//�޸Ľ���



	//������֪ƽ�淽�̼���������ά����;
	//x,yΪ�������������;
	//A,B,C,x0,y0,z0Ϊƽ�淽�̲���;
	//X,Y,ZΪ�������ά����;
	afx_msg void singleImageCalculate(double x,double y,double A,double B,double C,
		double x0,double y0,double z0,double &X,double&Y,double &Z);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//������������;
	afx_msg void moveref(double oldx,double oldy,double newx,double newy);
	//���ؾ�����������ص�����ĵ�ľ���;
	afx_msg CPoint getNearestRef(double x,double y,double &minDist/*,int &index*/);
	//ɾ�����е����;
	afx_msg void deleteAll();
	//�����������������Ӧ��ά����;
	afx_msg void getRealCoor(double x, double y, double &X, double &Y, double &Z);
	//������ά���������Ӧ��������;
	afx_msg void getPointCoor(double X, double Y, double Z, double &x, double &y);


	//////////////////////////////////////////////////////////////////////////
	//0712 Li ���
	//ʹ��CA�⣬��SU�л���ģ��
	void DrawEntityInSU();


	//��Ͷ
	//����һ�ζε��﷽ģ����
	std::vector<GraphicPolyline*> m_vObjLine;
	//��̬Ԥ��������
	std::vector<GraphicPolyline*> m_vAuxLine;

	//ƫ���������������ʱ���ȡ
	double m_dXOffset;
	double m_dYOffset;
	double m_dZOffset;

	//��׼�����
	//�Ƿ���ʾ��׼����
	bool m_bShowDatum;
	//��׼�����vector
	std::vector<GraphicPolyline*> m_vDatumGird;

	//��ʾ�����ص�ǰ������ÿ�λ�����Ҫ��˫�������ȡ��Ϣ
	void ShowCurrentDatum(std::vector<double> & gridStartCoor, std::vector<double> & gridEndCoor);
	void CloseCurrentDatum();

	//���������ܺ�ģ���ߡ������߻쵽һ����Ȼ����������ȫһ��
	//��Ϊ����ͬʱ�ػ棬�����и��Ե�vector�ͻ���ˢ�º���
	void DrawGrid(int lineNum,double* x1,double* y1,double* z1,
		double* x2,double* y2,double* z2);


	//0712
	//////////////////////////////////////////////////////////////////////////
};