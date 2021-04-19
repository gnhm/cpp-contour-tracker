//g++ $(pkg-config --cflags --libs opencv) -std=c++11 contour_tracker.cpp -o contour_tracker

#include "../../temika_header.h"
#include "../../contour_tracker_lib.h"
//#include <iostream>
#include "../../get_movie_frame.h"
//#include "contour_analyzer_lib.h"

//#include <assert.h>

#define VEC_X 0;
#define VEC_Y 1;
#define VEC_V 2;
#define VEC_W 3;

using namespace std;


camera_frame_struct temika_frame;

int main(int argc, char **argv)
{
	long offset = 0;
	first_frame(argv[1], &temika_frame, offset); //Get the first frame from the movie
	int horizontal_window = atoi(argv[2]);
	ct::Vector c((double) atof(argv[3]), (double) atof(argv[4]));
	ct::axes axis = (ct::axes) atoi(argv[5]);
	int slope_window = atoi(argv[6]);
	ct::Vector center((double) 112., (double) 112.);


	double *im_array = (double*) malloc(sizeof(double)*temika_frame.size_x*temika_frame.size_y); //Allocate the array
	image_array(&temika_frame, im_array); //Transform the temika array into an image array

	double bar_point_v_slope[3];
	
        ct::max_slope(bar_point_v_slope, im_array, temika_frame.size_x, temika_frame.size_y, c, axis, center, horizontal_window, slope_window);
	//ct::profile(im_array, temika_frame.size_x, temika_frame.size_y, full_profile, c, axis, horizontal_window);

	for (int i = 0; i < 3; i++)
	{
		printf("%f\t", bar_point_v_slope[i]);
	}	

	return 0;
}
