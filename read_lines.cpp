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
					printf("Center = (%f, %f)\n", ct_st.center->x, ct_st.center->y);
					printf("Position vector = (%f, %f)\n", ct_st.position_vector->x, ct_st.position_vector->y);
					printf("max_i = %d\n", ct_st.max_i);
					printf("burn = %d\n", ct_st.burn);
					for (int j = 0; j < 2; j++)
					{
						printf("%f\t%f\n", ct_st.contour_fine[2*j], ct_st.contour_fine[2*j + 1]);
					}
				}
				else
				{
					printf("bad frame\n");
				}
				printf("\n");
			}
		}

	}

	return 0;
}
