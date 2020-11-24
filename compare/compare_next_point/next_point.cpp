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

	int next_point_px[2];
	double next_point_fine[2];
	int contour_i = 1;
	int contour[2];
	contour[0] = (int) first_point.x;
	contour[1] = (int) first_point.y;

	next_point(next_point_px, next_point_fine, im_array, frame.size_x, frame.size_y, contour, contour_i, center, horizontal_window, slope_window, chirality);
	printf("%d\t%d\n", contour[0], contour[1]);
	printf("%f\t%f\n", next_point_fine[0], next_point_fine[1]);
	printf("%d\t%d\n", next_point_px[0], next_point_px[1]);

	free(im_array);
	return 0;
}
