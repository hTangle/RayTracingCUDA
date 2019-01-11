#pragma once
#include "MyCUDARayTracing.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "RayTracing.h"
class CUDARayTracingGPU:public RayTracing{
	//���Դ��ı���
	//Point *Rx;
	//Points *cuda_points;
	//Points *cuda_diffractionPoints;
	//������Ҫ����һ���ṹ�������洢����õ��Ĳ�������ط��и������ǣ����N=360������360��ray�����ܲ������䣬������ʵ�������������Ŀ����ԱȽ�С�������ô��ռ���˷���
	//MyVector *cuda_myVector;//tx
	//Point *cuda_Rx;//rx
	//Grids *grids;//ÿ�θ��µ�ͼ����ʱ�����ͷ��ڴ棬֮����д�����ݣ�Ȼ����д�뵽GPU��
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