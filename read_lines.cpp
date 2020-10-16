//g++ -std=c++11 read_lines.cpp -o read_lines
//
#include"temika_header.h"
#include"get_movie_frame.h"
#include<string.h>
#include"contour_analyzer_lib.h"

#define MAX_MODE 20
#define MIN_MODE 0

int main(int argc, char **argv)
{
	struct ct::ContourStruct ct_st;
        ct::Vector position_vector(-1,-1);
        ct::Vector center(-1, -1);
        ct_st.position_vector = &position_vector;
        ct_st.center = &center;

	double *cc = (double*) malloc(sizeof(double)*(MAX_MODE - MIN_MODE + 1));
	for (int i = 1; i < argc; i++)
	{
		FILE *fptr = fopen(argv[i],"r");
		if (fptr == NULL)
		{
			return -1;
		}
		else
		{

			const char *contour_full_ext = "_contour_full.txt";
			const char *fourier_ext = "_modes.txt";
			char *fourier_filename = (char*) malloc((strlen(argv[i]) + strlen(fourier_ext) - strlen(contour_full_ext))*sizeof(char));
			strncpy(fourier_filename, argv[i], strlen(argv[i]) - strlen(contour_full_ext));
			strcat(fourier_filename, fourier_ext);

			int good_line_count = 0; //REMOVE
			int bad_line_count = 0; //REMOVE

			FILE *fptr_save = fopen(fourier_filename, "w");
			if (fptr_save == NULL)
			{
				return -1;
			}
			else
			{

				//Print header
				for (int q = MIN_MODE; q <= MAX_MODE; q++)
				{
				      fprintf(fptr_save, "%d\t", q);
				}
				fprintf(fptr_save, "L(px)");
				fprintf(fptr_save,"\n");

				double L;
				while(read_contour(fptr, &ct_st) != -1)
				{
//					printf("max_i = %d\n", ct_st.max_i);
					if (ct_st.max_i != -1)
					{
						struct Contour cs;
						cs.max_i = ct_st.max_i - ct_st.start;
						cs.contour = ct_st.contour_fine + 2*ct_st.start;
						L = analyze_contour(&cs, cc, MIN_MODE, MAX_MODE);
						for (int q = MIN_MODE; q <= MAX_MODE; q++)
						{
						      fprintf(fptr_save, "%f\t", cc[q]);
						}
						fprintf(fptr_save,"%f", L);
						fprintf(fptr_save,"\n");
						good_line_count++;
					}
					else
					{
						for (int q = MIN_MODE; q <= MAX_MODE; q++)
						{
						      fprintf(fptr_save, "%d\t", -1);
						}
						fprintf(fptr_save,"%d", -1);
						fprintf(fptr_save,"\n");
						bad_line_count++;
					}
				}
			}
			fclose(fptr_save);
			free(fourier_filename);
			printf("good_line_count %d\n", good_line_count);
			printf("bad_line_count %d\n", bad_line_count);
		}

		fclose(fptr);
	}
	free(cc);

	return 0;
}
