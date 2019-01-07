#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "time.h" 
#include "MyCUDARayTracing.h"
#include "StaticConstants.h"
#include <QtCore/QDebug>



//������ʹ�õ�λ����
/*
���ǻ���һ���ļ��裬��Ϊ������ܳ��Ⱥ��ܿ�����
ÿ��������Ŀ���趨Ϊ2
�������֪����ǰ����Ϊ(x,y)
(int(x/2),int(y/2))��ʾ��ǰ���ڵ�����
��һά����grid������Ϊ int(x/2)+int(y/2)*ROW
����������Ҫ����������Ϣ����ָ����ǰ����λ�õĴ��䷽��
	������ڸ��ӱ߽紦������Ҫ�ı�
	����ں��ݽ��㴦
		��һ����	����
		�ڶ�����	(int(x/2)-1)+int(y/2)*ROW
		��������	(int(x/2)-1)+(int(y/2)-1)*ROW
		��������	(int(x/2))-1+int(y/2)*ROW
	����ں���߽紦
		ָ���������� int(x/2)+(int(y/2)-1)*ROW
	���������߽紦
		ָ��������� (int(x/2))-1+int(y/2)*ROW
*/



__device__ double directionIntersect(double x1, double y1, double x2, double y2, double x3, double y3) {
	//(x3-x1,y3-y1)X(x2-x1,y2-y1)
	return (y2 - y1)*(x3 - x1) - (x2 - x1)*(y3 - y1);
}
__device__ double Mymin(double x1, double x2) {
	if (x1 < x2)
		return x1;
	else
		return x2;
}
__device__ double Mymax(double x1, double x2) {
	return x1 > x2 ? x1 : x2;
}
__device__ double calDistance(double x1, double y1, double x2, double y2) {
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}
__device__ bool onSegment(double x1, double y1, double x2, double y2, double x3, double y3) {
	if (Mymin(x1, x2) <= x3 && x3 < Mymax(x1, x2) && Mymin(y1, y2) < y3&&y3 < Mymax(y1, y2)) {
		return true;
	}
	else {
		return false;
	}
}
__device__ void calculPointOfIntersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double &x, double &y) {
	double a = y2 - y1;
	double b = x1 - x2;
	double c = y4 - y3;
	double d = x3 - x4;
	double e = (y2 - y1)*x1 - (x2 - x1)*y1;
	double f = (y4 - y3)*x3 - (x4 - x3)*y3;
	x = (e*d - b * f) / (a*d - b * c);
	y = (a*f - e * c) / (a*d - b * c);
}
__device__ bool segmentsIntersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
	double d1 = directionIntersect(x3, y3, x4, y4, x1, y1);
	double d2 = directionIntersect(x3, y3, x4, y4, x2, y2);
	double d3 = directionIntersect(x1, y1, x2, y2, x3, y3);
	double d4 = directionIntersect(x1, y1, x2, y2, x4, y4);
	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2>0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4>0)))
		return true;
	else if (d1 == 0 && onSegment(x3, y3, x4, y4, x1, y1)) {
		return true;
	}
	else if (d2 == 0 && onSegment(x3, y3, x4, y4, x2, y2)) {
		return true;
	}
	else if (d3 == 0 && onSegment(x1, y1, x2, y2, x3, y3)) {
		return true;
	}
	else if (d4 == 0 && onSegment(x1, y1, x2, y2, x4, y4)) {
		return true;
	}
	else
		return false;
}

__device__ void normalizeVector(double &x, double &y) {
	double norm = sqrt(x*x + y * y);
	x = x / norm;
	y = y / norm;
}
//[x1,y1],[x2,y2]���ɵ�ֱ�ߵ�[x0,y0]�ľ���
__device__ double calculDistancePointAndLine(double x1, double y1, double x2, double y2, double x0, double y0) {
	double A = y2 - y1;
	double B = x1 - x2;
	double C = x2 * y1 - x1 * y2;
	return abs(A*x0 + B * y0 + C) / sqrt(A*A + B * B);
}

__device__ bool isEndPoint(Edge edge, double x, double y) {
	if (calDistance(edge.xend, edge.yend, x, y) < IS_DIFFRACTION_RADIUS || calDistance(edge.xstart, edge.ystart, x, y) < IS_DIFFRACTION_RADIUS) {
		return true;
	}
	else {
		return false;
	}
}

//__global__ void diffractionJudgeIsTouched(MyVector *vector, Grids *grids, Points *points, Point *point,int N) {
//	int i = blockDim.x * blockIdx.x + threadIdx.x;
//	if (i < N) {
//		int row = vector[i].row;
//		int col = vector[i].col;
//		int oldRow = row;
//		int oldCol = col;
//		double x = vector[i].x;
//		double y = vector[i].y;
//		double vectorX = vector[i].vectorX;
//		double vectorY = vector[i].vectorY;
//		while (true) {
//			oldRow = row;
//			oldCol = col;
//			if (row < 0 || col < 0 || row >= ROW || col >= COL)
//				break;
//			int indexC = row * COL + col;
//			//���ȼ���������һ���ߵĽ��㹹��һ���߶�
//			int currentX = col * LENGTH;
//			int currentY = row * LENGTH;
//			//��Ҫ�ж��Ƿ����
//			double newX = x, newY = y;
//			if (vectorX > 0 && vectorY > 0) {
//				if ((currentX + LENGTH - x)*vectorY > (currentY + LENGTH - y)*vectorX) {
//					//top
//					newX = x + (currentY + LENGTH - y)*vectorX / vectorY;
//					newY = currentY + LENGTH;
//					row++;
//				}
//				else if ((currentX + LENGTH - x)*vectorY == (currentY + LENGTH - y)*vectorX) {
//					newX = currentX + LENGTH;
//					newY = currentY + LENGTH;
//					row++;
//					col++;
//				}
//				else {
//					//right
//					newX = currentX + LENGTH;
//					newY = vectorY / vectorX * (currentX + 2 - x) + y;
//					col++;
//				}
//			}
//			else if (vectorX > 0 && vectorY < 0) {
//				if (-vectorY * (currentX + LENGTH - x) < vectorX*(y - currentY)) {
//					//right
//					newY = y + (currentX + 2 - x)*vectorY / vectorX;
//					newX = currentX + LENGTH;
//					col++;
//				}
//				else if (-vectorY * (currentX + LENGTH - x) == vectorX * (y - currentY)) {
//					newX = currentX + LENGTH;
//					newY = currentY;
//					col++;
//					row--;
//				}
//				else {
//					//bottom
//					newX = x - vectorX / vectorY * (y - currentY);
//					newY = currentY;
//					row--;
//				}
//			}
//			else if (vectorX < 0 && vectorY>0) {
//				if (-vectorX * (currentY + LENGTH - y) < vectorY*(x - currentX)) {
//					//top
//					newX = x + (vectorX / vectorY * (currentY + LENGTH - y));
//					newY = currentY + LENGTH;
//					row++;
//				}
//				else if (-vectorX * (currentY + LENGTH - y) == vectorY * (x - currentX)) {
//					newX = currentX;
//					newY = currentY + LENGTH;
//					row++;
//					col--;
//				}
//				else {
//					//left
//					newY = y + (vectorY / vectorX * (currentX - x));
//					newX = currentX;
//					col--;
//				}
//			}
//			else if (vectorX < 0 && vectorY < 0) {
//				if (vectorY*(x - currentX) < vectorX*(y - currentY)) {
//					//bottom
//					newX = x - vectorX / vectorY * (y - currentY);
//					newY = currentY;
//					row--;
//				}
//				else if (vectorY*(x - currentX) == vectorX * (y - currentY)) {
//					newX = currentX;
//					newY = currentY;
//					row--;
//					col--;
//				}
//				else {
//					//left
//					newY = y - (x - currentX)*vectorY / vectorX;
//					newX = currentX;
//					col--;
//				}
//			}
//			else if (vectorX == 0) {
//				newY = vectorY > 0 ? currentY + LENGTH : currentY;
//				row = vectorY > 0 ? row + 1 : row - 1;
//			}
//			else if (vectorY == 0) {
//				newX = vectorX > 0 ? currentX + LENGTH : currentY;
//				col = vectorX > 0 ? col + 1 : col - 1;
//			}
//			//vectorX vectorY������ͬʱΪ0
//			//��������а����߻��߽��յ�λ�ڸø�����ʱ������Ҫ����
//			if (grids->grids[indexC].isContains || (oldRow == point->row&&oldCol == point->col)) {
//				//contains edge
//				double distanceM = std::sqrt(2.0)*LENGTH + 1;
//				bool isFind = false;
//				double insertPointOutX = 0, insertPointOutY = 0;
//				int edgeIndex = -1;
//
//				for (int k = 0; k < grids->grids[indexC].N; k++) {
//					if (segmentsIntersect(x, y, newX, newY, grids->grids[indexC].edges[k].xstart, grids->grids[indexC].edges[k].ystart, grids->grids[indexC].edges[k].xend, grids->grids[indexC].edges[k].yend) == true) {
//						double insertPointX = 0, insertPointY = 0;//��ǰ����
//						calculPointOfIntersection(x, y, newX, newY, grids->grids[indexC].edges[k].xstart, grids->grids[indexC].edges[k].ystart, grids->grids[indexC].edges[k].xend, grids->grids[indexC].edges[k].yend, insertPointX, insertPointY);
//						if (currentX > insertPointX || insertPointX > currentX + LENGTH || currentY > insertPointY || insertPointY > currentY + LENGTH) {
//							//������������������
//							continue;
//						}
//						double tempDistance = calDistance(x, y, insertPointX, insertPointY);
//						//��ô���õ�Ŀ���Ǵ��ڷ�����µ������Ȼ����������ڣ�������һ��ǽ���ϣ��ű�Ȼ�����ǽ���ཻ�ģ������Ҫ���˵��������
//						//��ֹ�������Լ�
//						if (abs(insertPointX - x) < 0.0001 && abs(insertPointY - y) < 0.0001)
//							continue;
//						if (tempDistance < distanceM) {
//							isFind = true;
//							distanceM = tempDistance;
//							insertPointOutX = insertPointX;
//							insertPointOutY = insertPointY;
//							edgeIndex = k;
//						}
//					}
//				}
//				bool isReached = false;
//				if (oldRow == point->row&&oldCol == point->col) {
//					//��Ŀ��������������ʱ����Ҫ�ж��Ƿ���Ŀ����ཻ
//					if (x != newX && y != newY) {
//						double targetDistance = calculDistancePointAndLine(x, y, newX, newY, point->x, point->y);
//						//ֻ���ڲ���Բ�뾶���Ҿ����������������н���
//						if (targetDistance < CAPTURE_RADIUS) {
//							double targetPointDistance = calDistance(x, y, point->x, point->y);
//							if (targetPointDistance < distanceM) {
//								distanceM = targetPointDistance;
//								insertPointOutX = point->x;
//								insertPointOutY = point->y;
//								isReached = true;
//								isFind = true;
//							}
//						}
//					}
//				}
//
//				if (isFind&&isReached) {
//					//��Ŀ����ཻ����������
//					points[i].point[points[i].N].x = insertPointOutX;
//					points[i].point[points[i].N].y = insertPointOutY;
//					points[i].N++;
//					points[i].isFind = true;
//					break;
//				}
//				//�Ѿ���������ˣ�����н��㣬����Ҫ�������ߴ���ķ���
//				//����������䣬��Ҫ�ڵ�ǰ������һ���µķ������
//				else if (isFind) {
//					//��ǰ���ߵ�����
//					double currentVectorX = x - insertPointOutX;
//					double currentVectorY = y - insertPointOutY;
//					//Ҫ��������
//					//���������ǶԳ�
//					// \  |  /
//					//  \ | /
//					//   \|/
//					//------------------
//					//����ǽ��ķ�����
//					//grids->grids[indexC].edges[k]
//					//֮ǰ���ǽ��ķ�������˳ʱ�뷽��ģ�Ҳ���ǳ��ڣ���������Ĭ������ֻ���ⲿ�����
//					//��˷�������ȡ��
//					double edgeVectorX = -grids->grids[indexC].edges[edgeIndex].vectorX;
//					double edgeVectorY = -grids->grids[indexC].edges[edgeIndex].vectorY;
//
//					if (edgeVectorX*currentVectorX + edgeVectorY * currentVectorY < 0) {
//						//�н��������90������Ҫת��
//						edgeVectorX = -edgeVectorX;
//						edgeVectorY = -edgeVectorY;
//					}
//					//ע�⣬ǽ��ķ�����Ϊ��λ����
//					//���������ڷ������ϵ�ͶӰΪ(uv)v 
//					//��������Ϊ2(uv)v-u
//					//�ο�https://www.cnblogs.com/graphics/archive/2013/02/21/2920627.html
//
//					double temp = currentVectorX * edgeVectorX + currentVectorY * edgeVectorY;//uv
//					double edgeProjectVectorX = temp * edgeVectorX;
//					double edgeProjectVectorY = temp * edgeVectorY;
//					//������ϣ��������ݣ�������д�����Points
//					vectorX = 2 * edgeProjectVectorX - currentVectorX;
//					vectorY = 2 * edgeProjectVectorY - currentVectorY;
//					normalizeVector(vectorX, vectorY);//�����Ժ���Ҫ��һ��
//					x = insertPointOutX;//���·������
//					y = insertPointOutY;
//					points[i].point[points[i].N].x = x;
//					points[i].point[points[i].N].y = y;
//					points[i].N++;
//					row = oldRow;
//					col = oldCol;
//					if (points[i].N > MAX_REFLECTION_TIMES) {
//						//������������,û�ҵ����㣬��˵���Ϊ0
//						points[i].N = 0;
//						points[i].isFind = false;
//						break;
//					}
//					//}
//
//				}
//				else {
//					x = newX;
//					y = newY;
//				}
//			}
//			else {
//				//��������
//				x = newX;
//				y = newY;
//			}
//			//printf("current position:%f,%f", x, y);
//			//�����newX��newY�ǽ�����������꣬��ʱ��Ҫ�жϸ����߶�������еı��Ƿ��ཻ
//			//break;
//			//if (grids[indexC].grids->isContains) {
//			//	
//			//	for (int k = 0; k < grids[indexC].grids->N; i++) {
//			//		//judge weather is intersecting
//			//	}
//
//			//}
//			//else {
//
//
//			//	//��Ҫ�ҵ��������ĸ���
//			//	//update vectors
//			//	//remain direction update x and y
//			//}
//		}
//		//vector[i].
//	}
//}
__global__ void judegTouchedDiffraction(MyVector *vector, Grids *grids, Points *diffractionPoints, Point *point, int N, int currentPointsIndex) {
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < N) {
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
			//���ȼ���������һ���ߵĽ��㹹��һ���߶�
			int currentX = col * LENGTH;
			int currentY = row * LENGTH;
			//��Ҫ�ж��Ƿ����
			double newX = x, newY = y;
			if (vectorX > 0 && vectorY > 0) {
				if ((currentX + LENGTH - x)*vectorY > (currentY + LENGTH - y)*vectorX) {
					//top
					newX = x + (currentY + LENGTH - y)*vectorX / vectorY;
					newY = currentY + LENGTH;
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
			//vectorX vectorY������ͬʱΪ0
			//��������а����߻��߽��յ�λ�ڸø�����ʱ������Ҫ����
			if (grids->grids[indexC].isContains || (oldRow == point->row&&oldCol == point->col)) {
				//contains edge
				double distanceM = std::sqrt(2.0)*LENGTH + 1;
				bool isFind = false;
				double insertPointOutX = 0, insertPointOutY = 0;
				int edgeIndex = -1;

				for (int k = 0; k < grids->grids[indexC].N; k++) {
					if (segmentsIntersect(x, y, newX, newY, grids->grids[indexC].edges[k].xstart, grids->grids[indexC].edges[k].ystart, grids->grids[indexC].edges[k].xend, grids->grids[indexC].edges[k].yend) == true) {
						double insertPointX = 0, insertPointY = 0;//��ǰ����
						calculPointOfIntersection(x, y, newX, newY, grids->grids[indexC].edges[k].xstart, grids->grids[indexC].edges[k].ystart, grids->grids[indexC].edges[k].xend, grids->grids[indexC].edges[k].yend, insertPointX, insertPointY);
						if (currentX > insertPointX || insertPointX > currentX + LENGTH || currentY > insertPointY || insertPointY > currentY + LENGTH) {
							//������������������
							continue;
						}
						double tempDistance = calDistance(x, y, insertPointX, insertPointY);
						//��ô���õ�Ŀ���Ǵ��ڷ�����µ������Ȼ����������ڣ�������һ��ǽ���ϣ��ű�Ȼ�����ǽ���ཻ�ģ������Ҫ���˵��������
						//��ֹ�������Լ�
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
					//��Ŀ��������������ʱ����Ҫ�ж��Ƿ���Ŀ����ཻ
					if (x != newX && y != newY) {
						double targetDistance = calculDistancePointAndLine(x, y, newX, newY, point->x, point->y);
						//ֻ���ڲ���Բ�뾶���Ҿ����������������н���
						if (targetDistance < CAPTURE_RADIUS) {
							double targetPointDistance = calDistance(x, y, point->x, point->y);
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
					//��Ŀ����ཻ����������
					diffractionPoints[currentPointsIndex*N + i].point[diffractionPoints[currentPointsIndex*N + i].N].x = insertPointOutX;
					diffractionPoints[currentPointsIndex*N + i].point[diffractionPoints[currentPointsIndex*N + i].N].y = insertPointOutY;
					diffractionPoints[currentPointsIndex*N + i].N++;
					diffractionPoints[currentPointsIndex*N + i].isFind = true;
					break;
				}
				//�Ѿ���������ˣ�����н��㣬����Ҫ�������ߴ���ķ���
				//����������䣬��Ҫ�ڵ�ǰ������һ���µķ������
				else if (isFind) {
					//�ڴ˴��ж��Ƿ�������
					//isDiffraction����Ϊfalse���ܷ��ʸô�
					//if ((!isDiffraction) && isEndPoint(grids->grids[indexC].edges[edgeIndex], insertPointOutX, insertPointOutY)) {
					//	if (calDistance(grids->grids[indexC].edges[edgeIndex].xend, grids->grids[indexC].edges[edgeIndex].yend, insertPointOutX, insertPointOutY) < calDistance(grids->grids[indexC].edges[edgeIndex].xstart, grids->grids[indexC].edges[edgeIndex].ystart, insertPointOutX, insertPointOutY)) {
					//		insertPointOutX = grids->grids[indexC].edges[edgeIndex].xend;
					//		insertPointOutY = grids->grids[indexC].edges[edgeIndex].yend;
					//	}
					//	else {
					//		insertPointOutX = grids->grids[indexC].edges[edgeIndex].xstart;
					//		insertPointOutY = grids->grids[indexC].edges[edgeIndex].ystart;
					//	}
					//	//���������䣬�����µ��߳�
					//	MyVector *myVectorNew = (struct MyVector *)malloc(sizeof(struct MyVector) * N);
					//	points[i].point[points[i].N].x = insertPointOutX;
					//	points[i].point[points[i].N].y = insertPointOutY;
					//	points[i].N++;
					//	if (points[i].N > MAX_REFLECTION_TIMES) {
					//		//������������,û�ҵ����㣬��˵���Ϊ0
					//		points[i].N = 0;
					//		points[i].isFind = false;
					//		break;
					//	}
					//	else {
					//		points[i].isFind = true;
					//	}
					//	for (int m = 0; m < N; m++) {
					//		//myVector[i]= (struct MyVector *)malloc(sizeof(struct MyVector));
					//		//��Ҫ�������д��points

					//		myVectorNew[m].x = insertPointOutX;
					//		myVectorNew[m].y = insertPointOutY;
					//		double angle = 2 * PIs / N * i;
					//		myVectorNew[m].vectorX = cos(angle);
					//		myVectorNew[m].vectorY = sin(angle);
					//		myVectorNew[m].col = int(insertPointOutX / 2);
					//		myVectorNew[m].row = int(insertPointOutY / 2);
					//		//MyVector *cuda_myVectorNew;
					//		//cudaMalloc(&cuda_myVectorNew, sizeof(struct MyVector) * N);
					//		//cudaMemcpy(cuda_myVectorNew, myVectorNew, sizeof(struct MyVector) * N, cudaMemcpyHostToDevice);


					//	}
					//	int threadsPerBlock = 90;
					//	int blocksPerGrid = N / 90;
					//	//judgeIsTouched << <blocksPerGrid, threadsPerBlock >> > (myVectorNew, grids, points, diffractionPoints, point, N, true, i);
					//	//ִ�н����Ժ󽫲�������ִ��
					//	break;
					//}
					//else {
						//Severity	Code	Description	Project	File	Line	Suppression State
						//Error		kernel launch from __device__ or __global__ functions requires separate compilation mode	Demo1	D : \program\qt\R\RayTracingCUDA\raytracing3.0\Demo1\MyRayTracingCUDA.cu	574

						//��ǰ���ߵ�����
					double currentVectorX = x - insertPointOutX;
					double currentVectorY = y - insertPointOutY;
					//Ҫ��������
					//���������ǶԳ�
					// \  |  /
					//  \ | /
					//   \|/
					//------------------
					//����ǽ��ķ�����
					//grids->grids[indexC].edges[k]
					//֮ǰ���ǽ��ķ�������˳ʱ�뷽��ģ�Ҳ���ǳ��ڣ���������Ĭ������ֻ���ⲿ�����
					//��˷�������ȡ��
					double edgeVectorX = grids->grids[indexC].edges[edgeIndex].vectorX;
					double edgeVectorY = grids->grids[indexC].edges[edgeIndex].vectorY;

					if (edgeVectorX*currentVectorX + edgeVectorY * currentVectorY < 0) {
						//˵��������ǽ���ڲ������ֱ�ӽ���
						//��������
						//�н��������90������Ҫת��
						//edgeVectorX = -edgeVectorX;
						//edgeVectorY = -edgeVectorY;
						diffractionPoints[currentPointsIndex*N + i].N = 0;
						diffractionPoints[currentPointsIndex*N + i].isFind = false;
						break;
					}
					else {
						double temp = currentVectorX * edgeVectorX + currentVectorY * edgeVectorY;//uv
						double edgeProjectVectorX = temp * edgeVectorX;
						double edgeProjectVectorY = temp * edgeVectorY;
						//������ϣ��������ݣ�������д�����Points
						vectorX = 2 * edgeProjectVectorX - currentVectorX;
						vectorY = 2 * edgeProjectVectorY - currentVectorY;
						normalizeVector(vectorX, vectorY);//�����Ժ���Ҫ��һ��
						x = insertPointOutX;//���·������
						y = insertPointOutY;
						//if (isDiffraction) {
							//˵���������������,��Ҫ������д����Ӧ��λ��
						diffractionPoints[currentPointsIndex*N + i].point[diffractionPoints[currentPointsIndex*N + i].N].x = x;
						diffractionPoints[currentPointsIndex*N + i].point[diffractionPoints[currentPointsIndex*N + i].N].y = y;
						diffractionPoints[currentPointsIndex*N + i].N++;
						if (diffractionPoints[currentPointsIndex*N + i].N > MAX_REFLECTION_TIMES_AFTER_DIFFRACTION) {
							diffractionPoints[currentPointsIndex*N + i].N = 0;
							diffractionPoints[currentPointsIndex*N + i].isFind = false;
							break;
						}
						//}
						//else {
						//	points[i].point[points[i].N].x = x;
						//	points[i].point[points[i].N].y = y;
						//	points[i].N++;
						//	if (points[i].N > MAX_REFLECTION_TIMES) {
						//		//������������,û�ҵ����㣬��˵���Ϊ0
						//		points[i].N = 0;
						//		points[i].isFind = false;
						//		break;
						//	}
						//}
						row = oldRow;
						col = oldCol;
					}
					//ע�⣬ǽ��ķ�����Ϊ��λ����
					//���������ڷ������ϵ�ͶӰΪ(uv)v 
					//��������Ϊ2(uv)v-u
					//�ο�https://www.cnblogs.com/graphics/archive/2013/02/21/2920627.html


				//}
				//}

				}
				else {
					x = newX;
					y = newY;
				}
			}
			else {
				//��������
				x = newX;
				y = newY;
			}
			//printf("current position:%f,%f", x, y);
			//�����newX��newY�ǽ�����������꣬��ʱ��Ҫ�жϸ����߶�������еı��Ƿ��ཻ
			//break;
			//if (grids[indexC].grids->isContains) {
			//	
			//	for (int k = 0; k < grids[indexC].grids->N; i++) {
			//		//judge weather is intersecting
			//	}

			//}
			//else {


			//	//��Ҫ�ҵ��������ĸ���
			//	//update vectors
			//	//remain direction update x and y
			//}
		}
		//vector[i].
	}
}


/*
vector is input
points is output
points�Ǵ洢������ߵı���
point�Ǵ洢RX��ı���
N�����߷ֱ����
*/
__global__ void judgeIsTouched(MyVector *vector, Grids *grids, Points *points, Points *diffractionPoints, Point *point, int N, int currentPointsIndex) {
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < N) {
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
			//���ȼ���������һ���ߵĽ��㹹��һ���߶�
			int currentX = col * LENGTH;
			int currentY = row * LENGTH;
			//��Ҫ�ж��Ƿ����
			double newX = x, newY = y;
			if (vectorX > 0 && vectorY > 0) {
				if ((currentX + LENGTH - x)*vectorY > (currentY + LENGTH - y)*vectorX) {
					//top
					newX = x + (currentY + LENGTH - y)*vectorX / vectorY;
					newY = currentY + LENGTH;
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
			//vectorX vectorY������ͬʱΪ0
			//��������а����߻��߽��յ�λ�ڸø�����ʱ������Ҫ����
			if (grids->grids[indexC].isContains || (oldRow == point->row&&oldCol == point->col)) {
				//contains edge
				double distanceM = std::sqrt(2.0)*LENGTH + 1;
				bool isFind = false;
				double insertPointOutX = 0, insertPointOutY = 0;
				int edgeIndex = -1;

				for (int k = 0; k < grids->grids[indexC].N; k++) {
					if (segmentsIntersect(x, y, newX, newY, grids->grids[indexC].edges[k].xstart, grids->grids[indexC].edges[k].ystart, grids->grids[indexC].edges[k].xend, grids->grids[indexC].edges[k].yend) == true) {
						double insertPointX = 0, insertPointY = 0;//��ǰ����
						calculPointOfIntersection(x, y, newX, newY, grids->grids[indexC].edges[k].xstart, grids->grids[indexC].edges[k].ystart, grids->grids[indexC].edges[k].xend, grids->grids[indexC].edges[k].yend, insertPointX, insertPointY);
						if (currentX > insertPointX || insertPointX > currentX + LENGTH || currentY > insertPointY || insertPointY > currentY + LENGTH) {
							//������������������
							continue;
						}
						double tempDistance = calDistance(x, y, insertPointX, insertPointY);
						//��ô���õ�Ŀ���Ǵ��ڷ�����µ������Ȼ����������ڣ�������һ��ǽ���ϣ��ű�Ȼ�����ǽ���ཻ�ģ������Ҫ���˵��������
						//��ֹ�������Լ�
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
					//��Ŀ��������������ʱ����Ҫ�ж��Ƿ���Ŀ����ཻ
					if (x != newX && y != newY) {
						double targetDistance = calculDistancePointAndLine(x, y, newX, newY, point->x, point->y);
						//ֻ���ڲ���Բ�뾶���Ҿ����������������н���
						if (targetDistance < CAPTURE_RADIUS) {
							double targetPointDistance = calDistance(x, y, point->x, point->y);
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
					//��Ŀ����ཻ����������
					points[i].point[points[i].N].x = insertPointOutX;
					points[i].point[points[i].N].y = insertPointOutY;
					points[i].N++;
					points[i].isFind = true;
					break;
				}
				//�Ѿ���������ˣ�����н��㣬����Ҫ�������ߴ���ķ���
				//����������䣬��Ҫ�ڵ�ǰ������һ���µķ������
				else if (isFind) {
					//�ڴ˴��ж��Ƿ�������
					//isDiffraction����Ϊfalse���ܷ��ʸô�
					if (isEndPoint(grids->grids[indexC].edges[edgeIndex], insertPointOutX, insertPointOutY)) {
						if (calDistance(grids->grids[indexC].edges[edgeIndex].xend, grids->grids[indexC].edges[edgeIndex].yend, insertPointOutX, insertPointOutY) < calDistance(grids->grids[indexC].edges[edgeIndex].xstart, grids->grids[indexC].edges[edgeIndex].ystart, insertPointOutX, insertPointOutY)) {
							insertPointOutX = grids->grids[indexC].edges[edgeIndex].xend;
							insertPointOutY = grids->grids[indexC].edges[edgeIndex].yend;
						}
						else {
							insertPointOutX = grids->grids[indexC].edges[edgeIndex].xstart;
							insertPointOutY = grids->grids[indexC].edges[edgeIndex].ystart;
						}
						//���������䣬�����µ��߳�
						MyVector *myVectorNew = (struct MyVector *)malloc(sizeof(struct MyVector) * N);
						points[i].point[points[i].N].x = insertPointOutX;
						points[i].point[points[i].N].y = insertPointOutY;
						points[i].N++;
						if (points[i].N > MAX_REFLECTION_TIMES) {
							//������������,û�ҵ����㣬��˵���Ϊ0
							points[i].N = 0;
							points[i].isFind = false;
							break;
						}
						/*else {
							points[i].isFind = true;
						}*/
						for (int m = 0; m < N; m++) {
							//myVector[i]= (struct MyVector *)malloc(sizeof(struct MyVector));
							//��Ҫ�������д��points

							myVectorNew[m].x = insertPointOutX;
							myVectorNew[m].y = insertPointOutY;
							double angle = 2 * PIs / N * m;
							myVectorNew[m].vectorX = cos(angle);
							myVectorNew[m].vectorY = sin(angle);
							myVectorNew[m].col = int(insertPointOutX / 2);
							myVectorNew[m].row = int(insertPointOutY / 2);
							//MyVector *cuda_myVectorNew;
							//cudaMalloc(&cuda_myVectorNew, sizeof(struct MyVector) * N);
							//cudaMemcpy(cuda_myVectorNew, myVectorNew, sizeof(struct MyVector) * N, cudaMemcpyHostToDevice);


						}
						int threadsPerBlock = 180;
						int blocksPerGrid = N / 180;
						//__syncthreads();
						judegTouchedDiffraction << <blocksPerGrid, threadsPerBlock >> > (myVectorNew, grids, diffractionPoints, point, N, i);
						//__syncthreads();
						//judgeIsTouched << <blocksPerGrid, threadsPerBlock >> > (myVectorNew, grids, points, diffractionPoints, point, N, true, i);
						//ִ�н����Ժ󽫲�������ִ��
						break;
					}
					else {
						//Severity	Code	Description	Project	File	Line	Suppression State
						//Error		kernel launch from __device__ or __global__ functions requires separate compilation mode	Demo1	D : \program\qt\R\RayTracingCUDA\raytracing3.0\Demo1\MyRayTracingCUDA.cu	574

						//��ǰ���ߵ�����
						double currentVectorX = x - insertPointOutX;
						double currentVectorY = y - insertPointOutY;
						//Ҫ��������
						//���������ǶԳ�
						// \  |  /
						//  \ | /
						//   \|/
						//------------------
						//����ǽ��ķ�����
						//grids->grids[indexC].edges[k]
						//֮ǰ���ǽ��ķ�������˳ʱ�뷽��ģ�Ҳ���ǳ��ڣ���������Ĭ������ֻ���ⲿ�����
						//��˷�������ȡ��
						double edgeVectorX = grids->grids[indexC].edges[edgeIndex].vectorX;
						double edgeVectorY = grids->grids[indexC].edges[edgeIndex].vectorY;

						if (edgeVectorX*currentVectorX + edgeVectorY * currentVectorY < 0) {
							//˵��������ǽ���ڲ������ֱ�ӽ���
							//��������
							//�н��������90������Ҫת��
							//edgeVectorX = -edgeVectorX;
							//edgeVectorY = -edgeVectorY;
							points[i].N = 0;
							points[i].isFind = false;
							break;
						}
						else {
							double temp = currentVectorX * edgeVectorX + currentVectorY * edgeVectorY;//uv
							double edgeProjectVectorX = temp * edgeVectorX;
							double edgeProjectVectorY = temp * edgeVectorY;
							//������ϣ��������ݣ�������д�����Points
							vectorX = 2 * edgeProjectVectorX - currentVectorX;
							vectorY = 2 * edgeProjectVectorY - currentVectorY;
							normalizeVector(vectorX, vectorY);//�����Ժ���Ҫ��һ��
							x = insertPointOutX;//���·������
							y = insertPointOutY;
							//if (isDiffraction) {
							//	//˵���������������,��Ҫ������д����Ӧ��λ��
							//	diffractionPoints[currentPointsIndex*N + i].point[diffractionPoints[currentPointsIndex*N + i].N].x = x;
							//	diffractionPoints[currentPointsIndex*N + i].point[diffractionPoints[currentPointsIndex*N + i].N].y = y;
							//	diffractionPoints[currentPointsIndex*N + i].N++;
							//	if (diffractionPoints[currentPointsIndex*N + i].N > MAX_REFLECTION_TIMES) {
							//		diffractionPoints[currentPointsIndex*N + i].N = 0;
							//		diffractionPoints[currentPointsIndex*N + i].isFind = false;
							//		break;
							//	}
							//}
							//else {
							points[i].point[points[i].N].x = x;
							points[i].point[points[i].N].y = y;
							points[i].N++;
							if (points[i].N > MAX_REFLECTION_TIMES) {
								//������������,û�ҵ����㣬��˵���Ϊ0
								points[i].N = 0;
								points[i].isFind = false;
								break;
							}
							//}
							row = oldRow;
							col = oldCol;
						}
						//ע�⣬ǽ��ķ�����Ϊ��λ����
						//���������ڷ������ϵ�ͶӰΪ(uv)v 
						//��������Ϊ2(uv)v-u
						//�ο�https://www.cnblogs.com/graphics/archive/2013/02/21/2920627.html


					}
					//}

				}
				else {
					x = newX;
					y = newY;
				}
			}
			else {
				//��������
				x = newX;
				y = newY;
			}
			//printf("current position:%f,%f", x, y);
			//�����newX��newY�ǽ�����������꣬��ʱ��Ҫ�жϸ����߶�������еı��Ƿ��ཻ
			//break;
			//if (grids[indexC].grids->isContains) {
			//	
			//	for (int k = 0; k < grids[indexC].grids->N; i++) {
			//		//judge weather is intersecting
			//	}

			//}
			//else {


			//	//��Ҫ�ҵ��������ĸ���
			//	//update vectors
			//	//remain direction update x and y
			//}
		}
		//vector[i].
	}
}
/*
grid_input:�洢��������Ϣ��ÿ�������д��ڱߣ���Щ��Ҳ�ṹ������
N:��ʾ�����������ȼ�����֣����N=360����ÿ���Ƕ�Ϊ1��
*/
vector<vector<double>> initCUDAInput(Grids *grids, double TX_X, double TX_Y, double RX_X, double RX_Y, int N) {
	//��Ҫ��ʼ������
	//grids->�洢����
	//������������Ϊ50*50��С������ɣ�������������С����Щ��������ȫ����Ϊ��
	//int N = 180;
	printf("Start...%d\n", sizeof(bool));
	clock_t start, finish;
	start = clock();
	struct Grids *cuda_grids;

	MyVector *myVector = (struct MyVector *)malloc(sizeof(struct MyVector) * N);
	Points *points = (struct Points *)malloc(sizeof(struct Points) * N);
	//������Ҫ����һ���ṹ�������洢����õ��Ĳ�������ط��и������ǣ����N=360������360��ray�����ܲ������䣬������ʵ�������������Ŀ����ԱȽ�С�������ô��ռ���˷���
	Points *diffractionPoints = (struct Points *)malloc(sizeof(struct Points) * N*N);
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
		myVector[i].col = int(TX_X / 2);
		myVector[i].row = int(TX_Y / 2);
		points[i].N = 0;
		points[i].isFind = false;
		for (int j = 0; j < N; j++) {
			diffractionPoints[i*N + j].N = 0;
			diffractionPoints[i*N + j].isFind = false;
		}
	}
	MyVector *cuda_myVector;
	Points *cuda_points;
	Point *cuda_Rx;
	Points *cuda_diffractionPoints;

	cudaMalloc(&cuda_grids, sizeof(struct Grids));
	cudaMalloc(&cuda_myVector, sizeof(struct MyVector) * N);
	cudaMalloc(&cuda_points, sizeof(struct Points) * N);
	cudaMalloc(&cuda_diffractionPoints, sizeof(struct Points) * N*N);
	cudaMalloc(&cuda_Rx, sizeof(struct Point));

	cudaMemcpy(cuda_grids, grids, sizeof(struct Grids), cudaMemcpyHostToDevice);
	cudaMemcpy(cuda_myVector, myVector, sizeof(struct MyVector) * N, cudaMemcpyHostToDevice);
	cudaMemcpy(cuda_points, points, sizeof(struct Points) * N, cudaMemcpyHostToDevice);
	cudaMemcpy(cuda_diffractionPoints, diffractionPoints, sizeof(struct Points) * N*N, cudaMemcpyHostToDevice);
	cudaMemcpy(cuda_Rx, Rx, sizeof(struct Point), cudaMemcpyHostToDevice);
	//��ط�Ӧ�����������
	int threadsPerBlock = 90;
	int blocksPerGrid = N / 90;

	judgeIsTouched << <blocksPerGrid, threadsPerBlock >> > (cuda_myVector, cuda_grids, cuda_points, cuda_diffractionPoints, cuda_Rx, N, 0);

	cudaMemcpy(points, cuda_points, sizeof(struct Points) * N, cudaMemcpyDeviceToHost);
	cudaMemcpy(diffractionPoints, cuda_diffractionPoints, sizeof(struct Points) * N*N, cudaMemcpyDeviceToHost);
	vector<vector<double>> getResult;
	int pointsCount = 0;
	for (int i = 0; i < N; i++) {
		if (points[i].isFind) {
			vector<double> results;
			for (int j = 0; j < points[i].N; j++) {
				results.push_back(points[i].point[j].x);
				results.push_back(points[i].point[j].y);
			}
			getResult.push_back(results);
		}
		else
			if (points[i].N > 0) {
				vector<double> results;
				for (int j = 0; j < points[i].N; j++) {
					results.push_back(points[i].point[j].x);
					results.push_back(points[i].point[j].y);
				}
				//getResult.push_back(results);
				for (int m = 0; m < N; m++) {
					//qDebug() << "come to here"+ diffractionPoints[i*N + m].N;
					if (diffractionPoints[i*N + m].isFind) {
						vector<double> diffractionPo(results);
						for (int n = 0; n < diffractionPoints[i*N + m].N; n++) {
							diffractionPo.push_back(diffractionPoints[i*N + m].point[n].x);
							diffractionPo.push_back(diffractionPoints[i*N + m].point[n].y);
						}
						getResult.push_back(diffractionPo);
					}
				}
			}
	}
	//for (int i = 0; i < N; i++) {
	//	for (int j = 0; j < N; j++) {
	//		//qDebug()<<
	//		vector<double> diffractionPo;
	//		if (diffractionPoints[i*N + j].N > 0) {
	//			for (int n = 0; n < diffractionPoints[i*N + j].N; n++) {
	//				diffractionPo.push_back(diffractionPoints[i*N + j].point[n].x);
	//				diffractionPo.push_back(diffractionPoints[i*N + j].point[n].y);
	//			}
	//			getResult.push_back(diffractionPo);
	//		}

	//	}
	//}
	finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("%f seconds\n", duration);
	//��Ҫ����point
	cudaFree(cuda_grids);
	cudaFree(cuda_myVector);
	cudaFree(cuda_points);
	cudaFree(cuda_Rx);
	cudaFree(cuda_diffractionPoints);
	free(myVector);
	return getResult;
}

vector<QString> getCUDAInformation() {
	vector<QString> output;
	cudaDeviceProp deviceProp;
	int deviceCount;
	cudaError_t cudaError;
	cudaError = cudaGetDeviceCount(&deviceCount);
	for (int i = 0; i < deviceCount; i++)
	{
		cudaError = cudaGetDeviceProperties(&deviceProp, i);
		output.push_back(QString("%1").arg(i));
		output.push_back(QString(deviceProp.name));
		output.push_back(QString("%1").arg((int)(deviceProp.totalGlobalMem / 1024 / 1024)));
		output.push_back(QString("%1").arg((int)(deviceProp.sharedMemPerBlock / 1024)));
		output.push_back(QString("%1").arg((int)(deviceProp.regsPerBlock)));
		output.push_back(QString("%1").arg((int)deviceProp.maxThreadsPerBlock));
		output.push_back(QString("%1").arg(deviceProp.major));
		output.push_back(QString("%1").arg(deviceProp.minor));
		output.push_back(QString("%1").arg(deviceProp.multiProcessorCount));
	}
	return output;
}

//vector<vector<double>> initCUDAInput(vector<vector<vector<double>>> grid_input, double TX_X, double TX_Y, double RX_X, double RX_Y, int N) {
//	//��Ҫ��ʼ������
//	//grids->�洢����
//	//������������Ϊ50*50��С������ɣ�������������С����Щ��������ȫ����Ϊ��
//	//int N = 180;
//	printf("Start...%d\n", sizeof(bool));
//	clock_t start, finish;
//	start = clock();
//	struct Grids *grids = (struct Grids *)malloc(sizeof(struct Grids));
//	grids->width = 2;
//	grids->height = 2;
//	int i = 0;
//	for (; i < grid_input.size(); i++) {
//		if (grid_input[i].size() != 0) {
//			grids->grids[i].N = grid_input[i].size();
//			grids->grids[i].isContains = true;
//			for (int j = 0; j < grid_input[i].size(); j++) {
//				//edge
//				grids->grids[i].edges[j].xstart = grid_input[i][j][0];
//				grids->grids[i].edges[j].ystart = grid_input[i][j][1];
//				grids->grids[i].edges[j].xend = grid_input[i][j][2];
//				grids->grids[i].edges[j].yend = grid_input[i][j][3];
//				grids->grids[i].edges[j].vectorX = grid_input[i][j][4];
//				grids->grids[i].edges[j].vectorY = grid_input[i][j][5];
//			}
//		}
//		else {
//			grids->grids[i].N = 0;
//			grids->grids[i].isContains = false;
//		}
//	}
//	for (; i < ROW*COL; i++) {
//		grids->grids[i].N = 0;
//		grids->grids[i].isContains = false;
//	}
//	struct Grids *cuda_grids;
//
//	MyVector *myVector = (struct MyVector *)malloc(sizeof(struct MyVector) * N);
//	Points *points = (struct Points *)malloc(sizeof(struct Points) * N);
//	Point *Rx = (struct Point *)malloc(sizeof(struct Point));
//	Rx->x = RX_X;
//	Rx->y = RX_Y;
//	Rx->row = int(RX_Y / LENGTH);
//	Rx->col = int(RX_X / LENGTH);
//
//	for (int i = 0; i < N; i++) {
//		//myVector[i]= (struct MyVector *)malloc(sizeof(struct MyVector));
//		myVector[i].x = TX_X;
//		myVector[i].y = TX_Y;
//		double angle = 2 * PIs / N * i;
//		myVector[i].vectorX = cos(angle);
//		myVector[i].vectorY = sin(angle);
//		myVector[i].col = 0;
//		myVector[i].row = 1;
//		points[i].N = 0;
//	}
//	MyVector *cuda_myVector;
//	Points *cuda_points;
//	Point *cuda_Rx;
//	cudaMalloc(&cuda_grids, sizeof(struct Grids));
//	cudaMalloc(&cuda_myVector, sizeof(struct MyVector) * N);
//	cudaMalloc(&cuda_points, sizeof(struct Points) * N);
//	cudaMalloc(&cuda_Rx, sizeof(struct Point));
//
//	cudaMemcpy(cuda_grids, grids, sizeof(struct Grids), cudaMemcpyHostToDevice);
//	cudaMemcpy(cuda_myVector, myVector, sizeof(struct MyVector) * N, cudaMemcpyHostToDevice);
//	//cudaMemcpy(cuda_points, points, sizeof(struct Points) * 60, cudaMemcpyHostToDevice);
//	cudaMemcpy(cuda_Rx, Rx, sizeof(struct Point), cudaMemcpyHostToDevice);
//	int threadsPerBlock = 256;
//	int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
//	judgeIsTouched << <blocksPerGrid, threadsPerBlock >> > (cuda_myVector, cuda_grids, cuda_points, cuda_Rx, N);
//	cudaMemcpy(points, cuda_points, sizeof(struct Points) * N, cudaMemcpyDeviceToHost);
//
//	vector<vector<double>> getResult;
//	for (int i = 0; i < N; i++) {
//		if (points[i].N != 0) {
//			vector<double> results;
//			for (int j = 0; j < points[i].N; j++) {
//				results.push_back(points[i].point[j].x);
//				results.push_back(points[i].point[j].y);
//				//printf("%d-%d-%d:%f,%f\n", i, j, 2 * i, points[i].point[j].x, points[i].point[j].y);
//			}
//			getResult.push_back(results);
//		}
//	}
//	finish = clock();
//	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	printf("%f seconds\n", duration);
//	//��Ҫ����point
//	cudaFree(cuda_grids);
//	cudaFree(cuda_myVector);
//	cudaFree(cuda_points);
//	cudaFree(cuda_Rx);
//	return getResult;
//}

//int main()
//{
//	//printf("Start...%d\n", sizeof(bool));
//	vector<vector<vector<double>>> grid_input;
//
//	vector<vector<double>> grid0;
//
//	vector<double> edge0;
//	edge0.push_back(1);
//	edge0.push_back(0.5);
//	edge0.push_back(2);
//	edge0.push_back(0.5);
//	edge0.push_back(0);
//	edge0.push_back(1);
//	grid0.push_back(edge0);
//	vector<double> edge1;
//	edge1.push_back(1);
//	edge1.push_back(1.5);
//	edge1.push_back(2);
//	edge1.push_back(1.5);
//	edge1.push_back(0);
//	edge1.push_back(-1);
//	grid0.push_back(edge1);
//
//	vector<double> edge2;
//	edge2.push_back(1);
//	edge2.push_back(0.5);
//	edge2.push_back(1);
//	edge2.push_back(1.5);
//	edge2.push_back(1);
//	edge2.push_back(0);
//	grid0.push_back(edge2);
//	grid_input.push_back(grid0);
//
//	vector<vector<double>> grid1;
//	vector<double> edge3;
//	edge3.push_back(2);
//	edge3.push_back(0.5);
//	edge3.push_back(3);
//	edge3.push_back(0.5);
//	edge3.push_back(0);
//	edge3.push_back(1);
//	grid1.push_back(edge3);
//	vector<double> edge4;
//	edge4.push_back(2);
//	edge4.push_back(1.5);
//	edge4.push_back(3);
//	edge4.push_back(1.5);
//	edge4.push_back(0);
//	edge4.push_back(-1);
//	grid1.push_back(edge4);
//
//	vector<double> edge5;
//	edge5.push_back(3);
//	edge5.push_back(0.5);
//	edge5.push_back(3);
//	edge5.push_back(1.5);
//	edge5.push_back(-1);
//	edge5.push_back(0);
//	grid1.push_back(edge5);
//	grid_input.push_back(grid1);
//
//	vector<vector<double>> grid2;
//	grid_input.push_back(grid2);
//
//	vector<vector<double>> grid3;
//	vector<double> edge6;
//	edge6.push_back(6.5);
//	edge6.push_back(0.5);
//	edge6.push_back(6.5);
//	edge6.push_back(1.5);
//	edge6.push_back(1);
//	edge6.push_back(0);
//	grid3.push_back(edge6);
//	vector<double> edge7;
//	edge7.push_back(7.5);
//	edge7.push_back(0.5);
//	edge7.push_back(6.5);
//	edge7.push_back(0.5);
//	edge7.push_back(0);
//	edge7.push_back(1);
//	grid3.push_back(edge7);
//
//	vector<double> edge8;
//	edge8.push_back(7.5);
//	edge8.push_back(0.5);
//	edge8.push_back(7.5);
//	edge8.push_back(1.5);
//	edge8.push_back(-1);
//	edge8.push_back(0);
//	grid3.push_back(edge8);
//	vector<double> edge9;
//	edge9.push_back(7.5);
//	edge9.push_back(1.5);
//	edge9.push_back(6.5);
//	edge9.push_back(1.5);
//	edge9.push_back(0);
//	edge9.push_back(-1);
//	grid3.push_back(edge9);
//	grid_input.push_back(grid3);
//
//	initCUDAInput(grid_input, 1, 3, 3, 2, 180);
//	//clock_t start, finish;
//	//start = clock();
//	//struct Grids *grids = (struct Grids *)malloc(sizeof(struct Grids));
//
//	////��Ҫ��������װһ��
//	//grids->width = 2;
//	//grids->height = 2;
//
//	//grids->grids[0].N = 3;
//	//grids->grids[0].isContains = true;
//
//	//grids->grids[0].edges[0].xstart = 1;
//	//grids->grids[0].edges[0].ystart = 0.5;
//	//grids->grids[0].edges[0].xend = 2;
//	//grids->grids[0].edges[0].yend = 0.5;
//	//grids->grids[0].edges[0].vectorX = 0;
//	//grids->grids[0].edges[0].vectorY = 1;
//
//	//grids->grids[0].edges[1].xstart = 1;
//	//grids->grids[0].edges[1].ystart = 1.5;
//	//grids->grids[0].edges[1].xend = 2;
//	//grids->grids[0].edges[1].yend = 1.5;
//	//grids->grids[0].edges[1].vectorX = 0;
//	//grids->grids[0].edges[1].vectorY = -1;
//
//	//grids->grids[0].edges[2].xstart = 1;
//	//grids->grids[0].edges[2].ystart = 0.5;
//	//grids->grids[0].edges[2].xend = 1;
//	//grids->grids[0].edges[2].yend = 1.5;
//	//grids->grids[0].edges[2].vectorX = 1;
//	//grids->grids[0].edges[2].vectorY = 0;
//
//	//grids->grids[1].N = 3;
//	//grids->grids[1].isContains = true;
//
//	//grids->grids[1].edges[0].xstart = 2;
//	//grids->grids[1].edges[0].ystart = 0.5;
//	//grids->grids[1].edges[0].xend = 3;
//	//grids->grids[1].edges[0].yend = 0.5;
//	//grids->grids[1].edges[0].vectorX = 0;
//	//grids->grids[1].edges[0].vectorY = 1;
//
//	//grids->grids[1].edges[1].xstart = 2;
//	//grids->grids[1].edges[1].ystart = 1.5;
//	//grids->grids[1].edges[1].xend = 3;
//	//grids->grids[1].edges[1].yend = 1.5;
//	//grids->grids[1].edges[1].vectorX = 0;
//	//grids->grids[1].edges[1].vectorY = -1;
//
//	//grids->grids[1].edges[2].xstart = 3;
//	//grids->grids[1].edges[2].ystart = 0.5;
//	//grids->grids[1].edges[2].xend = 3;
//	//grids->grids[1].edges[2].yend = 1.5;
//	//grids->grids[1].edges[2].vectorX = -1;
//	//grids->grids[1].edges[2].vectorY = 0;
//
//	//grids->grids[2].N = 0;
//	//grids->grids[2].isContains = false;
//
//	//grids->grids[3].N = 4;
//	//grids->grids[3].isContains = true;
//
//	//grids->grids[3].edges[0].xstart = 2.5;
//	//grids->grids[3].edges[0].ystart = 2.5;
//	//grids->grids[3].edges[0].xend = 2.5;
//	//grids->grids[3].edges[0].yend = 3.5;
//	//grids->grids[3].edges[0].vectorX = 1;
//	//grids->grids[3].edges[0].vectorY = 0;
//
//	//grids->grids[3].edges[1].xstart = 3.5;
//	//grids->grids[3].edges[1].ystart = 2.5;
//	//grids->grids[3].edges[1].xend = 3.5;
//	//grids->grids[3].edges[1].yend = 3.5;
//	//grids->grids[3].edges[1].vectorX = -1;
//	//grids->grids[3].edges[1].vectorY = 0;
//
//	//grids->grids[3].edges[2].xstart = 2.5;
//	//grids->grids[3].edges[2].ystart = 2.5;
//	//grids->grids[3].edges[2].xend = 3.5;
//	//grids->grids[3].edges[2].yend = 2.5;
//	//grids->grids[3].edges[2].vectorX = 0;
//	//grids->grids[3].edges[2].vectorY = 1;
//
//	//grids->grids[3].edges[3].xstart = 2.5;
//	//grids->grids[3].edges[3].ystart = 3.5;
//	//grids->grids[3].edges[3].xend = 3.5;
//	//grids->grids[3].edges[3].yend = 3.5;
//	//grids->grids[3].edges[3].vectorX = 0;
//	//grids->grids[3].edges[3].vectorY = -1;
//
//	/*
//	�����ʼ��������ͼ��ʾ��һ��ģ�ͣ����������һ�������
//		 __ __ __ __ __ __ __ __
//		|               __ __   |
//		|     ��       |     |  |
//		|              |__ __|  |
//		|                       |
//		|      __ __ __ __      |
//		|     |           |     |
//		|     |__ __ __ __|     |
//		|__ __ __ __ __ __ __ __|
//	
//	��ֻ�ǲ���һ���ܷ���GPU���㷴�������
//	���������ܹ�ʹ�õģ�����Ϊɶ��ʹ��CPU����û�ж������
//	*/
//	/*grids.grids = new Grid[4];
//	for (int i = 0; i < 4; i++) {
//		grids.grids[i].N = 4;
//		grids.grids[i].isContains = true;
//		grids.grids[i].edges = new Edge[4];
//
//		for (int j = 0; j < 4; j++) {
//			grids.grids[i].edges[j].xstart = 0;
//			grids.grids[i].edges[j].ystart = 0 + i;
//			grids.grids[i].edges[j].xend = 0;
//			grids.grids[i].edges[j].yend = 1 + j;
//			grids.grids[i].edges[j].vectorX = 1;
//			grids.grids[i].edges[j].vectorY = 1;
//		}
//	}*/
//
//	//printf("grids's size%d\n", sizeof(struct Grids));
//	//struct Grids *cuda_grids;
//	//int N = 180;
//	//MyVector *myVector = (struct MyVector *)malloc(sizeof(struct MyVector) * N);
//	//Points *points = (struct Points *)malloc(sizeof(struct Points) * N);
//	//Point *Rx = (struct Point *)malloc(sizeof(struct Point));
//	//Rx->x = 3;
//	//Rx->y = 3;
//
//	//for (int i = 0; i < N; i++) {
//	//	//myVector[i]= (struct MyVector *)malloc(sizeof(struct MyVector));
//	//	myVector[i].x = 1;
//	//	myVector[i].y = 3;
//	//	double angle = 2 * PIs / N * i;
//	//	myVector[i].vectorX = cos(angle);
//	//	myVector[i].vectorY = sin(angle);
//	//	myVector[i].col = 0;
//	//	myVector[i].row = 1;
//	//	points[i].N = 0;
//	//}
//	//MyVector *cuda_myVector;
//	//Points *cuda_points;
//	//Point *cuda_Rx;
//	//cudaMalloc(&cuda_grids, sizeof(struct Grids));
//	//cudaMalloc(&cuda_myVector, sizeof(struct MyVector) * N);
//	//cudaMalloc(&cuda_points, sizeof(struct Points) * N);
//	//cudaMalloc(&cuda_Rx, sizeof(struct Point));
//
//	//cudaMemcpy(cuda_grids, grids, sizeof(struct Grids), cudaMemcpyHostToDevice);
//	//cudaMemcpy(cuda_myVector, myVector, sizeof(struct MyVector) * N, cudaMemcpyHostToDevice);
//	////cudaMemcpy(cuda_points, points, sizeof(struct Points) * 60, cudaMemcpyHostToDevice);
//	//cudaMemcpy(cuda_Rx, Rx, sizeof(struct Point), cudaMemcpyHostToDevice);
//
//
//	//int threadsPerBlock = 256;
//	//int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
//	//judgeIsTouched <<<blocksPerGrid, threadsPerBlock >>> (cuda_myVector, cuda_grids, cuda_points, cuda_Rx, N);
//	//cudaMemcpy(points, cuda_points, sizeof(struct Points) * N, cudaMemcpyDeviceToHost);
//
//
//	//for (int i = 0; i < N; i++) {
//	//	for (int j = 0; j < points[i].N; j++) {
//	//		printf("%d-%d-%d:%f,%f\n",i,j, 2*i,points[i].point[j].x, points[i].point[j].y);
//	//	}
//	//}
//	//finish = clock();
//	//double duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	//printf("%f seconds\n", duration);
//	//cudaFree(cuda_grids);
//	//cudaFree(cuda_myVector);
//	//cudaFree(cuda_points);
//	//cudaFree(cuda_Rx);
//
//
//	//for (int i = 0; i < 4; i++) {
//	//	for (int j = 0; j < 4; j++) {
//	//		printf("%d\n", grids.grids[i].edges[j].xstart);
//	//		//std::cout << grids.grids[i].edges[j].xstart << "," << grids.grids[i].edges[j].ystart << "," << grids.grids[i].edges[j].xend << "," << grids.grids[i].edges[j].yend << std::endl;
//	//	}
//	//}
//	//cudaError_t cudaStatus = addWithCuda(c, a, b, arraySize);
//	//if (cudaStatus != cudaSuccess) {
//	//	fprintf(stderr, "addWithCuda failed!");
//	//	return 1;
//	//}
//
//	//printf("{1,2,3,4,5} + {10,20,30,40,50} = {%d,%d,%d,%d,%d}\n",
//	//	c[0], c[1], c[2], c[3], c[4]);
//
//	//// cudaDeviceReset must be called before exiting in order for profiling and
//	//// tracing tools such as Nsight and Visual Profiler to show complete traces.
//	//cudaStatus = cudaDeviceReset();
//	//if (cudaStatus != cudaSuccess) {
//	//	fprintf(stderr, "cudaDeviceReset failed!");
//	//	return 1;
//	//}
//
//	return 0;
//}
//
