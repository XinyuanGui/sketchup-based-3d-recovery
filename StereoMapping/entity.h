#pragma once
#include <vector>
class lastPoint;
class CEditLine;
class CStereoViewDlg;
enum NearestSide{LEFT,RIGHT};
/************************************************************************/
/* ����Ϊ����entity���࣬Dominant�д����˾�������״�ĵ㣬LinePoints�д���
�����������õĵ㣬�Ǹ���Dominant�������*/
/************************************************************************/
class entity
{
public:
	entity(void);
	enum entityClassify{POLYGON,POLYLINE,ARC,CIRCLE,BUILDING};
	virtual void moveMouse(double lx,double ly,double rx,double ry)=0;
	virtual void leftButtonDown(double lx,double ly,double rx,double ry)=0;
	virtual void leftButtonUp(double lx,double ly,double rx,double ry)=0;
	virtual void rightButtonDown(double lx,double ly,double rx,double ry)=0;
	virtual void getNearestRef(double lx,double ly,double rx,double ry,
		double &rlx,double &rly,double &rrx,double &rry,double &minDist,
		NearestSide& side)=0;
	virtual void moveRef(double oldlx, double oldly, double oldrx, double oldry,
		double newlx, double newly, double newrx, double newry)=0;
	virtual void endDrawing() = 0;
	virtual void getRealCoor(double lx, double ly, double rx, double ry, CStereoViewDlg* pDlg,
		double &X, double &Y, double &Z);
	virtual void getPointCoor(double X, double Y, double Z, CStereoViewDlg* pDlg,
		double &lx, double &ly, double &rx, double &ry);
	//����local���������Ļ�ϵľ���;
	double screenDist(double x1, double y1, double x2, double y2, NearestSide side,CStereoViewDlg* pDlg);
	virtual void outputFirstPoint(double &X, double &Y, double &Z, CStereoViewDlg* pDlg);
	virtual entityClassify getClassify()=0; 
	virtual void clearData()=0;//����������ɾ��;
	virtual void* getData()
	{
		return NULL;
	}
	virtual bool canEntityFinish()
	{
		return true;
	}
	~entity(void);
};

class entityAlone:public entity
{
public:
	friend class polygon;
	entityAlone(CEditLine* pLLine,CEditLine* pRLine);
	//�����Ѿ��е�ʱ��ǰһ������빹��;
	entityAlone(CEditLine* pLLine,CEditLine* pRLine,
		lastPoint* lastPt);
    //ע����й�һ��;
	virtual void addDominantPoint(double lx,double ly,double rx,double ry)=0;
	virtual void addPreviewPoint(double lx,double ly,double rx,double ry)=0;
	virtual void outputFirstPoint(double &X, double &Y, double &Z, CStereoViewDlg* pDlg);
	virtual void setLastPoint(lastPoint* lastPt);
	//virtual void setFirstPoint(lastPoint* firstPt);
	virtual void getFirstPoint(double &lx,double& ly,double &rx,double &ry);//��ȡ��һ��������;
	virtual void getLastPoint(double &lx,double& ly,double &rx,double &ry);//��ȡ���һ��������(��Ԥ��������Ԥ��);
	virtual void clearData();

	//����Ԫ�ػ滭;
	virtual ~entityAlone();
protected:
	CEditLine* m_pLDominant;
	CEditLine* m_pRDominant;
	double m_previewLx;
	double m_previewLy;
	double m_previewRx;
	double m_previewRy;
	bool m_isPreviewExisted;
	CEditLine* m_pLLinePoints;
	CEditLine* m_pRLinePoints;
};