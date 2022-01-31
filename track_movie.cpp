//g++ -std=c++11 test_movie.cpp -o test_movie

#include "temika_header.h"
#include "get_movie_frame.h"
#include <string.h>
#include "contour_tracker-trackmovie_lib.h"
#include "contour_analyzer-trackmovie_lib.h"

#define SAMPLE 10
#define I_MAX 10000
#define VERBOSE 1

int main(int argc, char **argv)
{
	char *moviefile = argv[1];

	if(VERBOSE)
	{
		printf("Tracking %s\n", moviefile);
	}

	struct camera_frame_struct frame; //Temika frame struct - maybe worth looking into what this does (basically a way of reading movies?)
	long offset = 0; //Keep track of movie position
	int i = 0; //Frames opened so far
	int bad_frames = 0; //Frames which were untracked

	const char *movie_ext = ".movie";

	//Makes the seed contour_filename based on movie filename
	char contour_filename[1000];
	strcpy(contour_filename, moviefile);
	char *p = strstr(contour_filename, movie_ext);
	*p = '\0';
	strcat(contour_filename, "_contour.txt");

	//Makes the full contour_filename based on movie filename
	char contour_filename_new[1000];
	strcpy(contour_filename_new, moviefile);
	char *p_new = strstr(contour_filename_new, movie_ext);
	*p_new = '\0';
	strcat(contour_filename_new, "_contour_full.txt");

	//Initialize the contour tracking struct, from the struct definition in tracker-trackmovie_lib.h
	struct ct::ContourStruct ct_st;
	ct_st.max = 1000;
	ct_st.burn = 10;
	ct_st.chirality = 1;
	ct_st.contour_fine = (double*) malloc(2*ct_st.max*sizeof(double));
	ct_st.contour_px = (int*) malloc(2*ct_st.max*sizeof(int));
	ct::Vector position_vector(-1,-1);
	ct::Vector center(-1, -1);
	ct_st.position_vector = &position_vector;
	ct_st.center = &center;
	ct_st.horizontal_window = 5;
	ct_st.slope_window = 4;

	//This, I hope, can be gotten rid off - may rely on the first contour already being fed in
	double old_contour[2*SAMPLE];

	double new_center[2];
	double new_position[2];
	struct Contour cs; // from the struct defined in analyzer-trackmovie_lib.h

	//For fancy printing
	int characters = 0;

	//Try opening the file
	FILE *file;
	if ( !( file = fopen(moviefile, "rb" ) ) )
	{
		printf( "Couldn't open movie file.\n" );
		exit( EXIT_FAILURE );
	}
	
	// proceed if the files open successfully
        fseek(file, 0, SEEK_SET);
	
	
	while(((offset = get_next_frame(file, &frame)) != -1) && i < I_MAX) // the get_next_frame takes some apparently undefined struct as it second arg;
		// work out what frame means here (if it isn't obvious)
	{
		if (i == 0) // this is the important section, see equivalent on track_contours
		{
			//allocate the image array based on first frame. This will break if frames can change (easy fix, but it never happens)
			ct_st.rows = frame.size_x;
			ct_st.cols = frame.size_y;
			ct_st.im_array = (double*) malloc(sizeof(double)*frame.size_x*frame.size_y);

			//load the seed contour (not all. just a subset of size SAMPLE). Write to old_contour (why?). Get the contour_center.

			load_contour(contour_filename, &cs); // working on this atm
			for (int j = 0; j < SAMPLE; j++)
			{
				old_contour[2*j] = cs.contour[2*j*cs.max_i/SAMPLE];
				old_contour[2*j + 1] = cs.contour[2*j*cs.max_i/SAMPLE + 1];
			}
			contour_center(&cs, new_center);
			ct_st.center->x = new_center[0];
			ct_st.center->y = new_center[1];
		
			//For testing purposes
			//ct_st.center->x = 112.;
			//ct_st.center->y = 112.;

		}

		//Copy the frame to the array. This is slow, but can be sped up if one cares to do it.
		image_array(&frame, ct_st.im_array);

		//Initialize position_vector, contour_px, contour_fine, done, etc
		//Uncommenting leads to seg fault
		for (int j = 0; j < SAMPLE; j++)
		{
			ct_st.position_vector->x = old_contour[2*j]; 
			ct_st.position_vector->y = old_contour[2*j + 1]; 
			ct_st.contour_px[0] = (int) ct_st.position_vector->x;
			ct_st.contour_px[1] = (int) ct_st.position_vector->y;
			ct_st.contour_fine[0] = (double) ct_st.contour_px[0];
			ct_st.contour_fine[1] = (double) ct_st.contour_px[1];

			ct_st.done = 0;
			ct_st.max_i = 0;
			ct::get_contour(&ct_st);
			if (ct_st.max_i != -1)
			{
				break;
			}
		}

		if (ct_st.max_i == -1) //Bad frame
		{
				bad_frames++;
		}
		else //Good frame
		{
			for (int j = 0; j < SAMPLE; j++) //Copy current contour into old_contour
			{
				old_contour[2*j] = ct_st.contour_fine[2*j*ct_st.max_i/SAMPLE];
				old_contour[2*j + 1] = ct_st.contour_fine[2*j*ct_st.max_i/SAMPLE + 1];
			}
		}

		save_contour(contour_filename_new, ct_st); //Save everything

		if(VERBOSE)
		{
			//This is just to print stuff out
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
		}

		i++;

	}

	if (VERBOSE)
	{
		for (int cc = 0; cc < characters + 4; cc++)
		{
			printf("\b");
		}
		printf("Total frames = %d\n", i);
		printf("Frames dropped = %d\n", bad_frames);
		printf("\n");
	}

	free(ct_st.contour_fine);
	free(ct_st.contour_px);
	free(ct_st.im_array);

	return 0;
}
