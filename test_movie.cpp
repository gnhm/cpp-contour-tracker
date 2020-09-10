//g++ -std=c++11 test_movie.cpp -o test_movie

#include"temika_header.h"
#include"get_movie_frame.h"
#include<string.h>
#include"contour_analyzer_lib.h"
#include"contour_tracker_lib.h"

#define SAMPLE 1

int track_contour(char *moviefile)
{
	struct camera_frame_struct frame;
	long offset = 0;
	int i = 0;
	double *im_array;
	char contour_filename[100];
	char contour_filename_new[100];
	char *movie_ext = ".movie";

	double new_center[2];
	double new_position[2];

	struct Contour cs;

	struct ct::ContourStruct ct_st;
	ct_st.max = 1000;
	ct_st.burn = 20;
	ct_st.chirality = 1;
	ct_st.contour_fine = (double*) malloc(2*ct_st.max*sizeof(double));
	ct_st.contour_px = (int*) malloc(2*ct_st.max*sizeof(int));

	ct::Vector position_vector(-1,-1);
	ct::Vector center(-1, -1);
	ct_st.position_vector = &position_vector;
	ct_st.center = &center;

	int horizontal_window = 20;
	int slope_window = 5;

	int old_max_i;
	double old_contour[2*SAMPLE];

	strcpy(contour_filename, moviefile);
	char *p = strstr(contour_filename, movie_ext);
	*p = '\0';
	strcat(contour_filename, "_contour.txt");

	strcpy(contour_filename_new, moviefile);
	char *p_new = strstr(contour_filename_new, movie_ext);
	*p_new = '\0';
	strcat(contour_filename_new, "_contour_full.txt");

	int bad_frames = 0;

	int characters = 0;

	FILE *file;
	if ( !( file = fopen(moviefile, "rb" ) ) )
	{
		printf( "Couldn't open movie file.\n" );
		exit( EXIT_FAILURE );
	}

	while((offset = get_frame(file, &frame, offset)) != -1)
	{
		if (i == 0)
		{
			//allocate the image array based on first frame
			im_array = (double*) malloc(sizeof(double)*frame.size_x*frame.size_y);

			//load the first contour
			load_contour(contour_filename, &cs);
			for (int j = 0; j < SAMPLE; j++)
			{
				old_contour[2*j] = cs.contour[2*j*cs.max_i/SAMPLE];
				old_contour[2*j + 1] = cs.contour[2*j*cs.max_i/SAMPLE + 1];
			}
			contour_center(&cs, new_center);
			ct_st.center->x = new_center[0];
			ct_st.center->y = new_center[1];
		}

		image_array(&frame, im_array);

		ct_st.im_array = im_array;
		ct_st.rows = frame.size_x;
		ct_st.cols = frame.size_y;
		ct_st.horizontal_window = horizontal_window;
		ct_st.slope_window = slope_window;

		//No way around it. Need to save old contour points for sampling elsewhere.
		for (int j = 0; j < SAMPLE; j++)
		{
			ct_st.position_vector->x = old_contour[2*j]; 
			ct_st.position_vector->y = old_contour[2*j + 1]; 
			ct_st.contour_px[0] = (int) ct_st.position_vector->x;
			ct_st.contour_px[1] = (int) ct_st.position_vector->y;
			ct_st.contour_fine[0] = (double) ct_st.contour_px[0];
			ct_st.contour_fine[1] = (double) ct_st.contour_px[1];
			ct::get_contour(&ct_st);
			if (ct_st.max_i != -1)
			{
				break;
			}
		}

		ct_st.position_vector->x = old_contour[0]; 
		ct_st.position_vector->y = old_contour[1]; 
		ct_st.contour_px[0] = (int) ct_st.position_vector->x;
		ct_st.contour_px[1] = (int) ct_st.position_vector->y;
		ct_st.contour_fine[0] = (double) ct_st.contour_px[0];
		ct_st.contour_fine[1] = (double) ct_st.contour_px[1];
		ct_st.done = -1;
		ct_st.start = -1;
		ct::get_contour(&ct_st);

		if (ct_st.max_i == -1)
		{
//				printf("Bad frame\n");
				bad_frames++;
		}
		else
		{
			for (int j = 0; j < SAMPLE; j++)
			{
				old_contour[2*j] = ct_st.contour_fine[2*j*ct_st.max_i/SAMPLE];
				old_contour[2*j + 1] = ct_st.contour_fine[2*j*ct_st.max_i/SAMPLE + 1];
			}
//				printf("max_i = %d\n", ct_st.max_i);
		}
		save_contour(contour_filename_new, ct_st);

		if (i == 0)
		{
			printf("i = %d", i);
		}
		else if (i % 10 == 0)
		{
			for (int cc = 0; cc < characters; cc++)
			{
				printf("\b");
			}

			printf("%d", i);
		}
		fflush(stdout);
		characters = (int) ((i == 0) ? 1 : (log10(i) + 1));

		i++;
	}

	for (int cc = 0; cc < characters + 4; cc++)
	{
		printf("\b");
	}
	printf("Total frames = %d\n", i);
	printf("Frames dropped = %d\n", bad_frames);
	printf("\n");
	free(ct_st.contour_fine);
	free(ct_st.contour_px);
	free(ct_st.im_array);

	return 0;
}

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		printf("Tracking %s\n", argv[i]);
		track_contour(argv[i]);
	}

	return 0;
}
