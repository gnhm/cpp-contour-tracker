#include"../../temika_header.h"
#include"../../get_movie_frame.h"
#include<string.h>
#include"../../contour_analyzer_lib.h"

#define SAMPLE 10

#define I_MAX 60000

int main(int argc, char **argv)
{

	char *moviefile = argv[1];

	struct camera_frame_struct frame; //Temika frame struct
	long offset = 0; //Keep track of movie position
	int i = 0; //Frames opened so far
	int bad_frames = 0; //Frames which were untracked

	//Try opening the file
	FILE *file;
	if ( !( file = fopen(moviefile, "rb" ) ) )
	{
		printf( "Couldn't open movie file.\n" );
		exit( EXIT_FAILURE );
	}
	fseek(file, 0, SEEK_SET);

	get_next_frame(file, &frame);
	double *im_array = (double*) malloc(sizeof(double)*frame.size_x*frame.size_y);
	fseek(file, 0, SEEK_SET);

	while((get_next_frame(file, &frame) != -1) && i < I_MAX)
	{
		/*
		image_array(&frame, im_array);
		for (int k = 0; k < frame.size_y; k++)
		{
			for (int j = 0; j < frame.size_x; j++)
			{
				printf("%f\t", im_array[j + k*frame.size_x]);
			}
			printf("\n");
		}
		printf("\n");
		*/
		i++;
	}
	fclose(file);

	return 0;
}
