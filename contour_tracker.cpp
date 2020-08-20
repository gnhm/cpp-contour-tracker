//g++ $(pkg-config --cflags --libs opencv) -std=c++11 contour_tracker.cpp -o contour_tracker

#include "temika_header.h"
#include "contour_tracker_lib.h"
#include <iostream>
#include "get_movie_frame.h"
#include "contour_analyzer_lib.h"

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

	//TODO Test with actual cell
	//TODO Fourier decompose, and check how "bumpy" it is

	first_frame(&temika_frame); //Get the first frame from the movie
	double *im_array = (double*) malloc(sizeof(double)*temika_frame.size_x*temika_frame.size_y); //Allocate the array
	image_array(&temika_frame, im_array); //Transform the temika array into an image array

	/*
        int n = 500;
        double R = 100;
        double s = 10;

	double *im_array = (double*) malloc(sizeof(double)*n*n); //Allocate the array
	fake_circle(im_array, n, R, s); //Creates a fake circular image, for trouble shooting

	ct::Vector center(n/2, n/2);
	double angle = 0.00;
	ct::Vector position_vector((0.5*n) + R*cos(angle), (0.5*n) + R*sin(angle));
	*/

	ct::Vector center(temika_frame.size_x/2, temika_frame.size_y/2);
	ct::Vector position_vector(152, 115);

	int max = 500;
	int burn = 10;
	int chirality = 1;
	double *contour_fine = (double*) malloc(2*max*sizeof(double));
	int *contour_px = (int*) malloc(2*max*sizeof(int));

	contour_px[0] = (int) position_vector.x;
	contour_px[1] = (int) position_vector.y;
	contour_fine[0] = (double) contour_px[0];
	contour_fine[1] = (double) contour_px[1];

	int max_i = ct::get_contour(contour_fine, contour_px, im_array, temika_frame.size_x, temika_frame.size_y, max, burn, center, horizontal_window, slope_window, chirality);

	if (max_i == -1)
	{
		printf("Did not close\n");
		return 1;
	}
	else
	{
		contour_fine += (burn - 1)*2;
		max_i -= burn - 1;
	}
	analyze_contour(contour_fine, max_i);

	/*
	printf("<IMAGE>\n");
	for (int j = 0; j < temika_frame.size_y; j++)
	{
		for (int i = 0; i < temika_frame.size_x; i++)
		{
			printf("%f\t", *(im_array + j*temika_frame.size_x + i));
		}
		printf("\n");
	}
	printf("</IMAGE>\n");

	printf("<CONTOUR>\n");
	if (max_i == -1)
	{
		printf("Did not close\n");
	}
	else
	{
		contour_fine += (burn - 1)*2;
		max_i -= burn - 1;

		for (int i = 0; i < max_i; i++)
		{
			printf("%f\t%f\n", contour_fine[2*i], contour_fine[2*i + 1]);
		}
	}
	printf("</CONTOUR>\n");
	*/

	return 1;
}
