#pragma once
#include "fxImage.h"
#include "AerialBase.h"
#include "AerialPhotoRender.h"
#include "render.h"
#include "point3.h"
#include "AeriaGeoModel.h"
#include <vector>
#include "resource.h"
#define DraggingThresh 10
#define MoveRefThresh 5
// CStereoViewDlg dialog
class entity;
class polygonBuilding;
class polygon;

enum TOOLS{CIRCLE, ARC, POLYLINE,
	PAN, CUBOID,COPY,RIDGE2,TIN_TRAINGLE,SPHERE,BALCONY,T_WINDOW,PARAPET};

//0627
enum PROCEDURE{DRAW,MODIFY, DATUM};

enum LINESTATE{ROOF,SIDE};
enum SIDESTATE{ROOFPOINT,BOTTOMPOINT};
enum VERTICALMODE{ON,OFF};//��ֱ������;

//0712 ��
enum POLYGONMODE{POLYGONON,POLYGONOFF};//ƽ��滭����;
//�Ķ����� 0712

enum MODIFYSTATE{ DRAGGING, MOVEREF, NEAUTRAL };

//0627
enum DATUMTYPE{NODATUM, HORIZON, VERTICAL, RANDOM};


inline double pointDist(double x1, double y1, double x2, double y2)
{
	double r = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	return r;
}

//�滭������Ҫ����Ϣ;
struct lastPoint
{
	lastPoint()
	{
		m_lLastPt = NULL;
		m_rLastPt = NULL;
		m_isLastPointSet = false;
	}
	CPoint3D* m_lLastPt;
	CPoint3D* m_rLastPt;//�Ѿ�ȷ���Ļ滭�����һ�㣨����ƶ�ʱ����¼��;
	bool m_isLastPointSet;
};


class CStereoViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CStereoViewDlg)

public:
	CStereoViewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStereoViewDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_STEREOVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	CAerialPhoto* m_lpAerialPhoto[2];
	CRender           m_Render;
	CAerialPhotoRender m_LViewer;
	CAerialPhotoRender m_RViewer;
	CAeriaGeoModel     m_AerialGeoModel;
private:
	double m_iniScale;
	bool isLoadImage;
	double m_panStep;//�ֶ����ò���
	CPoint m_MouseLastPoint;//�ƶ�Ӱ�����֮ǰλ��
	double m_panMulti;//�Ӳ�ƽ�Ʊ���

	CEditLayer m_LLineLayer;
	CEditLayer m_RLineLayer;
	CEditLine* m_pLLine;
	CEditLine* m_pRLine;
	entity* m_editedEntity;
	polygonBuilding*m_editedBuilding;

	polygon* m_editedPolygon; //0712 ��



	std::vector<entity*>m_entities;//����entities;





	//drawInformation m_information;
	lastPoint m_lastPt;
	TOOLS              m_tool;//��ģ����
	PROCEDURE m_procedure;
	bool m_isDrawingBuilding;//��������ڻ�building������񽻸���ȥ��;
	//����������ڻ�building�����ڴ�������;
	LINESTATE m_lineState;
	SIDESTATE m_sideState;
	MODIFYSTATE m_modifyState;
	VERTICALMODE m_verticalMode;
	CPoint m_before;
	CPoint m_new;

public:

	//0712 ��
	POLYGONMODE m_polygonMode;
	//�Ķ����� 0712

	int m_flowNum;/*������ʶ,-1Ϊ��δ��ʼ������0Ϊ��δ��ʼvector��1Ϊ�ѿ�ʼvector����δ��ʼ
				  �µ�entity��2Ϊ����entity��,3��Ϊ�Բ�߽��в���*/

private:
	
	//************************************
	// Method:    AddPolygonBuilding
	// FullName:  CStereoViewDlg::AddPolygonBuilding
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: UINT nFlags  ������궯����ʶ,1ΪLButtonDown,2ΪLButtonUp,3ΪmouseMove,4ΪRButtonDown;
	// Parameter: CPoint point  ���������Ļ����
	//************************************
	void AddPolygonBuilding(UINT nFlags, CPoint point);


public:
	void SetAerialPhoto(CAerialPhoto* lpLeftPhoto, CAerialPhoto* lpRightPhoto);
	void LoadImages();
	void CleanAllContent(void);
	void TranslateScreenToLocal(CPoint p, double& lwx, double& lwy, double& rwx, double& rwy);
	void SetTool(TOOLS tool);//���ý�ģ����

//////////////////////////////////////////////////////////////////////////
	//��׼���� 0626 ��

	//��ǰ���Ű��˶��ٴΣ��ж����ŵ����ʱ��ʾ���� 10�����ϲ���ʾ����
	int m_nImgScale;

	//NODATUM ��ʾ��û���׼
	DATUMTYPE m_DatumType;

	//0��δ���  1����һ��  2�������
	int m_nClickMode;

	//�Ƿ���ʾ��׼����
	bool m_bShowDatum;

	std::vector<CEditLine*> m_vLDatumGird;
	std::vector<CEditLine*> m_vRDatumGird;

	CPoint3D m_datumCent; //�������� ��һ��
	CPoint3D m_datumPnt2;  //�ڶ���
	CPoint3D m_datumPnt3;  //������

	double m_dGridSize; //Ĭ��ֵ1��
	double m_dGridRange; //Ĭ��ֵ100��

	//�����׼ʹ�õı���
	double m_dRangeX, m_dRangeY; //��������˵������ֵ����ǰ���������м�
	double m_dSizeX, m_dSizeY; //ÿһ����������ֵ

	//�����׼�����洹ֱ
	double m_dRangeX2, m_dRangeY2, m_dRangeZ;
	double m_dSizeX2, m_dSizeY2, m_dSizeZ; 

	//������ʱӦ����������ǰ�������ÿռ����꣬���ɴ����������������һ�μ���
	//��Щ���������½���׼����ʱ���ã���Ҫ������Ϊ����
	void ShowHorDatumGrid(CPoint mousePnt);
	void ShowVerDatumGrid(CPoint mousePnt2);
	void ShowRanDatumGrid(CPoint mousePnt3);

	void GetSpaceCoor(CPoint point, CPoint3D & resultPnt);

	//���������ܺ�ģ���ߡ������߻쵽һ����Ȼ����������ȫһ��
	//��Ϊ����ͬʱ�ػ棬�����и��Ե�vector�ͻ���ˢ�º���
	void DrawGrid(int lineNum,double* x1,double* y1,double* z1,
		double* x2,double* y2,double* z2);
	//0 1 2 ˮƽ ���� ����
	void ChangeMode2Datum(int nDatumType);
	void ChangeGridSize(bool isEnlarge);

	//��׼������洢����
	std::vector<double> m_vGridStartCoor;
	std::vector<double> m_vGridEndCoor;

	//��ǰ��׼�棺��������ƽ����һ��
	double m_dNormX, m_dNormY, m_dNormZ, m_dX, m_dY, m_dZ;

	//��õ�ǰ��׼�棬�����δ���û�׼�棬����false
	bool GetDatumPlane(double & normX, double & normY, double & normZ, 
		double & X, double & Y, double & Z);


	//����Ӱ���Զ��ɼ����ĵĵ�����
	void GetSpaceCentPnt(CPoint3D & spaceCentPnt);

	//0712 ����CA�⣬��SU�л���ģ��
	void DrawEntityInSU();


	//0714 �ر�/��ʾ ���еĻ�׼����
	void CloseCurrentDatum();
	void ShowCurrentDatum();


	//ƫ���������������ʱ���ȡ
	double m_dXOffset;
	double m_dYOffset;
	double m_dZOffset;


//////////////////////////////////////////////////////////////////////////



	//�޸�ģʽ;
	void moveref(CPoint pointBefore, CPoint pointNew);
	CPoint& getNearestRef(CPoint pointBefore, double &dist);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
//	afx_msg void OnMove(int x, int y);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void switchProcedure();

	public:
	afx_msg void switchVerticalMode();

	//0712 ��
	afx_msg void switchPolygonMode();//����ģʽ�����л�
	afx_msg void getInformation();//��ȡ��ǰ״ֵ̬;
	afx_msg void deleteEntities(std::vector<entity*>&entVec);
	//Ĭ������Ƭ�Ĺ�������ƽ��
	afx_msg void getRayByMouse(double &dX,double &dY,double&dZ,
		double &sourceX,double &sourceY,double &sourceZ);
	//�޸����


	//afx_msg void OnHorizonDatum();
};
