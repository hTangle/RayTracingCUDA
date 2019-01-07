#pragma once
#include <StaticConstants.h>
#include <vector>
#include <QString>
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
	bool isContains;
	struct Edge edges[MAX_EDGE_GRID];
};

struct Grids
{
	int height;
	int width;
	struct Grid grids[ROW*COL];
};

vector<vector<double>> initCUDAInput(Grids *grids, double TX_X, double TX_Y, double RX_X, double RX_Y, int N);
vector<vector<double>> initCUDAInputCPU(Grids *grids, double TX_X, double TX_Y, double RX_X, double RX_Y, int N);
vector<QString> getCUDAInformation();
//vector<vector<double>> initCUDAInput(vector<vector<vector<double>>> grid_input, double TX_X, double TX_Y, double RX_X, double RX_Y, int N);
