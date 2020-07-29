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
int coordinates_width;

int main(int argc, char **argv)
{
	first_frame(&temika_frame); //Get the first frame from the movie
	double *im_array = (double*) malloc(sizeof(double)*temika_frame.size_x*temika_frame.size_y); //Allocate the array
	image_array(&temika_frame, im_array); //Transform the temika array into an image array

	//Allocate a big enough array to write full profile
	double *full_profile = (double*) malloc(sizeof(double)*(2*horizontal_window + 1)*3); 


	cout << temika_frame.size_x << endl;
	cout << temika_frame.size_y << endl;

	//Get profile
	ct::Vector position_vector(928, 5);
	coordinates_width = profile(im_array, temika_frame.size_x, temika_frame.size_x, full_profile, position_vector, ct::x, horizontal_window);
	printf("coordinates_width = %d\n", coordinates_width);

	if (coordinates_width == -1)
	{
		printf("Off frame\n");
		return -1;
	}

	//print stuff

	printf("x =");
	for (int i = 0; i < coordinates_width; i++)
	{
		printf(" %f", full_profile[i*3]);
	}
	printf("\n");

	printf("y =");
	for (int i = 0; i < coordinates_width; i++)
	{
		printf(" %f", full_profile[i*3 + 1]);
	}
	printf("\n");

	printf("p =");
	for (int i = 0; i < coordinates_width; i++)
	{
		printf(" %f", full_profile[i*3 + 2]);
	}
	printf("\n");

	return 1;
}
