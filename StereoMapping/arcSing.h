#pragma once
#include "entitySing.h"
#define PI 3.1415926535898
#define RADIOSTEP 0.261799
struct arcSingData
{
	double m_centerx;
	double m_centery;
	double m_centerX;
	double m_centerY;
	double m_centerZ;
	double m_r;
	double m_arc;
	double m_dominantPx[3];
	double m_dominantPy[3];
	double m_dominantPX[3];
	double m_dominantPY[3];
	double m_dominantPZ[3];
};
class arcSing
	:public entitySing
{
public:
	arcSing(GraphicLayer* pPointLayer, GraphicLayer* pLineLayer,
		singleImageView* pView, directionSing* verticalD);
	arcSing(GraphicLayer* pPointLayer, GraphicLayer* pLineLayer,
		singleImageView* pView, lastPointSing* lastP, directionSing* verticalD);
	virtual void moveMouse(double x, double y);
	virtual void leftButtonDown(double x, double y){};
	virtual void leftButtonUp(double x, double y);
	virtual void rightButtonDown(double x, double y);
	virtual void getNearestRef(double x, double y,
		double &resultx, double &resulty, double &minDist);
	virtual void moveRef(double oldx, double oldy, double newx, double newy);
	virtual void endDrawing();
	virtual void addDominantPoint(double x, double y);
	virtual void addPreviewPoint(double x, double y);
	virtual void setVertical(directionSing verticalD, double x0, double y0, double z0);//�Է�������������;
	virtual void calculateArc();
	//�������в�������LinePoints�еĵ㣬��ʱ��Ϊ������ˮƽ��;
	virtual void calculateLinePoins();
	virtual void* getData();
	virtual bool canEndDrawing();
	virtual classify getClassify()
	{
		return classify::arc;
	}
	virtual ~arcSing();
protected:
	//std::vector<GraphicPoint*> m_middlePoints;//��ȥ��β����м��;
	double m_cx;
	double m_cy;
	double m_cz;//Բ����ʵ��ά����;
	double m_averZ;
	double m_averX;
	double m_averY;//Բ����ƽ�����꣨���Ļ���
	double m_A;
	double m_B;
	double m_C;
	double m_D;//ƽ�淽��;AX+BY+CZ+D=0
	double m_r;
	double m_X[3];
	double m_Y[3];
	double m_Z[3];//����ȷ�����̵����������ά����;
	int m_linePoints;//��m_line�е�ĸ���;
	bool m_isDrawing;
	double m_fai;//����;
};

