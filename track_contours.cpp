/* ---- DESCRIPTION OF THE SCRIPT ----

*coming soon*


key: #####(a.h) = function defined in header a    <-- [hardly used]

a bit on char vs string from sstackoverflow:
# A string is a class that contains a char array, but automatically manages it for you. 
# Most string implementations have a built-in array of 16 characters (so short strings don't fragment the heap) and use the heap for longer strings.


the asterisk (*) seems to be used for:
# declaring pointers (where "reference variables" are denoted &x, which seems to be essentially another way of using the variable, although & can also
  be used to retrieve the "memory address" of the variable. A pointer stores the memory address as the value of the variable. The pointer variable type 
  has to be the same as the type of variable it is pointing to.
  This can also be done with any combo: 
  	string* mystring; // Preferred
	string *mystring;
	string * mystring;)
# dereferencing pointers (if a variable is a pointer, say string* ptr = &food, then you can get the actual value of the variable using *ptr.
	Maybe this is why it's nice to keep conventions wrt declaring and deferencing pointers. 
	NB modifying pointers will change the value of the original variable.)
# multiplication (of course)


NOTE: when doing stuff locally use g++ over gcc as it does some linking by default!!!

Try the makefile.

-------------------------------------- */



//g++ -std=c++11 test_movie.cpp -o test_movie
/* does the above mean we are still working with c++11? When we're up to c++20 stable now? */

#include"temika_header.h" // hoping this won't be needed
#include"get_movie_frame.h" // no need to compile headers
#include<string.h> 
#include"contour_analyzer_lib.h" 
// maybe it was meant to be this one!!
//#include"contour_tracker_lib.h"

#define SAMPLE 10

#define I_MAX 100000

#define VERBOSE 1

int track_contour(char* moviefile) // this is a function which (I think) returns an integer given the moviefile memory address as an argument
{
	// what does this bit do, seems like a lot of initialisation:
	struct camera_frame_struct frame;
	long offset = 0;
	int i = 0;
	double* im_array; // declaring the pointer im_array
	char contour_filename[100]; // this notation means the variable contour_filenme will have 100 elements (characters in this case)
	char contour_filename_new[100];
	const char* movie_ext = ".movie"; // not sure what value this pointer will have; on testing with a simple code (std::cout<<movie_ext;) it gave ".movie"

	double new_center[2]; // similarly here you will get an array of two doubles in this variable
	double new_position[2];
	
	// seems to be the source of the first error... it should be defined elsewhere, and the "cs" is just an instance of that data type, just like
	// declaring any other variable. The problem is, it 
	struct Contour cs;
	
	
	if (VERBOSE) {
		printf("Structures initialised. \n");	
	}
	
	
	struct ct::ContourStruct ct_st; // declaring any other variable, of type struct ct::ContourStruct #####(contour_tracker_lib.h)
	ct_st.max = 1000;
	ct_st.burn = 20;
	ct_st.chirality = 1;
	ct_st.contour_fine = (double*) malloc(2*ct_st.max*sizeof(double));
	ct_st.contour_px = (int*) malloc(2*ct_st.max*sizeof(int));

	ct::Vector position_vector(-1,-1); // using the namespace thing, cf std::cout in older scripts
	ct::Vector center(-1, -1);
	ct_st.position_vector = &position_vector;
	ct_st.center = &center;

	int horizontal_window = 20;
	int slope_window = 5;
	ct_st.horizontal_window = horizontal_window;
	ct_st.slope_window = slope_window;

	int old_max_i;
	double old_contour[2*SAMPLE];

	strcpy(contour_filename, moviefile); // copy the strings?
	char *p = strstr(contour_filename, movie_ext);
	*p = '\0';
	strcat(contour_filename, "_contour.txt");

	strcpy(contour_filename_new, moviefile);
	char *p_new = strstr(contour_filename_new, movie_ext);
	*p_new = '\0';
	strcat(contour_filename_new, "_contour_full.txt"); // concatenate these strings, as in the python file
	
	// initialising some integers
	int bad_frames = 0;
	int characters = 0;
	
	// WHAT IS THIS
	FILE *file;
	if ( !( file = fopen(moviefile, "rb" ) ) )
	{
		printf( "Couldn't open movie file.\n" );
		exit( EXIT_FAILURE );
	}
	
	if (VERBOSE) {
		printf("FILE opened. \n");	
	}

	while(((offset = get_frame(file, &frame)) != -1) && i < I_MAX)
		// if we can pass something else in as "frame" which doesn't cause a segmentation fault, we're good. What this does can't be too hard.
		// get_frame() from get_movie_frame.h
	{
		if (i == 0)
		{ // this whole statement cannot be completed. Work out what it does and if it's necessary.
			/*
			Only seems to happen at the start, so I guess it it some sort of initialisation.
			
			
			*/
			
			
			if (VERBOSE) {
				printf("i==0 case. \n");	
			}
			//allocate the image array based on first frame
			ct_st.im_array = (double*) malloc(sizeof(double)*frame.size_x*frame.size_y);
			
			
			if (VERBOSE) {
				printf("Allocated some memory [i==0 case]. \n");	
			}
			
			ct_st.rows = frame.size_x;
			ct_st.cols = frame.size_y;

			//load the first contour - in contour_analyzer_lib.h
			load_contour(contour_filename, &cs);
			
			if (VERBOSE) {
				printf("Loaded contour [i==0 case]. \n");	
			}
			for (int j = 0; j < SAMPLE; j++)
			{
				old_contour[2*j] = cs.contour[2*j*cs.max_i/SAMPLE];
				old_contour[2*j + 1] = cs.contour[2*j*cs.max_i/SAMPLE + 1];
			}
			
			if (VERBOSE) {
				printf("Done for loop [i==0 case]. \n");	
			}
			
			contour_center(&cs, new_center);
			
			if (VERBOSE) {
				printf("contour_centered() [i==0 case]. \n");	
			}
			
			ct_st.center->x = new_center[0];
			ct_st.center->y = new_center[1];
		}
		
		if (VERBOSE) {
			printf("Finished i==0 case. \n");	
		}
	
		image_array(&frame, ct_st.im_array);
		
		if (VERBOSE) {
			printf("image_array() completed successfully. \n");	
		}
		
		
		//No way around it. Need to save old contour points for sampling elsewhere.
		for (int j = 0; j < SAMPLE; j++)
		{
			ct_st.position_vector->x = old_contour[2*j]; 
			ct_st.position_vector->y = old_contour[2*j + 1]; 
			ct_st.contour_px[0] = (int) ct_st.position_vector->x;
			ct_st.contour_px[1] = (int) ct_st.position_vector->y;
			ct_st.contour_fine[0] = (double) ct_st.contour_px[0];
			ct_st.contour_fine[1] = (double) ct_st.contour_px[1];
			ct_st.done = -1;
			ct_st.start = -1;
			ct::get_contour(&ct_st);
			if (ct_st.max_i != -1)
			{
				break;
			}
		}
		
		if (VERBOSE) {
			printf("for loop completed. \n");	
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
		
		if (VERBOSE) {
			printf("about to save_contour(). \n");	
		}
		save_contour(contour_filename_new, ct_st); //Save everything
		
		if (VERBOSE) {
			printf("contour_saved(). \n");	
		}
		
		
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



// this is the actual active code, essentially just repeating the above function loads (argc times, whatever that is)
int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		printf("Tracking %s\n", argv[i]);
		track_contour(argv[i]);
	}

	return 0;
}
