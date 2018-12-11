#pragma once
#include "StaticConstants.h"
#include <vector>
#include <cmath>
#include<fstream>
#include <list> 
#include <QtCore/QDebug>
#include "MyCUDARayTracing.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
using namespace std;

class MyEdge {
public:
	double x1, y1, x2, y2, vx, vy;
};
class MyGrid {
public:
	int N = 0;
	MyEdge *myedge = new MyEdge[MAX_EDGE_GRID];
	void insert(double x1, double y1, double x2, double y2, double vx, double vy) {
		if (N == 60) {
			qDebug() << "exceed 60";
			return;
		}
		myedge[N].x1 = x1;
		myedge[N].y1 = y1;
		myedge[N].x2 = x2;
		myedge[N].y2 = y2;
		myedge[N].vx = vx;
		myedge[N].vy = vy;
		N++;
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
	//vector<list<list<double>>> girdList;
	//vector<vector<vector<double>>> grid_input;
	//MyClass *myClass = new MyClass[ROW*COL];
	//vector<vector<double>> grids[ROW*COL];
	double Tx_x, Tx_y, Rx_x, Rx_y;
	int RayNumbers;
public:
	RayTracingCUDA();
	~RayTracingCUDA();
	//��clear��������֮�佫N��0��N��һ��ָ�룬ָʾ���˶��ٸ���
	void clear() {
		for (int i = 0; i < COL*ROW; i++) {
			mygrid[i].clear();
		}
	}
	QString beginCUDA() {
		struct Grids *grids = (struct Grids *)malloc(sizeof(struct Grids));
		grids->width = LENGTH;
		grids->height = LENGTH;
		for (int i = 0; i < ROW*COL; i++) {
			if (mygrid[i].N != 0) {
				grids->grids[i].N = mygrid[i].N;
				grids->grids[i].isContains = true;
				for (int j = 0; j < mygrid[i].N; j++) {
					grids->grids[i].edges[j].xstart = mygrid[i].myedge[j].x1;
					grids->grids[i].edges[j].ystart = mygrid[i].myedge[j].y1;
					grids->grids[i].edges[j].xend = mygrid[i].myedge[j].x2;
					grids->grids[i].edges[j].yend = mygrid[i].myedge[j].y2;
					grids->grids[i].edges[j].vectorX = mygrid[i].myedge[j].vx;
					grids->grids[i].edges[j].vectorY = mygrid[i].myedge[j].vy;
				}
			}
			else {
				grids->grids[i].N = 0;
				grids->grids[i].isContains = false;
			}
		}
		vector<vector<double>> getResult = initCUDAInput(grids, Tx_x, Tx_y, Rx_x, Rx_y, 360);
		//ofstream out("line.txt");
		QJsonObject json;
		json.insert("type", QString("output"));
		QJsonArray paths;
		QJsonObject pointTemp;
		pointTemp.insert("x", Tx_x);
		pointTemp.insert("y", Tx_y);
		pointTemp.insert("z", 0);

		for (int i = 0; i < getResult.size(); i++) {
			QJsonObject path;
			path.insert("pathloss", -117.8);
			QJsonArray nodeList;
			nodeList.push_back(pointTemp);
			for (int j = 0; j < getResult[i].size(); j += 2) {
				QJsonObject point;
				point.insert("x", getResult[i][j]);
				point.insert("y", getResult[i][j + 1]);
				point.insert("z", 0);
				qDebug() << getResult[i][j] << "," << getResult[i][j + 1];
				nodeList.push_back(point);
			}
			path.insert("nodeList", nodeList);
			paths.push_back(path);
		}
		json.insert("paths", paths);
		QJsonDocument doc(json);
		QString strJson(doc.toJson(QJsonDocument::Compact));
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
	//��һ������
	void normalizationVector(double &x, double &y) {
		double z = sqrt(x*x + y * y);
		x = x / z;
		y = y / z;
	}
	/*
	�ڽ��������궼��˳ʱ�������£�������Ҫ�ж���������������������ڸ����������
	��p2��p1��˳ʱ�뷽�򣬷���ֵΪ-1
	*/
	int multipCross(double x1, double y1, double x2, double y2) {
		double z = x1 * y2 - x2 - y1;
		if (z > 0) {
			//p1��p2��˳ʱ�뷽��
			return 1;
		}
		else if (z == 0) {
			//p1 p2����
			return 0;
		}
		else {
			//p1��p2����ʱ�뷽��
			return -1;
		}
	}
	//ǽ�����ʼ����Ϊ[x1,y1],�յ�����Ϊ[x2,y2]
	//��Ҫ�����ǽ����뵽����ϵͳ
	//���ǵ�gridϵͳ��50*50������ÿ������ı߳�Ϊ2
	//���������Ϊ �����x1,y1��������x2,y2Ϊֹ��ÿ��������ǽ��
	void addEdgeToGrids(double x1, double y1, double x2, double y2) {
		double vectorX, vectorY, normalVectorX, normalVectorY;//������
		normalVectorX = y2 - y1;
		normalVectorY = x1 - x2;
		vectorX = x2 - x1;
		vectorY = y2 - y1;

		normalizationVector(vectorX, vectorY);//��һ��
		normalizationVector(normalVectorX, normalVectorY);
		int row = int(y1 / LENGTH);
		int col = int(x1 / LENGTH);
		int targetRow = int(y2 / LENGTH);
		int targetCol = int(x2 / LENGTH);
		int minRow = row < targetRow ? row : targetRow;
		int minCol = col < targetCol ? col : targetCol;
		targetRow = row < targetRow ? targetRow : row;
		targetCol = col < targetCol ? targetCol : col;
		for (int i = minRow; i <= targetRow; i++) {
			for (int j = minCol; j <= targetCol; j++) {
				int currentIndex = row * COL + col;
				mygrid[currentIndex].insert(x1, y1, x2, y2, normalVectorX, normalVectorY);
			}
		}

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
			//	//����x1,y1
			//	x1 = newX;
			//	y1 = newY;
			//}
		//} while (!(row == targetRow && col == targetCol));
	}

};

