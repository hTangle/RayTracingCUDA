#pragma once
#include "MyCUDARayTracing.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "RayTracing.h"
class CUDARayTracingGPU:public RayTracing{
	//类自带的变量
	//Point *Rx;
	//Points *cuda_points;
	//Points *cuda_diffractionPoints;
	//我们需要定义一个结构体用来存储绕射得到的参数，这地方有个问题是，如果N=360，则这360条ray均可能产生绕射，但是事实情况是绕射产生的可能性比较小，因此这么多空间就浪费了
	//MyVector *cuda_myVector;//tx
	//Point *cuda_Rx;//rx
	//Grids *grids;//每次更新地图数据时，先释放内存，之后在写入数据，然后在写入到GPU中
public:
	vector<vector<double>> initCUDAInput(double RX_X, double RX_Y);
	vector<QString> getCUDAInformation();
	void updateGrids(Grids *grids);
	//virtual bool updateEdges(int index, double x0, double y0, double x1, double y1, double vx, double vy);
	void updateTx(double x, double y);
	void updateRx(double x, double y);
	//double getSpendTime() { return spendTime; }
	//void clearGrids();
	//bool isReady() {
	//	return mapUpdateFlag && txUpdateFlag&&rxUpdateFlag;
	//}
	CUDARayTracingGPU();
	//CUDARayTracingGPU(int N);
	//void setN(int N) { this->N = N; }
	void init();
	~CUDARayTracingGPU();

};