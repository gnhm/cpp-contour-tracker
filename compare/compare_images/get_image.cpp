#include <iostream>
#include <stdlib.h>
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
	for (int i = 0; i < frame.size_y; i++)
	{
		for (int j = 0; j < frame.size_x; j++)
		{
			printf("%f\t", im_array[i*frame.size_x + j]);
		}
		printf("\n");
	}
	free(im_array);
	return 0;
}
