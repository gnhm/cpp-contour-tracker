//g++ $(pkg-config --cflags --libs opencv) -std=c++11 contour_tracker.cpp -o contour_tracker

#include "temika_header.h"
#include "contour_tracker_lib.h"
#include <iostream>
#include "get_movie_frame.h"

#include <assert.h>

#define VEC_X 0;
#define VEC_Y 1;
#define VEC_V 2;
#define VEC_W 3;

using namespace std;

camera_frame_struct temika_frame;

//int horizontal_window = 5;
//int slope_window = 4;

int horizontal_window = 20;
int slope_window = 5;

int main(int argc, char **argv)
{

	//double *im_array = (double*) malloc(sizeof(double)*temika_frame.size_x*temika_frame.size_y); //Allocate the array
	//first_frame(&temika_frame); //Get the first frame from the movie
	//image_array(&temika_frame, im_array); //Transform the temika array into an image array

        int n = 500;
        double R = 100;
        double s = 10;

	double *im_array = (double*) malloc(sizeof(double)*n*n); //Allocate the array
	fake_circle(im_array, n, R, s); //Creates a fake circular image, for trouble shooting

	//Get profile
	//ct::Vector center(112, 112);
	
	//ct::axes axis = ct::x;
	//ct::Vector position_vector(150, 115);
	//ct::axes axis = ct::x;
	//ct::Vector position_vector(75, 115);
	
	//ct::axes axis = ct::y;
	//ct::Vector position_vector(113, 78);
	
//	ct::axes axis = ct::v;
//	ct::Vector position_vector(86, 86);

//	ct::axes axis = ct::w;
//	ct::Vector position_vector(136, 85);

//	ct::max_slope(bar_point_v_slope, im_array, n, n, position_vector, axis, center, horizontal_window, slope_window);
//	ct::max_slope(bar_point_v_slope, im_array, temika_frame.size_x, temika_frame.size_y, position_vector, axis, center, horizontal_window, slope_window);

	ct::Vector center(n/2, n/2);
//	ct::axes axis = ct::x;
	double angle = 0.00;
	ct::Vector position_vector((0.5*n) + R*cos(angle), (0.5*n) + R*sin(angle));
//	ct::Vector position_vector(n/2, n/2 + R);


	int max = 2000;
	double bar_point_v_slope[3];
	double *contour = (double*) malloc(2*max*sizeof(double));
	contour[0] = position_vector.x;
	contour[1] = position_vector.y;
	int contour_i = 0;
	int chirality = 1;


	for (int i = 1; i < max; i++)
	{
		ct::next_point(contour + 2*i, im_array, n, n, contour, i, center, horizontal_window, slope_window, chirality);
	}
	for (int i = 0; i < max; i++)
	{
		printf("%f\t%f\n", contour[2*i], contour[2*i + 1]);
	}
		return 1;
}
