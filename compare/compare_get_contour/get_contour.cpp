//g++ $(pkg-config --cflags --libs opencv) -std=c++11 contour_tracker.cpp -o contour_tracker

#include <iostream>
#include <stdlib.h>
//#include "../../running_slope.h"
#include "../../temika_header.h"
#include "../../contour_tracker_lib.h"
#include "../../get_movie_frame.h"

int main(int argc, char **argv)
{
        FILE *file;
        if ( !( file = fopen(argv[1], "rb" ) ) )
        {
                printf( "Couldn't open movie file.\n" );
                exit( EXIT_FAILURE );
        }

	camera_frame_struct frame;
	get_frame(file, &frame, 0);
	double *im_array = (double*) malloc(sizeof(double)*frame.size_x*frame.size_y);
	image_array(&frame, im_array);

        ct::Vector first_point((double) atof(argv[2]), (double) atof(argv[3]));
        ct::Vector center((double) atof(argv[4]), (double) atof(argv[5]));
	int horizontal_window = atoi(argv[6]);
	int slope_window = atoi(argv[7]);
	int chirality = atoi(argv[8]);
	int n = atoi(argv[9]);

	ct::ContourStruct ct_st;
        ct_st.max = 1000;
        ct_st.burn = 20;
        ct_st.chirality = chirality;
        ct_st.contour_fine = (double*) malloc(2*ct_st.max*sizeof(double));
        ct_st.contour_px = (int*) malloc(2*ct_st.max*sizeof(int));
	ct_st.im_array = im_array;
        ct_st.position_vector = &first_point;
        ct_st.center = &center;
        ct_st.rows = frame.size_y;
        ct_st.cols = frame.size_x;
	ct_st.horizontal_window = horizontal_window;
        ct_st.slope_window = slope_window;
        ct_st.done = 0;
        ct_st.start = 0;
        ct_st.max_i = 0;

	ct_st.contour_px[0] = (int) first_point.x;
	ct_st.contour_px[1] = (int) first_point.y;
	ct_st.contour_fine[0] = first_point.x;
	ct_st.contour_fine[1] = first_point.y;

	//ct::get_contour(&ct_st);
	//next_point(next_point_px, next_point_fine, im_array, frame.size_x, frame.size_y, contour, contour_i, center, horizontal_window, slope_window, chirality);
	printf("%f\t%f\n", ct_st.contour_fine[0], ct_st.contour_fine[1]); 
	printf("%d\t%d\n", ct_st.contour_px[0], ct_st.contour_px[1]); 
	for (int i = 1; i<n; i++)
	{
		next_point(ct_st.contour_px + 2*i, ct_st.contour_fine + 2*i, ct_st.im_array, ct_st.rows, ct_st.cols, ct_st.contour_px, i, *(ct_st.center), ct_st.horizontal_window, ct_st.slope_window, ct_st.chirality);
		printf("%f\t%f\n", ct_st.contour_fine[2*i], ct_st.contour_fine[2*i + 1]); 
		printf("%d\t%d\n", ct_st.contour_px[2*i], ct_st.contour_px[2*i + 1]); 
	}

	/*
	printf("%d\t%d\n", contour[0], contour[1]);
	printf("%f\t%f\n", next_point_fine[0], next_point_fine[1]);
	printf("%d\t%d\n", next_point_px[0], next_point_px[1]);
	*/

	return 0;
}
