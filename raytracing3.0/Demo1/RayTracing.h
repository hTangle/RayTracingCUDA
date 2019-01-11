#pragma once
#include "MyCUDARayTracing.h"
class RayTracing
{
public:
	bool mapUpdateFlag = true;
	bool txUpdateFlag = false;
	bool rxUpdateFlag = true;
	double spendTime = 0;
	int N;
	MyVector *myVector;
	Points *points;
	//我们需要定义一个结构体用来存储绕射得到的参数，这地方有个问题是，如果N=360，则这360条ray均可能产生绕射，但是事实情况是绕射产生的可能性比较小，因此这么多空间就浪费了
	Points *diffractionPoints;
public:
	struct Grids *dev_grids;
public:
	virtual vector<vector<double>> initCUDAInput(double RX_X, double RX_Y)=0;
	virtual void updateTx(double x, double y)=0;
	virtual void updateRx(double x, double y)=0;
	virtual void init()=0;
	vector<QString> getCUDAInformation() {
		vector<QString> v;
		v.push_back(QString("CPU"));
		return v;
	}
	bool updateEdges(int index, double x0, double y0, double x1, double y1, double vx, double vy);
	double getSpendTime() { return spendTime; }
	void clearGrids();
	bool isReady() {
		return mapUpdateFlag && txUpdateFlag&&rxUpdateFlag;
	}
	void setN(int N) { this->N = N; }

public:
	RayTracing();
	~RayTracing();
};

