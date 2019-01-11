#pragma once
#include "StaticConstants.h"
#include <vector>
#include <cmath>
#include<fstream>
#include <list> 
#include <QtCore/QDebug>
#include "MyCUDARayTracing.h"
#include "CUDARayTracingGPU.h"
#include "MyRayTracingCPU.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>

using namespace std;
class MyEdge {
public:
	double x1, y1, x2, y2, vx, vy;
	MyEdge() {
		x1 = 0, y1 = 0, x2 = 0, y2 = 0, vx = 0, vy = 0;
	}
};
class MyGrid {
public:
	int N = 0;
	MyEdge *myedge = new MyEdge[MAX_EDGE_GRID];
	bool insert(double x1, double y1, double x2, double y2, double vx, double vy) {
		if (N >= MAX_EDGE_GRID - 1) {
			qDebug() << "exceed" << MAX_EDGE_GRID;
			return false;
		}
		else {
			myedge[N].x1 = x1;
			myedge[N].y1 = y1;
			myedge[N].x2 = x2;
			myedge[N].y2 = y2;
			myedge[N].vx = vx;
			myedge[N].vy = vy;
			N++;
			return true;
		}

	}
	void clear() {
		N = 0;
	}
};
//struct MyGridsHere
//{
//	vector<vector<double>> grids;
//};
class RayTracingCUDA
{
public:
	MyGrid *mygrid = new MyGrid[COL*ROW];
	//RayTracingCUDA 
	//CUDARayTracingGPU raytracingGPU;
	RayTracingCPU raytracingGPU;
	//vector<list<list<double>>> girdList;
	//vector<vector<vector<double>>> grid_input;
	//MyClass *myClass = new MyClass[ROW*COL];
	//vector<vector<double>> grids[ROW*COL];
	double Tx_x = -100, Tx_y = -100, Rx_x = -100, Rx_y = -100;
	int RayNumbers;
	vector<QString> getCUDADetailInformation() {
		return raytracingGPU.getCUDAInformation();
	}
public:
	RayTracingCUDA() {
		//180 0.013 360 0.049 540 0.104 720 0.198
		raytracingGPU.setN(900);
		raytracingGPU.init();
	}
	~RayTracingCUDA() {

	}
	//我clear的做法是之间将N置0，N是一个指针，指示存了多少个边
	void updateTx(double x, double y) {
		raytracingGPU.updateTx(x, y);
	}
	void updateRx(double x, double y) {
		raytracingGPU.updateRx(x, y);
	}
	void clear() {
		raytracingGPU.clearGrids();
	}
	QString runInputDataCUDA(struct Grids *grids) {
		qDebug() << "begin CUDA";
		QJsonObject json;
		json.insert("type", QString("output"));
		//if (raytracingGPU->isReady()) {
		//	vector<vector<double>> getResult = raytracingGPU->initCUDAInput();
		//	QJsonArray paths;
		//	QJsonObject pointTemp;
		//	pointTemp.insert("x", Tx_x - 50);
		//	pointTemp.insert("y", (Tx_y - 50));
		//	pointTemp.insert("z", 0);
		//	qDebug() << "getResult.size()" << getResult.size();
		//	for (int i = 0; i < getResult.size(); i++) {
		//		QJsonObject path;
		//		path.insert("pathloss", -117.8);
		//		QJsonArray nodeList;
		//		nodeList.push_back(pointTemp);
		//		for (int j = 0; j < getResult[i].size(); j += 2) {
		//			QJsonObject point;
		//			point.insert("x", (getResult[i][j] - 50));
		//			point.insert("y", (getResult[i][j + 1] - 50));
		//			point.insert("z", 0);
		//			//qDebug() << (getResult[i][j] - 50)<< "," << (getResult[i][j + 1] - 50);
		//			nodeList.push_back(point);
		//		}
		//		path.insert("nodeList", nodeList);
		//		paths.push_back(path);
		//	}
		//	json.insert("paths", paths);
		//	json.insert("state", QString("1"));
		//	qDebug() << "calculations finished";
		//}
		//else {
		//	qDebug() << "calculations do not begin";
		//	
		//}
		json.insert("state", QString("0"));
		QJsonDocument doc(json);
		QString strJson(doc.toJson(QJsonDocument::Compact));
		qDebug() << "Send Message";
		return strJson;
	}
	void updateMapdata() {
		//struct Grids *grids = (struct Grids *)malloc(sizeof(struct Grids));
		//grids->width = LENGTH;
		//grids->height = LENGTH;
		//int edgeCount = 0;
		//for (int i = 0; i < ROW*COL; i++) {
		//	if (mygrid[i].N != 0) {
		//		grids->grids[i].N = mygrid[i].N;
		//		edgeCount += mygrid[i].N;
		//		grids->grids[i].isContains = true;
		//		for (int j = 0; j < mygrid[i].N; j++) {
		//			grids->grids[i].edges[j].xstart = mygrid[i].myedge[j].x1;
		//			grids->grids[i].edges[j].ystart = mygrid[i].myedge[j].y1;
		//			grids->grids[i].edges[j].xend = mygrid[i].myedge[j].x2;
		//			grids->grids[i].edges[j].yend = mygrid[i].myedge[j].y2;
		//			grids->grids[i].edges[j].vectorX = mygrid[i].myedge[j].vx;
		//			grids->grids[i].edges[j].vectorY = mygrid[i].myedge[j].vy;
		//		}
		//	}
		//	else {
		//		grids->grids[i].N = 0;
		//		grids->grids[i].isContains = false;
		//	}
		//}
		//qDebug() << "data inited succeed, there has " << edgeCount << "edges";
		//raytracingGPU.updateGrids(grids);
		//free(grids);
	}
	QString beginCUDA() {
		//struct Grids *grids = (struct Grids *)malloc(sizeof(struct Grids));
		//grids->width = LENGTH;
		//grids->height = LENGTH;
		//int edgeCount = 0;
		//for (int i = 0; i < ROW*COL; i++) {
		//	if (mygrid[i].N != 0) {
		//		grids->grids[i].N = mygrid[i].N;
		//		edgeCount += mygrid[i].N;
		//		grids->grids[i].isContains = true;
		//		for (int j = 0; j < mygrid[i].N; j++) {
		//			grids->grids[i].edges[j].xstart = mygrid[i].myedge[j].x1;
		//			grids->grids[i].edges[j].ystart = mygrid[i].myedge[j].y1;
		//			grids->grids[i].edges[j].xend = mygrid[i].myedge[j].x2;
		//			grids->grids[i].edges[j].yend = mygrid[i].myedge[j].y2;
		//			grids->grids[i].edges[j].vectorX = mygrid[i].myedge[j].vx;
		//			grids->grids[i].edges[j].vectorY = mygrid[i].myedge[j].vy;
		//		}
		//	}
		//	else {
		//		grids->grids[i].N = 0;
		//		grids->grids[i].isContains = false;
		//	}
		//}
		//qDebug() << "data inited succeed, there has " << edgeCount << "edges";
		//raytracingGPU.updateGrids(grids);
		qDebug() << "begin CUDA";
		QJsonObject json;
		json.insert("type", QString("output"));
		if (raytracingGPU.isReady()) {
			vector<vector<double>> getResult = raytracingGPU.initCUDAInput(Rx_x, Rx_y);
			//vector<vector<double>> getResult = initCUDAInputS(grids,Tx_x,Tx_y,Rx_x,Rx_y,180);
			qDebug() << "calculation finished";
			QJsonArray paths;
			QJsonObject pointTemp;
			pointTemp.insert("x", Tx_x - 50);
			pointTemp.insert("y", (Tx_y - 50));
			pointTemp.insert("z", 0);
			qDebug() << "getResult.size()" << getResult.size();
			for (int i = 0; i < getResult.size(); i++) {
				QJsonObject path;
				path.insert("pathloss", -117.8);
				QJsonArray nodeList;
				nodeList.push_back(pointTemp);
				for (int j = 0; j < getResult[i].size(); j += 2) {
					QJsonObject point;
					point.insert("x", (getResult[i][j] - 50));
					point.insert("y", (getResult[i][j + 1] - 50));
					point.insert("z", 0);
					//qDebug() << (getResult[i][j] - 50)<< "," << (getResult[i][j + 1] - 50);
					nodeList.push_back(point);
				}
				path.insert("nodeList", nodeList);
				paths.push_back(path);
			}
			json.insert("paths", paths);
			json.insert("state", QString("1"));
			json.insert("time", QString("%1: runtime is %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(raytracingGPU.getSpendTime()));
		}
		else {
			json.insert("state", QString("0"));
		}
		

		//json.insert("time", raytracingGPU.getSpendTime());
		//if (raytracingGPU.isReady()) {
		//	vector<vector<double>> getResult = raytracingGPU.initCUDAInput(grids);
		//	qDebug() << "calculation finished";			
		//	QJsonArray paths;
		//	QJsonObject pointTemp;
		//	pointTemp.insert("x", Tx_x - 50);
		//	pointTemp.insert("y", (Tx_y - 50));
		//	pointTemp.insert("z", 0);
		//	qDebug() << "getResult.size()" << getResult.size();
		//	for (int i = 0; i < getResult.size(); i++) {
		//		QJsonObject path;
		//		path.insert("pathloss", -117.8);
		//		QJsonArray nodeList;
		//		nodeList.push_back(pointTemp);
		//		for (int j = 0; j < getResult[i].size(); j += 2) {
		//			QJsonObject point;
		//			point.insert("x", (getResult[i][j] - 50));
		//			point.insert("y", (getResult[i][j + 1] - 50));
		//			point.insert("z", 0);
		//			//qDebug() << (getResult[i][j] - 50)<< "," << (getResult[i][j + 1] - 50);
		//			nodeList.push_back(point);
		//		}
		//		path.insert("nodeList", nodeList);
		//		paths.push_back(path);
		//	}
		//	json.insert("paths", paths);
		//	json.insert("state", QString("1"));
		//	json.insert("time", raytracingGPU.getSpendTime());
		//}
		//else {
		//	qDebug() << "calculations do not begin";
		//	json.insert("state", QString("0"));
		//}	
		QJsonDocument doc(json);
		QString strJson(doc.toJson(QJsonDocument::Compact));
		qDebug() << "Send Message";
		return strJson;

		//QJsonObject json;
		//json.insert("type", QString("output"));
		//QJsonArray paths;

		//for (int i = 0; i < getResult.size(); i++) {
		//	QJsonObject path;
		//	path.insert("pathloss", -117.8);
		//	QJsonArray nodeList;
		//	for (int j = 0; j < getResult[i].size(); j += 2) {
		//		QJsonObject point;
		//		point.insert("x", getResult[i][j]);
		//		point.insert("y", getResult[i][j + 1]);
		//		point.insert("z", 0);
		//		nodeList.push_back(point);
		//		//out << i << ":[" << getResult[i][j] << "," << getResult[i][j + 1] << "],\n";
		//	}
		//	//path.insert("nodeList", nodeList);
		//	//paths.insert(i, path);
		//}
		////json.insert("paths", paths);
		////out.close();
		//free(grids);
		////QJsonDocument doc(json);
		////QString strJson(doc.toJson(QJsonDocument::Compact));
		//return getResult;
	}
	void printVector() {
		ofstream out("out.txt");
		for (int i = 0; i < ROW*COL; i++) {
			//out << i << ":\n";
			for (int j = 0; j < mygrid[i].N; j++) {
				//out << "    edge" << j<<":\n";
				out << "[";
				out << mygrid[i].myedge[j].x1 << ",";
				out << mygrid[i].myedge[j].y1 << ",";
				out << mygrid[i].myedge[j].x2 << ",";
				out << mygrid[i].myedge[j].y2 << "],";
				//out << mygrid[i].myedge[j].vx << " ";
				//out << mygrid[i].myedge[j].vy;
				out << "\n";
			}
			//out <<"\n";
		}
		out.close();
	}
	QString getMapDataJSON() {
		QJsonObject json;
		json.insert("type", QString("mapDataGet"));
		QJsonArray paths;
		for (int i = 0; i < ROW*COL; i++) {
			if (mygrid[i].N > 1) {
				for (int j = 0; j < mygrid[i].N; j++) {
					QJsonArray nodeList;
					nodeList.push_back(mygrid[i].myedge[j].x1 - 50);
					nodeList.push_back(mygrid[i].myedge[j].y1 - 50);
					nodeList.push_back(mygrid[i].myedge[j].x2 - 50);
					nodeList.push_back(mygrid[i].myedge[j].y2 - 50);
					paths.push_back(nodeList);
				}
			}
		}
		json.insert("data", paths);
		QJsonDocument doc(json);
		QString strJson(doc.toJson(QJsonDocument::Compact));
		qDebug() << "Send Message";
		return strJson;
	}
	//归一化向量
	void normalizationVector(double &x, double &y) {
		double z = sqrt(x * x + y * y);
		x = x / z;
		y = y / z;
	}
	/*
	在建筑物坐标都是顺时针的情况下，我们需要判断内向法向量，这个法向量在该向量的左侧
	即p2在p1的顺时针方向，返回值为-1
	*/
	int multipCross(double x1, double y1, double x2, double y2) {
		double z = x1 * y2 - x2 - y1;
		if (z > 0) {
			//p1在p2的顺时针方向
			return 1;
		}
		else if (z == 0) {
			//p1 p2共线
			return 0;
		}
		else {
			//p1在p2的逆时针方向
			return -1;
		}
	}
	//墙面的起始坐标为[x1,y1],终点坐标为[x2,y2]
	//需要将这个墙面加入到坐标系统
	//我们的grid系统是50*50的网格，每个网格的边长为2
	//将问题抽象为 从起点x1,y1出发，到x2,y2为止，每次碰到的墙面
	double direction(double v1x, double v1y, double v2x, double v2y) {
		//返回为正，则
		return (v1x*v2y - v2x * v1y);
	}
	bool addEdgeToGrids(double x1, double y1, double x2, double y2) {
		double normalVectorX, normalVectorY;//法向量
		//这地方直接求法向量，而且法向量位于直线的顺时针方向
		normalVectorX = y1 - y2;//-y
		normalVectorY = x2 - x1;//x
		//vectorX = x2 - x1;
		//vectorY = y2 - y1;
		//normalizationVector(vectorX, vectorY);//归一化
		normalizationVector(normalVectorX, normalVectorY);
		int row = int(y1 / LENGTH);
		int col = int(x1 / LENGTH);
		int targetRow = int(y2 / LENGTH);
		int targetCol = int(x2 / LENGTH);
		int minRow = row < targetRow ? row : targetRow;
		int minCol = col < targetCol ? col : targetCol;
		targetRow = row < targetRow ? targetRow : row;
		targetCol = col < targetCol ? targetCol : col;
		minRow = minRow >= ROW ? ROW - 1 : minRow;
		minCol = minCol >= COL ? COL - 1 : minCol;
		targetRow = targetRow >= ROW ? ROW - 1 : targetRow;
		targetCol = targetCol >= COL ? COL - 1 : targetCol;
		//qDebug() << "[" << x1 << "," << y1 << "],[" << x2 << "," << y2 << "]";
		//qDebug() << "[" << minRow << "," << targetRow << "],[" << minCol << "," << targetCol << "]";
		//[66.2588, 98.8762], [66.6485, 100]
		//if()
		bool flag = true;
		for (int i = minRow; i <= targetRow; i++) {
			for (int j = minCol; j <= targetCol; j++) {
				int currentIndex = i * COL + j;
				flag = raytracingGPU.updateEdges(currentIndex, x1, y1, x2, y2, normalVectorX, normalVectorY);
				//flag = mygrid[currentIndex].insert(x1, y1, x2, y2, normalVectorX, normalVectorY);
			}
		}
		return flag;
		//[53.95, 68.1127], [56.3247, 68.1282]
		//	[34, 34], [26, 28]
		//	56.3247, 68.1282
		//	[56.3247, 68.1282], [56.2781, 65.1017]
		//	[32, 34], [28, 28]
		//	100, 50.5743
		//	[100, 50.5743], [98.9601, 45.5456]
		//	[22, 25], [49, 49]
		//	98.9601, 45.5456
		//	[98.9601, 45.5456], [97.1442, 45.8404]
		//	[22, 22], [48, 49]
		//	97.1442, 45.8404
		//	[97.1442, 45.8404], [98.1686, 50.8847]
		//	[22, 25], [48, 49]
		//	98.1686, 50.8847
		//	[98.1686, 50.8847], [100, 50.5743]
		//	[25, 25], [49, 49]
		//do {
			//if (row < 0 || row >= ROW || col < 0 || COL <= col)
			//	return;
			//int currentIndex= row * COL + col;
			//if (row == targetRow && col == targetCol) {
			//	mygrid[currentIndex].insert(x1, y1, x2, y2, normalVectorX, normalVectorY);
			//}
			//else {
			//	int currentX = col * LENGTH;
			//	int currentY = row * LENGTH;
			//	double newX = x1, newY = y1;
			//	if (vectorX > 0 && vectorY > 0) {
			//		if ((currentX + LENGTH - x1)*vectorY > (currentY + LENGTH - y1)*vectorX) {
			//			//top
			//			if (vectorY != 0) {
			//				newX = x1 + (currentX + LENGTH - x1)*vectorX / vectorY;
			//				newY = currentY + LENGTH;
			//			}
			//			else {
			//				newX = currentX + LENGTH;
			//			}
			//			row++;
			//		}
			//		else if ((currentX + LENGTH - x1)*vectorY == (currentY + LENGTH - y1)*vectorX) {
			//			newX = currentX + LENGTH;
			//			newY = currentY + LENGTH;
			//			row++;
			//			col++;
			//		}
			//		else {
			//			//right
			//			newX = currentX + LENGTH;
			//			newY = vectorY / vectorX * (currentX + 2 - x1) + y1;
			//			col++;
			//		}
			//	}
			//	else if (vectorX > 0 && vectorY < 0) {
			//		if (-vectorY * (currentX + LENGTH - x1) < vectorX*(y1 - currentY)) {
			//			//right
			//			newY = y1 + (currentX + 2 - x1)*vectorY / vectorX;
			//			newX = currentX + LENGTH;
			//			col++;
			//		}
			//		else if (-vectorY * (currentX + LENGTH - x1) == vectorX * (y1 - currentY)) {
			//			newX = currentX + LENGTH;
			//			newY = currentY;
			//			col++;
			//			row--;
			//		}
			//		else {
			//			//bottom
			//			newX = x1 - vectorX / vectorY * (y1 - currentY);
			//			newY = currentY;
			//			row--;
			//		}
			//	}
			//	else if (vectorX < 0 && vectorY>0) {
			//		if (-vectorX * (currentY + LENGTH - y1) < vectorY*(x1 - currentX)) {
			//			//top
			//			newX = x1 + (vectorX / vectorY * (currentY + LENGTH - y1));
			//			newY = currentY + LENGTH;
			//			row++;
			//		}
			//		else if (-vectorX * (currentY + LENGTH - y1) == vectorY * (x1 - currentX)) {
			//			newX = currentX;
			//			newY = currentY + LENGTH;
			//			row++;
			//			col--;
			//		}
			//		else {
			//			//left
			//			newY = y1 + (vectorY / vectorX * (currentX - x1));
			//			newX = currentX;
			//			col--;
			//		}
			//	}
			//	else if (vectorX < 0 && vectorY < 0) {
			//		if (vectorY*(x1 - currentX) < vectorX*(y1 - currentY)) {
			//			//bottom
			//			newX = x1 - vectorX / vectorY * (y1 - currentY);
			//			newY = currentY;
			//			row--;
			//		}
			//		else if (vectorY*(x1 - currentX) == vectorX * (y1 - currentY)) {
			//			newX = currentX;
			//			newY = currentY;
			//			row--;
			//			col--;
			//		}
			//		else {
			//			//left
			//			newY = y1 - (x1 - currentX)*vectorY / vectorX;
			//			newX = currentX;
			//			col--;
			//		}
			//	}
			//	else if (vectorX == 0) {
			//		newY = vectorY > 0 ? currentY + LENGTH : currentY;
			//		row = vectorY > 0 ? row + 1 : row - 1;
			//	}
			//	else if (vectorY == 0) {
			//		newX = vectorX > 0 ? currentX + LENGTH : currentY;
			//		col = vectorX > 0 ? col + 1 : col - 1;
			//	}
			//	mygrid[currentIndex].insert(x1, y1, newX, newY, normalVectorX, normalVectorY);
			//	//vector<double> edge;
			//	//edge.push_back(x1);
			//	//edge.push_back(y1);
			//	//edge.push_back(newX);
			//	//edge.push_back(newY);
			//	//edge.push_back(vectorX);
			//	//edge.push_back(vectorY);
			//	//myClass[currentIndex].grids.push_back(edge);
			//	//grid_input.at(currentIndex).push_back(edge);
			//	//更新x1,y1
			//	x1 = newX;
			//	y1 = newY;
			//}
		//} while (!(row == targetRow && col == targetCol));
	}

};

