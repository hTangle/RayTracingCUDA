#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "time.h" 
#include "MyCUDARayTracing.h"
#include "StaticConstants.h"

//向量均使用单位向量
/*
我们基于一定的假设，认为网格的总长度和总宽度相等
每个子网格的宽度设定为2
如果我们知道当前坐标为(x,y)
(int(x/2),int(y/2))表示当前所在的网格
则一维数组grid的坐标为 int(x/2)+int(y/2)*ROW
但是我们需要考虑向量信息，即指明当前坐标位置的传输方向
	如果不在格子边界处，则不需要改变
	如果在横纵交点处
		第一象限	不变
		第二象限	(int(x/2)-1)+int(y/2)*ROW
		第三象限	(int(x/2)-1)+(int(y/2)-1)*ROW
		第四象限	(int(x/2))-1+int(y/2)*ROW
	如果在横向边界处
		指向三四象限 int(x/2)+(int(y/2)-1)*ROW
	如果在纵向边界处
		指向二三象限 (int(x/2))-1+int(y/2)*ROW
*/



double directionIntersectCPU(double x1, double y1, double x2, double y2, double x3, double y3) {
	//(x3-x1,y3-y1)X(x2-x1,y2-y1)
	return (y2 - y1)*(x3 - x1) - (x2 - x1)*(y3 - y1);
}
double MyminCPU(double x1, double x2) {
	if (x1 < x2)
		return x1;
	else
		return x2;
}
double MymaxCPU(double x1, double x2) {
	return x1 > x2 ? x1 : x2;
}
double calDistanceCPU(double x1, double y1, double x2, double y2) {
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}
bool onSegmentCPU(double x1, double y1, double x2, double y2, double x3, double y3) {
	if (MyminCPU(x1, x2) <= x3 && x3 < MymaxCPU(x1, x2) && MyminCPU(y1, y2) < y3&&y3 < MymaxCPU(y1, y2)) {
		return true;
	}
	else {
		return false;
	}
}
void calculPointOfIntersectionCPU(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double &x, double &y) {
	double a = y2 - y1;
	double b = x1 - x2;
	double c = y4 - y3;
	double d = x3 - x4;
	double e = (y2 - y1)*x1 - (x2 - x1)*y1;
	double f = (y4 - y3)*x3 - (x4 - x3)*y3;
	x = (e*d - b * f) / (a*d - b * c);
	y = (a*f - e * c) / (a*d - b * c);
}
bool segmentsIntersectCPU(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
	double d1 = directionIntersectCPU(x3, y3, x4, y4, x1, y1);
	double d2 = directionIntersectCPU(x3, y3, x4, y4, x2, y2);
	double d3 = directionIntersectCPU(x1, y1, x2, y2, x3, y3);
	double d4 = directionIntersectCPU(x1, y1, x2, y2, x4, y4);
	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2>0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4>0)))
		return true;
	else if (d1 == 0 && onSegmentCPU(x3, y3, x4, y4, x1, y1)) {
		return true;
	}
	else if (d2 == 0 && onSegmentCPU(x3, y3, x4, y4, x2, y2)) {
		return true;
	}
	else if (d3 == 0 && onSegmentCPU(x1, y1, x2, y2, x3, y3)) {
		return true;
	}
	else if (d4 == 0 && onSegmentCPU(x1, y1, x2, y2, x4, y4)) {
		return true;
	}
	else
		return false;
}

void normalizeVectorCPU(double &x, double &y) {
	double norm = sqrt(x*x + y * y);
	x = x / norm;
	y = y / norm;
}
double calculDistancePointAndLineCPU(double x1, double y1, double x2, double y2, double x0, double y0) {
	double A = y2 - y1;
	double B = x1 - x2;
	double C = x2 * y1 - x1 * y2;
	return abs(A*x0 + B * y0 + C) / sqrt(A*A + B * B);
}
/*
vector is input
points is output
*/
void judgeIsTouchedCPU(MyVector *vector, Grids *grids, Points *points, Point *point, int N) {
	
	for (int i = 0; i < N; i++) {
		int row = vector[i].row;
		int col = vector[i].col;
		int oldRow = row;
		int oldCol = col;
		double x = vector[i].x;
		double y = vector[i].y;
		double vectorX = vector[i].vectorX;
		double vectorY = vector[i].vectorY;
		while (true) {
			int row = vector[i].row;
			int col = vector[i].col;
			int oldRow = row;
			int oldCol = col;
			double x = vector[i].x;
			double y = vector[i].y;
			double vectorX = vector[i].vectorX;
			double vectorY = vector[i].vectorY;
			while (true) {
				oldRow = row;
				oldCol = col;
				if (row < 0 || col < 0 || row >= ROW || col >= COL)
					break;
				int indexC = row * COL + col;
				//首先计算与其他一个边的交点构成一个线段
				int currentX = col * LENGTH;
				int currentY = row * LENGTH;
				//需要判断是否出界
				double newX = x, newY = y;
				if (vectorX > 0 && vectorY > 0) {
					if ((currentX + LENGTH - x)*vectorY > (currentY + LENGTH - y)*vectorX) {
						//top
						if (vectorY != 0) {
							newX = x + (currentX + LENGTH - x)*vectorX / vectorY;
							newY = currentY + LENGTH;
						}
						else {
							newX = currentX + LENGTH;
						}
						row++;
					}
					else if ((currentX + LENGTH - x)*vectorY == (currentY + LENGTH - y)*vectorX) {
						newX = currentX + LENGTH;
						newY = currentY + LENGTH;
						row++;
						col++;
					}
					else {
						//right
						newX = currentX + LENGTH;
						newY = vectorY / vectorX * (currentX + 2 - x) + y;
						col++;
					}
				}
				else if (vectorX > 0 && vectorY < 0) {
					if (-vectorY * (currentX + LENGTH - x) < vectorX*(y - currentY)) {
						//right
						newY = y + (currentX + 2 - x)*vectorY / vectorX;
						newX = currentX + LENGTH;
						col++;
					}
					else if (-vectorY * (currentX + LENGTH - x) == vectorX * (y - currentY)) {
						newX = currentX + LENGTH;
						newY = currentY;
						col++;
						row--;
					}
					else {
						//bottom
						newX = x - vectorX / vectorY * (y - currentY);
						newY = currentY;
						row--;
					}
				}
				else if (vectorX < 0 && vectorY>0) {
					if (-vectorX * (currentY + LENGTH - y) < vectorY*(x - currentX)) {
						//top
						newX = x + (vectorX / vectorY * (currentY + LENGTH - y));
						newY = currentY + LENGTH;
						row++;
					}
					else if (-vectorX * (currentY + LENGTH - y) == vectorY * (x - currentX)) {
						newX = currentX;
						newY = currentY + LENGTH;
						row++;
						col--;
					}
					else {
						//left
						newY = y + (vectorY / vectorX * (currentX - x));
						newX = currentX;
						col--;
					}
				}
				else if (vectorX < 0 && vectorY < 0) {
					if (vectorY*(x - currentX) < vectorX*(y - currentY)) {
						//bottom
						newX = x - vectorX / vectorY * (y - currentY);
						newY = currentY;
						row--;
					}
					else if (vectorY*(x - currentX) == vectorX * (y - currentY)) {
						newX = currentX;
						newY = currentY;
						row--;
						col--;
					}
					else {
						//left
						newY = y - (x - currentX)*vectorY / vectorX;
						newX = currentX;
						col--;
					}
				}
				else if (vectorX == 0) {
					newY = vectorY > 0 ? currentY + LENGTH : currentY;
					row = vectorY > 0 ? row + 1 : row - 1;
				}
				else if (vectorY == 0) {
					newX = vectorX > 0 ? currentX + LENGTH : currentY;
					col = vectorX > 0 ? col + 1 : col - 1;
				}
				//vectorX vectorY不可能同时为0
				//如果格子中包含边或者接收点位于该格子内时，才需要遍历
				if (grids->grids[indexC].isContains || (oldRow == point->row&&oldCol == point->col)) {
					//contains edge
					double distanceM = std::sqrt(2.0)*LENGTH;
					bool isFind = false;
					double insertPointOutX = 0, insertPointOutY = 0;
					int edgeIndex = 0;

					for (int k = 0; k < grids->grids[indexC].N; k++) {
						if (segmentsIntersectCPU(x, y, newX, newY, grids->grids[indexC].edges[k].xstart, grids->grids[indexC].edges[k].ystart, grids->grids[indexC].edges[k].xend, grids->grids[indexC].edges[k].yend) == true) {
							double insertPointX = 0, insertPointY = 0;//当前交点
							calculPointOfIntersectionCPU(x, y, newX, newY, grids->grids[indexC].edges[k].xstart, grids->grids[indexC].edges[k].ystart, grids->grids[indexC].edges[k].xend, grids->grids[indexC].edges[k].yend, insertPointX, insertPointY);
							if (currentX > insertPointX || insertPointX > currentX + LENGTH || currentY > insertPointY || insertPointY > currentY + LENGTH) {
								//交点必须在这个格子内
								continue;
							}
							double tempDistance = calDistanceCPU(x, y, insertPointX, insertPointY);
							//这么设置的目的是存在反射后，新的起点仍然在这个格子内，而且在一个墙面上，着必然与这个墙面相交的，因此需要过滤掉这个交点
							//防止交点是自己
							if (abs(insertPointX - x) < 0.0001 && abs(insertPointY - y) < 0.0001)
								continue;
							if (tempDistance < distanceM) {
								isFind = true;
								distanceM = tempDistance;
								insertPointOutX = insertPointX;
								insertPointOutY = insertPointY;
								edgeIndex = k;
							}
						}
					}
					bool isReached = false;
					if (oldRow == point->row&&oldCol == point->col) {
						//当目标点在这个格子中时，需要判断是否与目标点相交
						if (x != newX && y != newY) {
							double targetDistance = calculDistancePointAndLineCPU(x, y, newX, newY, point->x, point->y);
							//只有在捕获圆半径内且距离出发点最近，才有交点
							if (targetDistance < CAPTURE_RADIUS) {
								double targetPointDistance = calDistanceCPU(x, y, point->x, point->y);
								if (targetPointDistance < distanceM) {
									distanceM = targetPointDistance;
									insertPointOutX = point->x;
									insertPointOutY = point->y;
									isReached = true;
									isFind = true;
								}
							}
						}
					}

					if (isFind&&isReached) {
						//与目标点相交，存在射线
						points[i].point[points[i].N].x = insertPointOutX;
						points[i].point[points[i].N].y = insertPointOutY;
						points[i].N++;
						points[i].isFind = true;
						break;
					}
					//已经遍历完成了，如果有交点，则需要更改射线传输的方向
					else if (isFind) {
						//当前射线的向量
						//if (debugFlag) {
						//	double currentVectorX = x - insertPointOutX;
						//	double currentVectorY = y - insertPointOutY;
						//	normalizeVector(currentVectorX, currentVectorY);
						//	vectorX = currentVectorX;
						//	vectorY = currentVectorY;
						//	points[i].point[points[i].N].x = insertPointOutX;
						//	points[i].point[points[i].N].y = insertPointOutY;
						//	points[i].N++;
						//	x = insertPointOutX;
						//	y = insertPointOutY;
						//	row = oldRow;
						//	col = oldCol;
						//	if (points[i].N > MAX_REFLECTION_TIMES) {
						//		//超出了最大次数,没找到交点，因此点数为0
						//		//points[i].N = 0;
						//		break;
						//	}
						//}
						//else {
						double currentVectorX = x - insertPointOutX;
						double currentVectorY = y - insertPointOutY;
						//要求反射向量
						//具体做法是对称
						// \  |  /
						//  \ | /
						//   \|/
						//------------------
						//先求墙面的法向量
						//grids->grids[indexC].edges[k]
						//之前求的墙面的法向量是顺时针方向的，也就是朝内，但是我们默认射线只从外部射过来
						//因此法向量均取反
						double edgeVectorX = -grids->grids[indexC].edges[edgeIndex].vectorX;
						double edgeVectorY = -grids->grids[indexC].edges[edgeIndex].vectorY;

						if (edgeVectorX*currentVectorX + edgeVectorY * currentVectorY < 0) {
							//夹角如果大于90度则需要转换
							edgeVectorX = -edgeVectorX;
							edgeVectorY = -edgeVectorY;
						}
						//注意，墙面的法向量为单位向量
						//反射向量在法向量上的投影为(uv)v 
						//反射向量为2(uv)v-u
						//参考https://www.cnblogs.com/graphics/archive/2013/02/21/2920627.html

						double temp = currentVectorX * edgeVectorX + currentVectorY * edgeVectorY;//uv
						double edgeProjectVectorX = temp * edgeVectorX;
						double edgeProjectVectorY = temp * edgeVectorY;
						//遍历完毕，更新数据，将交点写入输出Points
						vectorX = 2 * edgeProjectVectorX - currentVectorX;
						vectorY = 2 * edgeProjectVectorY - currentVectorY;
						normalizeVectorCPU(vectorX, vectorY);//完事以后需要归一化
						x = insertPointOutX;//更新反射起点
						y = insertPointOutY;
						points[i].point[points[i].N].x = x;
						points[i].point[points[i].N].y = y;
						points[i].N++;
						row = oldRow;
						col = oldCol;
						if (points[i].N > MAX_REFLECTION_TIMES) {
							//超出了最大次数,没找到交点，因此点数为0
							points[i].N = 0;
							points[i].isFind = false;
							break;
						}
						//}

					}
				}
				else {
					//更新数据
					x = newX;
					y = newY;
				}
				//printf("current position:%f,%f", x, y);
				//上面的newX，newY是接下来点的坐标，这时需要判断该条线段与格子中的边是否相交
				//break;
				//if (grids[indexC].grids->isContains) {
				//	
				//	for (int k = 0; k < grids[indexC].grids->N; i++) {
				//		//judge weather is intersecting
				//	}

				//}
				//else {


				//	//需要找到接下来的格子
				//	//update vectors
				//	//remain direction update x and y
				//}
			}
			//vector[i].
		}
	}
}
/*
grid_input:存储着网格信息，每个网格中存在边，这些边也会构成向量
N:表示射线条数，等间隔划分，如果N=360，则每个角度为1度
*/
vector<vector<double>> initCUDAInputCPU(Grids *grids, double TX_X, double TX_Y, double RX_X, double RX_Y, int N) {
	//需要初始化的有
	//grids->存储网格
	//我们设置网格为50*50的小格子组成，如果传入的数据小于这些，则将其他全部置为空
	//int N = 180;
	//printf("Start...%d\n", sizeof(bool));
	clock_t start, finish;
	start = clock();
	//struct Grids *cuda_grids;

	MyVector *myVector = (struct MyVector *)malloc(sizeof(struct MyVector) * N);
	Points *points = (struct Points *)malloc(sizeof(struct Points) * N);
	Point *Rx = (struct Point *)malloc(sizeof(struct Point));
	Rx->x = RX_X;
	Rx->y = RX_Y;
	Rx->row = int(RX_Y / LENGTH);
	Rx->col = int(RX_X / LENGTH);

	for (int i = 0; i < N; i++) {
		//myVector[i]= (struct MyVector *)malloc(sizeof(struct MyVector));
		myVector[i].x = TX_X;
		myVector[i].y = TX_Y;
		double angle = 2 * PIs / N * i;
		myVector[i].vectorX = cos(angle);
		myVector[i].vectorY = sin(angle);
		myVector[i].col = int(TX_X/2);
		myVector[i].row = int(TX_Y/2);
		points[i].N = 0;
		points[i].isFind = false;
	}



	judgeIsTouchedCPU(myVector, grids, points, Rx, N);
	vector<vector<double>> getResult;
	int pointsCount = 0;
	for (int i = 0; i < N; i++) {
		if (points[i].isFind) {
			vector<double> results;
			for (int j = 0; j < points[i].N; j++) {
				results.push_back(points[i].point[j].x);
				results.push_back(points[i].point[j].y);
				//printf("%d-%d-%d:%f,%f\n", i, j, 2 * i, points[i].point[j].x, points[i].point[j].y);
			}
			getResult.push_back(results);
		}
		else {
			pointsCount++;
		}
	}
	finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("%f seconds\n", duration);
	//需要返回point
	free(myVector);
	return getResult;
}