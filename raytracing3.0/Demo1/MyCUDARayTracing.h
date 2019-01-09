#pragma once

#include <StaticConstants.h>
#include <vector>
#include <QString>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
using namespace std;

struct Point
{
	double x, y;
	int col, row;
};
struct Points
{
	int N;
	bool isFind;
	struct Point point[30];
};
struct MyVector
{
	double x, y;
	double vectorX, vectorY;
	int row, col;
};


struct Edge {
	double xstart, xend;
	double ystart, yend;
	double vectorX, vectorY;
};

struct Grid
{
	int N;
	bool isContains=false;
	struct Edge edges[MAX_EDGE_GRID];
};

struct Grids
{
	int height;
	int width;
	struct Grid grids[ROW*COL];
};

class CUDARayTracingGPU {
	//���Դ��ı���
private:
	bool mapUpdateFlag = false;
	bool txUpdateFlag = false;
	bool rxUpdateFlag = false;
	double spendTime=0;
	int N;
	MyVector *myVector;
	//Point *Rx;
	//Points *cuda_points;
	//Points *cuda_diffractionPoints;
	Points *points;
	//������Ҫ����һ���ṹ�������洢����õ��Ĳ�������ط��и������ǣ����N=360������360��ray�����ܲ������䣬������ʵ�������������Ŀ����ԱȽ�С�������ô��ռ���˷���
	Points *diffractionPoints;
public:
	struct Grids *dev_grids;
	//MyVector *cuda_myVector;//tx
	//Point *cuda_Rx;//rx
	//Grids *grids;//ÿ�θ��µ�ͼ����ʱ�����ͷ��ڴ棬֮����д�����ݣ�Ȼ����д�뵽GPU��
public:
	vector<vector<double>> initCUDAInput(double RX_X, double RX_Y);
	vector<QString> getCUDAInformation();
	void updateGrids(Grids *grids);
	bool updateEdges(int index,double x0,double y0,double x1,double y1,double vx,double vy);
	void updateTx(double x, double y);
	void updateRx(double x, double y);
	double getSpendTime() { return spendTime; }
	void clearGrids();
	bool isReady() { 
		return mapUpdateFlag && txUpdateFlag&&rxUpdateFlag; 
	}
	CUDARayTracingGPU();
	CUDARayTracingGPU(int N);
	void setN(int N) { this->N = N; }
	void init();
	~CUDARayTracingGPU();

};
vector<vector<double>> initCUDAInputS(Grids *grids, double TX_X, double TX_Y, double RX_X, double RX_Y, int N);
//vector<vector<double>> initCUDAInputCPU(Grids *grids, double TX_X, double TX_Y, double RX_X, double RX_Y, int N);
//vector<QString> getCUDAInformation();

//vector<vector<double>> initCUDAInput(vector<vector<vector<double>>> grid_input, double TX_X, double TX_Y, double RX_X, double RX_Y, int N);
