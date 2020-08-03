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

int horizontal_window = 5;
int slope_window = 4;

int main(int argc, char **argv)
{
	first_frame(&temika_frame); //Get the first frame from the movie
	double *im_array = (double*) malloc(sizeof(double)*temika_frame.size_x*temika_frame.size_y); //Allocate the array
	image_array(&temika_frame, im_array); //Transform the temika array into an image array

	//Get profile
	ct::Vector center(112, 112);
	
	//ct::axes axis = ct::x;
	//ct::Vector position_vector(150, 115);
	//ct::axes axis = ct::x;
	//ct::Vector position_vector(75, 115);
	
	//ct::axes axis = ct::y;
	//ct::Vector position_vector(113, 78);
	
//	ct::axes axis = ct::v;
//	ct::Vector position_vector(86, 86);

	ct::axes axis = ct::w;
	ct::Vector position_vector(136, 85);

	double bar_point_v_slope[3];

	ct::max_slope(bar_point_v_slope, im_array, temika_frame.size_x, temika_frame.size_y, position_vector, axis, center, horizontal_window, slope_window);
	return 1;
}
