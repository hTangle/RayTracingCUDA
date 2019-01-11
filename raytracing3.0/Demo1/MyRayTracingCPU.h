#pragma once
#include "RayTracing.h"

class RayTracingCPU :public RayTracing {
public:
	vector<vector<double>> initCUDAInput(double RX_X, double RX_Y);
	void updateTx(double x, double y);
	void updateRx(double x, double y);
	void init();
public:
	RayTracingCPU();
	~RayTracingCPU();
private:
	double directionIntersectCPU(double x1, double y1, double x2, double y2, double x3, double y3);
	double MyminCPU(double x1, double x2);
	double MymaxCPU(double x1, double x2);
	double calDistanceCPU(double x1, double y1, double x2, double y2);
	bool onSegmentCPU(double x1, double y1, double x2, double y2, double x3, double y3);
	void calculPointOfIntersectionCPU(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double &x, double &y);
	bool segmentsIntersectCPU(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
	void normalizeVectorCPU(double &x, double &y);
	double calculDistancePointAndLineCPU(double x1, double y1, double x2, double y2, double x0, double y0);
	bool isEndPointCPU(Edge edge, double x, double y);
	void judegTouchedDiffractionCPU(MyVector *vector, Point *point, int currentPointsIndex);
	void judgeIsTouchedCPU(Point *point, int currentPointsIndex);
	//vector<vector<double>> initCUDAInputCPU(Grids *grids, double TX_X, double TX_Y, double RX_X, double RX_Y, int N);
};