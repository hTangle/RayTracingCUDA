#include "RayTracing.h"



RayTracing::RayTracing()
{
}


RayTracing::~RayTracing()
{
}
void RayTracing::clearGrids() {
	for (int i = 0; i < ROW*COL; i++) {
		dev_grids->grids[i].N = 0;
		dev_grids->grids[i].isContains = false;
	}
}
bool RayTracing::updateEdges(int index, double x0, double y0, double x1, double y1, double vx, double vy) {
	if (dev_grids->grids[index].N >= MAX_EDGE_GRID) {
		return false;
	}
	dev_grids->grids[index].edges[dev_grids->grids[index].N].xstart = x0;
	dev_grids->grids[index].edges[dev_grids->grids[index].N].ystart = y0;
	dev_grids->grids[index].edges[dev_grids->grids[index].N].xend = x1;
	dev_grids->grids[index].edges[dev_grids->grids[index].N].yend = y1;
	dev_grids->grids[index].edges[dev_grids->grids[index].N].vectorX = vx;
	dev_grids->grids[index].edges[dev_grids->grids[index].N].vectorY = vy;
	dev_grids->grids[index].N++;
	dev_grids->grids[index].isContains = true;
	return true;
}