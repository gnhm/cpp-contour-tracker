//g++ $(pkg-config --cflags --libs opencv) -std=c++11 contour_tracker.cpp -o contour_tracker

#include <iostream>
#include <stdlib.h>
#include "../../running_slope.h"

int main(int argc, char **argv)
{
	int window = atoi(argv[1]);
	int orientation = atoi(argv[2]);
	int n = (argc - 3)/2;
	double x[n];
	double y[n];
	for (int i = 0; i < n; i++)
	{
		x[i] = atof(argv[i + 3]);
		y[i] = atof(argv[i + 3 + n]);
	}

	double ml[n-window];
	double bl[n-window];

	double slope_intercept[2];
//	get_maximum_slope_and_running(slope_intercept, ml, bl, x, y, n, window, orientation);
	get_maximum_slope(slope_intercept, x, y, n, window, orientation);

	for (int i = 0; i < 2; i++)
	{
		printf("%.9f\t", slope_intercept[i]);
	}	
	printf("\n");
	/*
	for (int i = 0; i < n-window; i++)
	{
		printf("%.9f\t", ml[i]);
	}	
	printf("\n");
	for (int i = 0; i < n-window; i++)
	{
		printf("%.9f\t", bl[i]);
	}	
	*/

	return 0;
}
