#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "time.h" 
#include "MyCUDARayTracing.h"
#include "MyRayTracingCPU.h"
#include "StaticConstants.h"

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



double RayTracingCPU::directionIntersectCPU(double x1, double y1, double x2, double y2, double x3, double y3) {
	//(x3-x1,y3-y1)X(x2-x1,y2-y1)
	return (y2 - y1)*(x3 - x1) - (x2 - x1)*(y3 - y1);
}
double RayTracingCPU::MyminCPU(double x1, double x2) {
	if (x1 < x2)
		return x1;
	else
		return x2;
}
double RayTracingCPU::MymaxCPU(double x1, double x2) {
	return x1 > x2 ? x1 : x2;
}
double RayTracingCPU::calDistanceCPU(double x1, double y1, double x2, double y2) {
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}
bool RayTracingCPU::onSegmentCPU(double x1, double y1, double x2, double y2, double x3, double y3) {
	if (MyminCPU(x1, x2) <= x3 && x3 < MymaxCPU(x1, x2) && MyminCPU(y1, y2) < y3&&y3 < MymaxCPU(y1, y2)) {
		return true;
	}
	else {
		return false;
	}
}
void RayTracingCPU::calculPointOfIntersectionCPU(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double &x, double &y) {
	double a = y2 - y1;
	double b = x1 - x2;
	double c = y4 - y3;
	double d = x3 - x4;
	double e = (y2 - y1)*x1 - (x2 - x1)*y1;
	double f = (y4 - y3)*x3 - (x4 - x3)*y3;
	x = (e*d - b * f) / (a*d - b * c);
	y = (a*f - e * c) / (a*d - b * c);
}
bool RayTracingCPU::segmentsIntersectCPU(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
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

void RayTracingCPU::normalizeVectorCPU(double &x, double &y) {
	double norm = sqrt(x*x + y * y);
	x = x / norm;
	y = y / norm;
}
double RayTracingCPU::calculDistancePointAndLineCPU(double x1, double y1, double x2, double y2, double x0, double y0) {
	double A = y2 - y1;
	double B = x1 - x2;
	double C = x2 * y1 - x1 * y2;
	return abs(A*x0 + B * y0 + C) / sqrt(A*A + B * B);
}
bool RayTracingCPU::isEndPointCPU(Edge edge, double x, double y) {
	double d1 = calDistanceCPU(edge.xend, edge.yend, x, y);
	double d2 = calDistanceCPU(edge.xstart, edge.ystart, x, y);
	if (d1 < IS_DIFFRACTION_RADIUS || d2 < IS_DIFFRACTION_RADIUS) {
		return true;
	}
	else {
		return false;
	}
}
void RayTracingCPU::judegTouchedDiffractionCPU(MyVector *vector,Point *point, int currentPointsIndex) {
	//int i = blockDim.x * blockIdx.x + threadIdx.x;
	for (int i = 0; i < N;i++) {
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
			if (dev_grids->grids[indexC].isContains || (oldRow == point->row&&oldCol == point->col)) {
				//contains edge
				double distanceM = std::sqrt(2.0)*LENGTH + 1;
				bool isFind = false;
				double insertPointOutX = 0, insertPointOutY = 0;
				int edgeIndex = -1;

				for (int k = 0; k < dev_grids->grids[indexC].N; k++) {
					if (segmentsIntersectCPU(x, y, newX, newY, dev_grids->grids[indexC].edges[k].xstart, dev_grids->grids[indexC].edges[k].ystart, dev_grids->grids[indexC].edges[k].xend, dev_grids->grids[indexC].edges[k].yend) == true) {
						double insertPointX = 0, insertPointY = 0;//��ǰ����
						calculPointOfIntersectionCPU(x, y, newX, newY, dev_grids->grids[indexC].edges[k].xstart, dev_grids->grids[indexC].edges[k].ystart, dev_grids->grids[indexC].edges[k].xend, dev_grids->grids[indexC].edges[k].yend, insertPointX, insertPointY);
						if (currentX > insertPointX || insertPointX > currentX + LENGTH || currentY > insertPointY || insertPointY > currentY + LENGTH) {
							//������������������
							continue;
						}
						double tempDistance = calDistanceCPU(x, y, insertPointX, insertPointY);
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
						double targetDistance = calculDistancePointAndLineCPU(x, y, newX, newY, point->x, point->y);
						//ֻ���ڲ���Բ�뾶���Ҿ����������������н���
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
					double edgeVectorX = dev_grids->grids[indexC].edges[edgeIndex].vectorX;
					double edgeVectorY = dev_grids->grids[indexC].edges[edgeIndex].vectorY;

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
						normalizeVectorCPU(vectorX, vectorY);//�����Ժ���Ҫ��һ��
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
*/
void RayTracingCPU::judgeIsTouchedCPU(Point *point,int currentPointsIndex) {
	//int i = blockDim.x * blockIdx.x + threadIdx.x;
	for (int i = 0; i < N;i++) {
		int row = myVector[i].row;
		int col = myVector[i].col;
		int oldRow = row;
		int oldCol = col;
		double x = myVector[i].x;
		double y = myVector[i].y;
		double vectorX = myVector[i].vectorX;
		double vectorY = myVector[i].vectorY;
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
			if (dev_grids->grids[indexC].isContains || (oldRow == point->row&&oldCol == point->col)) {
				//contains edge
				double distanceM = std::sqrt(2.0)*LENGTH + 1;
				bool isFind = false;
				double insertPointOutX = 0, insertPointOutY = 0;
				int edgeIndex = -1;

				for (int k = 0; k < dev_grids->grids[indexC].N; k++) {
					if (segmentsIntersectCPU(x, y, newX, newY, dev_grids->grids[indexC].edges[k].xstart, dev_grids->grids[indexC].edges[k].ystart, dev_grids->grids[indexC].edges[k].xend, dev_grids->grids[indexC].edges[k].yend) == true) {
						double insertPointX = 0, insertPointY = 0;//��ǰ����
						calculPointOfIntersectionCPU(x, y, newX, newY, dev_grids->grids[indexC].edges[k].xstart, dev_grids->grids[indexC].edges[k].ystart, dev_grids->grids[indexC].edges[k].xend, dev_grids->grids[indexC].edges[k].yend, insertPointX, insertPointY);
						if (currentX > insertPointX || insertPointX > currentX + LENGTH || currentY > insertPointY || insertPointY > currentY + LENGTH) {
							//������������������
							continue;
						}
						double tempDistance = calDistanceCPU(x, y, insertPointX, insertPointY);
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
						double targetDistance = calculDistancePointAndLineCPU(x, y, newX, newY, point->x, point->y);
						//ֻ���ڲ���Բ�뾶���Ҿ����������������н���
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
					if (isEndPointCPU(dev_grids->grids[indexC].edges[edgeIndex], insertPointOutX, insertPointOutY)) {
						if (calDistanceCPU(dev_grids->grids[indexC].edges[edgeIndex].xend, dev_grids->grids[indexC].edges[edgeIndex].yend, insertPointOutX, insertPointOutY) < calDistanceCPU(dev_grids->grids[indexC].edges[edgeIndex].xstart, dev_grids->grids[indexC].edges[edgeIndex].ystart, insertPointOutX, insertPointOutY)) {
							insertPointOutX = dev_grids->grids[indexC].edges[edgeIndex].xend;
							insertPointOutY = dev_grids->grids[indexC].edges[edgeIndex].yend;
						}
						else {
							insertPointOutX = dev_grids->grids[indexC].edges[edgeIndex].xstart;
							insertPointOutY = dev_grids->grids[indexC].edges[edgeIndex].ystart;
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
						judegTouchedDiffractionCPU(myVectorNew, point, i);
						free(myVectorNew);
						//__syncthreads();
						//judegTouchedDiffraction << <blocksPerGrid, threadsPerBlock >> > (myVectorNew, grids, diffractionPoints, point, N, i);
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
						double edgeVectorX = dev_grids->grids[indexC].edges[edgeIndex].vectorX;
						double edgeVectorY = dev_grids->grids[indexC].edges[edgeIndex].vectorY;

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
							normalizeVectorCPU(vectorX, vectorY);//�����Ժ���Ҫ��һ��
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
vector<vector<double>> RayTracingCPU::initCUDAInput(double RX_X, double RX_Y) {
	//��Ҫ��ʼ������
	//grids->�洢����
	//������������Ϊ50*50��С������ɣ�������������С����Щ��������ȫ����Ϊ��
	//int N = 180;
	//printf("Start...%d\n", sizeof(bool));
	clock_t start, finish;
	start = clock();
	//struct Grids *cuda_grids;

	//MyVector *myVector = (struct MyVector *)malloc(sizeof(struct MyVector) * N);
	//Points *points = (struct Points *)malloc(sizeof(struct Points) * N);
	Point *Rx = (struct Point *)malloc(sizeof(struct Point));
	//Points *diffractionPoints = (struct Points *)malloc(sizeof(struct Points) * N*N);

	Rx->x = RX_X;
	Rx->y = RX_Y;
	Rx->row = int(RX_Y / LENGTH);
	Rx->col = int(RX_X / LENGTH);

	//for (int i = 0; i < N; i++) {
	//	//myVector[i]= (struct MyVector *)malloc(sizeof(struct MyVector));
	//	myVector[i].x = TX_X;
	//	myVector[i].y = TX_Y;
	//	double angle = 2 * PIs / N * i;
	//	myVector[i].vectorX = cos(angle);
	//	myVector[i].vectorY = sin(angle);
	//	myVector[i].col = int(TX_X / 2);
	//	myVector[i].row = int(TX_Y / 2);
	//	points[i].N = 0;
	//	points[i].isFind = false;
	//	for (int j = 0; j < N; j++) {
	//		diffractionPoints[i*N + j].N = 0;
	//		diffractionPoints[i*N + j].isFind = false;
	//	}
	//}



	judgeIsTouchedCPU(Rx,0);
	finish = clock();
	spendTime = (double)(finish - start) / CLOCKS_PER_SEC;
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
		else if (points[i].N > 0) {
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
	//	for (int m = 0; m < N; m++) {
	//		//qDebug() << "come to here"+ diffractionPoints[i*N + m].N;
	//		if (diffractionPoints[i*N + m].N>0) {
	//			vector<double> diffractionPo;
	//			for (int n = 0; n < diffractionPoints[i*N + m].N; n++) {
	//				diffractionPo.push_back(diffractionPoints[i*N + m].point[n].x);
	//				diffractionPo.push_back(diffractionPoints[i*N + m].point[n].y);
	//			}
	//			getResult.push_back(diffractionPo);
	//		}
	//	}
	//}
	//finish = clock();
	//double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//printf("%f seconds\n", duration);
	//��Ҫ����point
	free(Rx);
	return getResult;
}	
RayTracingCPU::RayTracingCPU() {

}
RayTracingCPU::~RayTracingCPU() {
	free(myVector);
	free(points);
	free(diffractionPoints);
	free(dev_grids);
}
void RayTracingCPU::init() {
	points = (struct Points *)malloc(sizeof(struct Points) * N);
	diffractionPoints= (struct Points *)malloc(sizeof(struct Points) * N*N);
	dev_grids= (struct Grids *)malloc(sizeof(struct Grids));
	for (int i = 0; i < N; i++) {
		points[i].N = 0;
		points[i].isFind = false;
		for (int j = 0; j < N; j++) {
			diffractionPoints[i*N + j].N = 0;
			diffractionPoints[i*N + j].isFind = false;
		}
	}
}
void RayTracingCPU::updateTx(double x, double y) {
	if (txUpdateFlag) {
		//cudaFree(cuda_myVector);
	}
	else {
		txUpdateFlag = true;
		myVector= (struct MyVector *)malloc(sizeof(struct MyVector) * N);
		//cudaMallocManaged((void**)&myVector, sizeof(struct MyVector) * N);
		//myVector = (struct MyVector *)malloc(sizeof(struct MyVector) * N);
	}

	for (int i = 0; i < N; i++) {
		//myVector[i]= (struct MyVector *)malloc(sizeof(struct MyVector));
		myVector[i].x = x;
		myVector[i].y = y;
		double angle = 2 * PIs / N * i;
		myVector[i].vectorX = cos(angle);
		myVector[i].vectorY = sin(angle);
		myVector[i].col = int(x / 2);
		myVector[i].row = int(y / 2);
	}
}
void RayTracingCPU::updateRx(double x, double y) {
	rxUpdateFlag = true;
}