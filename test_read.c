#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define MAX_BUFFER 1000

struct Contour
{
	double *contour;
	int max_i;
};

//void *load_contour(char *contour_filename, double* c, int* n)
int load_contour(char *contour_filename, struct Contour* cs)
{
	FILE *fptr = fopen(contour_filename,"r");

	char c;
	char line_buffer[1000];
	const char* start_tag = "<contour_fine>";
	const char* end_tag = "</contour_fine>";
	const char* max_i_tag = "max_i = ";
	int contour_read_mode = 0;

	double *contour;

	int max_i_found = 0;

	int cf_i;

	if (fptr == NULL)
	{
		return -1;
	}
	else
	{
		int i = 0;
		while ((c = getc(fptr)) != EOF)
		{
			if (c == '\n')
			{
				line_buffer[i] = '\0';

				if (strstr(line_buffer, start_tag) != NULL)
				{
					contour_read_mode = 1;
					cf_i = 0;
					if (max_i_found)
					{
						cs->contour = (double*) malloc(sizeof(double)*2*cs->max_i);
					}
					else
					{
						cs->contour = (double*) malloc(sizeof(double)*2*MAX_BUFFER);
					}
				}

				else if (strstr(line_buffer, max_i_tag))
				{
					cs->max_i = atoi(line_buffer + strlen(max_i_tag));
					max_i_found = 1;
				}

				else if (contour_read_mode)
				{
					if (strstr(line_buffer, end_tag) != NULL)
					{
						contour_read_mode = 0;
					}
					else
					{
						char *p = strchr(line_buffer, '\t');
						cs->contour[2*cf_i + 1] = atof(p);
						*p = '\0';
						cs->contour[2*cf_i] = atof(line_buffer);
						cf_i++;
						//printf("%s\n", line_buffer);
						//printf("%f\n", atof(p));
						//printf("%f\n", atof(line_buffer));
						//printf("\n");
					}
				}

				i = 0;
			}
			else
			{
				line_buffer[i] = c;
				i++;
			}
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct Contour cs;
	load_contour(argv[1], &cs);
	printf("max_i = %d\n", cs.max_i);
	for (int i = 0; i < cs.max_i; i++)
	{
		printf("%f\t%f\n", *(cs.contour + 2*i), *(cs.contour + 2*i + 1));
	}
	return 0;
}
