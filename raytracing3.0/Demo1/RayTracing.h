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
	//������Ҫ����һ���ṹ�������洢����õ��Ĳ�������ط��и������ǣ����N=360������360��ray�����ܲ������䣬������ʵ�������������Ŀ����ԱȽ�С�������ô��ռ���˷���
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

