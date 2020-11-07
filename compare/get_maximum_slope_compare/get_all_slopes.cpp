//g++ $(pkg-config --cflags --libs opencv) -std=c++11 contour_tracker.cpp -o contour_tracker

//#include "../../temika_header.h"
//#include "../../contour_tracker_lib.h"
#include <iostream>
//#include "../../get_movie_frame.h"
//#include "contour_analyzer_lib.h"
#include <stdlib.h>
#include "../../running_slope.h"

//using namespace std;

//get_maximum_slope(double *slope_intercept, double *x, double *y, int n, int window, int orientation)
//get_maximum_slope(slope_intercept, full_profile, full_profile + 2*coordinates_width, coordinates_width, slope_window, orientation);

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
	get_maximum_slope(slope_intercept, x, y, n, window, orientation);
	running_slope(ml, bl, x, y, n, window);
//	printf("%f\t", slope_intercept[0]);
//	printf("%f\t", slope_intercept[1]);
//	printf("\n");

	for (int i = 0; i < n - window; i++)
	{
		//printf("%f\t", slope_intercept[i]);
		printf("%f\t", ml[i]);
		//printf("%f\t", x[i]);
	}	
	printf("\n");
	for (int i = 0; i < n - window; i++)
	{
		//printf("%f\t", slope_intercept[i]);
		printf("%f\t", bl[i]);
		//printf("%f\t", x[i]);
	}	

	return 0;
}
