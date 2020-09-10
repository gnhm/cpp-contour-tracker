//g++ -std=c++11 test_movie.cpp -o test_movie

#include"temika_header.h"
#include"get_movie_frame.h"
#include<string.h>
#include"contour_analyzer_lib.h"

int main(int argc, char **argv)
{
	struct ct::ContourStruct ct_st;
        ct::Vector position_vector(-1,-1);
        ct::Vector center(-1, -1);
        ct_st.position_vector = &position_vector;
        ct_st.center = &center;

	for (int i = 1; i < argc; i++)
	{
		FILE *fptr = fopen(argv[i],"r");
		if (fptr == NULL)
		{
			return -1;
		}
		else
		{
			while(read_contour(fptr, &ct_st) != -1)
			{
				if (ct_st.max_i != 1)
				{
					struct Contour cs;
					cs.max_i = ct_st.max_i - ct_st.start;
					cs.contour = ct_st.contour_fine + 2*ct_st.start;
					analyze_contour(&cs);
				}
				else
				{
//					printf("bad frame\n");
				}
				printf("\n");
			}
		}

	}

	return 0;
}
