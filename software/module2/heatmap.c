/*
 * heatmap.c
 *
 * Functions for drawing the heatmap of GPS points.
 */

#include "heatmap.h"
#include "graphics.h"
#include <math.h>

int colours[HEATMAP_H][HEATMAP_V];

void gen_heatmap(GPSPoint points[], int numPoints, Colours colourScheme){
	//Initialize 2D array representing points
	int count[HEATMAP_H][HEATMAP_V] = {{0}};

	//Check where points land
	int i;
	for (i = 0; i < numPoints; i++) {
		int xi, yi;
		for (yi = 0; yi < HEATMAP_V; yi++) {
			for (xi = 0; xi < HEATMAP_H; xi++) {
				if (points[i].x < ((xi+1) * XRES/HEATMAP_H) && points[i].x >= (xi * XRES/HEATMAP_H) &&
				 	points[i].y < ((yi+1) * MENU_TOP/HEATMAP_V) && points[i].y >= (yi * MENU_TOP/HEATMAP_V)) {
					count[xi][yi]++;
					break;
				}
			}
		}
	}

	//Initialize interpret submenu with count
	initInterpret(count, numPoints);

	//Find max and min counts
	int x, y;
	int max_count = 0;
	int min_count = numPoints;
	for (y = 0; y < HEATMAP_V; y++) {
		for (x = 0; x < HEATMAP_H; x++) {
			if (count[x][y] > max_count) {
				max_count = count[x][y];
			}
			if (count[x][y] < min_count) {
				min_count = count[x][y];
			}
		}
	}

	//Make colours proportional to number of points
	int shade;
	for (y = 0; y < HEATMAP_V; y++) {
		for (x = 0; x < HEATMAP_H; x++) {
			shade = ceil(((count[x][y] - min_count) * (HM_SHADES - 1))/(double)(max_count - min_count));
			colours[x][y] = colourScheme.shades[shade];
		}
	}
}

void draw_heatmap()
{
	//Draw points
	int h, v;
	for (v = 0; v < HEATMAP_V; v++) {
		for (h = 0; h < HEATMAP_H; h++) {
			WriteFilledRectangle(h * (XRES-1)/HEATMAP_H, v * (MENU_TOP-1)/HEATMAP_V, (h + 1) * (XRES-1)/HEATMAP_H, (v + 1) * (MENU_TOP-1)/HEATMAP_V, colours[h][v]);
		}
	}
}

void connect_points(GPSPoint points[], int numPoints, int connectTheDotsLine)
{
	GPSPoint point_a;
	GPSPoint point_b;
	if (numPoints > 0) {
		WriteCircle((int)points[0].x, (int)points[0].y, POINT_CIRCLE_RADIUS, connectTheDotsLine);
	}
	int i;
	for(i = 1; i < numPoints; i++) {
		point_a = points[i-1];
		point_b = points[i];

		// draw circle around each point for testing
		WriteCircle((int)point_b.x, (int)point_b.y, POINT_CIRCLE_RADIUS, connectTheDotsLine);

		// draw a line from point_a to point_b
		WriteLine((int)point_a.x, (int)point_a.y, (int)point_b.x, (int)point_b.y, connectTheDotsLine);
	}
}

void connect_points_all_sets(localDataSets sets, Colours colourScheme){
	int i, color;
	for(i = 0, color = 0; i < MAX_N_SETS; i++){
		if(sets.headTimeQueue == i){
			continue;
		}else{
			while(color == colourScheme.connectTheDotsLine || color == colourScheme.menuBackground){
				color++;
			}

			connect_points(sets.dataSets[i].points, sets.dataSets[i].size, color);
			color++;
		}
	}

	connect_points(sets.dataSets[sets.headTimeQueue].points, sets.dataSets[sets.headTimeQueue].size, colourScheme.connectTheDotsLine);
}
