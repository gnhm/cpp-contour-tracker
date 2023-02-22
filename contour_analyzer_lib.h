#include <cmath>
#include <math.h>
#include"contour_tracker_lib.h"

#define NM_PER_PX 97.6

// from -trackmovie version
#define MAX_BUFFER 1000


//TODO The first and last points are the same... The program is written assuming they are not. Go through and fix
//The fourier transform needs to be fixed due to angle displacement
void analyze_contour(double *c, int n)
{
	double L = 0; //Circumference
	double x_c = 0;
	double y_c = 0;
	double ds_new;
	double ds_old;
	for (int i = 0; i < n - 1; i++)
	{
		ds_new = sqrt(pow((c[2*(i+1)] - c[2*i]), 2) + pow((c[2*(i+1) + 1] - c[2*i + 1]), 2));

		L += ds_new;
		if (i == 0)
		{
			ds_old = sqrt(pow((c[2*n] - c[0]), 2) + pow((c[2*n + 1] - c[1]), 2));
		}

		x_c += c[2*i]*(ds_new + ds_old);
		y_c += c[2*i + 1]*(ds_new + ds_old);
		ds_old = ds_new;
	}	
	x_c /= 2*L;
	y_c /= 2*L;

//	printf("R = %f nm\n", L*NM_PER_PX/(2*M_PI));
//	printf("Center = (%f, %f)\n", x_c, y_c);

	double r[n];
	double theta[n];

	double xt;
	double yt;
	for (int i = 0; i < n ; i++)
	{
		xt = c[2*i] - x_c;
		yt = c[2*i + 1] - y_c;

		r[i] = sqrt(pow(xt, 2) + pow(yt, 2));

		if (xt > 0)
		{
			theta[i] = atan(yt/xt);
		}
		else if (xt < 0 && yt > 0)
		{
			theta[i] = atan(yt/xt) + M_PI;
		}
		else if (xt < 0 && yt < 0)
		{
			theta[i] = atan(yt/xt) - M_PI;
		}

		if (theta[i] < 0)
		{
			theta[i] += 2*M_PI;
		}

		//TODO We want the "jump" to occur between n-1 and 0, and nowhere else
//		theta[i] += 2*M_PI - theta[0];
//		printf("%f\t%f\n", theta[i], r[i]);
	}


	//TODO This doesn't work, probably because of angle displacement gets very big.

	int max_mode = 20;
	int min_mode = 0;
	double a[max_mode - min_mode + 1];
	double b[max_mode - min_mode + 1];
	double cc[max_mode - min_mode + 1];
	double r0 = 0;
	double w;
	double total_angle = 0;

	/*
	for (int i = 0; i < n; i++)
	{
		r[i] = 20;
		theta[i] = 2*M_PI*((double)i)/((double)(n-1));
	}
	*/

	for (int i = 0; i < n - 1; i++)
	{
		w = std::abs(theta[i+1] - theta[i]) < M_PI ? std::abs(theta[i+1] - theta[i]) : 2*M_PI - std::abs((theta[i+1] - theta[i]));
		total_angle += w;
		r0 += ((r[i] + r[i+1])/2)*w/(2*M_PI);
		for (int j = min_mode; j <= max_mode; j++)
		{
			if (i == 0)
			{
				a[j] = 0;
				b[j] = 0;
			}
			a[j] += 0.5*(r[i]*cos(j*theta[i]) + r[i+1]*cos(j*theta[i+1]))*w;
			b[j] += 0.5*(r[i]*sin(j*theta[i]) + r[i+1]*sin(j*theta[i+1]))*w;
		}
	}
	//MODE 0 = Should be normalized to 1
	//MODE 1 = How "heart" or "kidney" shaped is it?
	//MODE 2 = How dumbbell shaped is it?
	//MODE 3 = How tree-foil shaped is it?
	//Maybe from mode 3 and above, we check that the < 0.15
	//TODO One of the modes (10? changes everything I run it... WHY??)
	for (int i = min_mode; i <= max_mode; i++)
	{
		cc[i] = sqrt(pow(a[i], 2) + pow(b[i], 2))/(2*M_PI*r0);
		printf("%d\t%f\n", i, cc[i]);
	}
	printf("\n");
	printf("Total angle = %f\n", total_angle);
}




/***********************************





added from -trackmovie version to have two separate dependencies




*************************************/

struct Contour
{
        double *contour; // should be double *contour; having some memory issues with this
        int max_i;
};

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

long int read_contour(FILE *fptr, ct::ContourStruct *ct_st)
{
	/*
	FILE *fptr = fopen(contour_filename,"r");
	if (fptr == NULL)
	{
		return -1;
	}
	*/

	char line[MAX_BUFFER];
	int i = 0;
	char c;

	char *p;

	bool contour_fine_mode = false;
	bool contour_px_mode = false;

	int contour_fine_i = 0;
	int contour_px_i = 0;

	while ((c = getc(fptr)) != EOF)
	{
		if (c == '\n')
		{
			line[i] = '\0';
			i = 0;

			if (contour_fine_mode)
			{

				if(strstr(line, "</contour_fine>") != NULL)
				{
					contour_fine_mode = false;
					contour_fine_i = 0;
				}
				else
				{
					double c_x;
					double c_y;

					char *p = strchr(line, '\t');
					c_y = atof(p);
					*p = '\0';
					c_x = atof(line);
					ct_st->contour_fine[2*contour_fine_i] = c_x;
					ct_st->contour_fine[2*contour_fine_i + 1] = c_y;
					contour_fine_i++;
				}
			}

			else if (contour_px_mode)
			{

				if(strstr(line, "</contour_px>") != NULL)
				{
					contour_px_mode = false;
					contour_px_i = 0;
					return ftell(fptr);
				}
				else
				{
					int c_x;
					int c_y;

					char *p = strchr(line, '\t');
					c_y = atoi(p);
					*p = '\0';
					c_x = atoi(line);
					ct_st->contour_px[2*contour_px_i] = c_x;
					ct_st->contour_px[2*contour_px_i + 1] = c_y;
					contour_px_i++;
				}
			}
			else
			{

				if(strstr(line, "Center = ") != NULL)
				{
					char *pstart = strchr(line, '(');
					char *pend = strchr(pstart, ',');
					long int n = (long int) (pend - pstart);

					char buff[n-1];
					memcpy(buff, pstart + 1, n - 1);
					buff[n] = '\0';

					ct_st->center->x = atof(buff);

					char *pend2 = strchr(pstart, ')');
					long int n2 = (long int) (pend2 - pend);
					char buff2[n2-1];
					memcpy(buff2, pend + 2, n2 - 2);
					buff2[n2 - 2] = '\0';
					ct_st->center->y = atof(buff2);
				}

				else if(strstr(line, "Position vector = ") != NULL)
				{
					char *pstart = strchr(line, '(');
					char *pend = strchr(pstart, ',');
					long int n = (long int) (pend - pstart);

					char buff[n-1];
					memcpy(buff, pstart + 1, n - 1);
					buff[n] = '\0';

					ct_st->position_vector->x = atof(buff);

					char *pend2 = strchr(pstart, ')');
					long int n2 = (long int) (pend2 - pend);
					char buff2[n2-1];
					memcpy(buff2, pend + 2, n2 - 2);
					buff2[n2 - 2] = '\0';
					ct_st->position_vector->y = atof(buff2);
				}

				else if(strstr(line, "max_i = ") != NULL)
				{
					char *pstart = strstr(line, " = ");
					ct_st->max_i = atoi(pstart + 3);
				}

				else if(strstr(line, "burn = ") != NULL)
				{
					char *pstart = strstr(line, " = ");
					ct_st->burn = atoi(pstart + 3);
				}

				else if(strstr(line, "horizontal_window = ") != NULL)
				{
					char *pstart = strstr(line, " = ");
					ct_st->horizontal_window = atoi(pstart + 3);
				}

				else if(strstr(line, "slope_window = ") != NULL)
				{
					char *pstart = strstr(line, " = ");
					ct_st->slope_window = atoi(pstart + 3);
				}

				else if(strstr(line, "chirality = ") != NULL)
				{
					char *pstart = strstr(line, " = ");
					ct_st->chirality = atoi(pstart + 3);
				}

				else if(strstr(line, "<contour_fine>") != NULL)
				{
					contour_fine_mode = true;
					if (ct_st->max_i != -1)
					{
						ct_st->contour_fine = (double*) malloc(sizeof(double)*2*ct_st->max_i);
					}
					else
					{
						return 0;
					}

				}

				else if(strstr(line, "<contour_px>") != NULL)
				{
					contour_px_mode = true;
					if (ct_st->max_i != -1)
					{
						ct_st->contour_px = (int*) malloc(sizeof(int)*2*ct_st->max_i);
					}
					else
					{
						return 0;
					}

				}
			}
		}
		else
		{
			line[i] = c;
			i++;
		}
	}
	return -1;
}

void contour_center(struct Contour* cs, double *center)
//void analyze_contour(double *c, int n)
{
        double L = 0; //Circumference
        double x_c = 0;
        double y_c = 0;
        double ds_new;
        double ds_old;
        for (int i = 0; i < cs->max_i - 1; i++)
        {
                ds_new = sqrt(pow((cs->contour[2*(i+1)] - cs->contour[2*i]), 2) + pow((cs->contour[2*(i+1) + 1] - cs->contour[2*i + 1]), 2));

                L += ds_new;
                if (i == 0)
                {
                        ds_old = sqrt(pow((cs->contour[2*(cs->max_i - 1)] - cs->contour[0]), 2) + pow((cs->contour[2*(cs->max_i - 1) + 1] - cs->contour[1]), 2));
                }

                x_c += cs->contour[2*i]*(ds_new + ds_old);
                y_c += cs->contour[2*i + 1]*(ds_new + ds_old);
                ds_old = ds_new;
        }
        x_c /= 2*L;
        y_c /= 2*L;
	center[0] = x_c;
	center[1] = y_c;
}


/**************
//TODO The first and last points are the same... The program is written assuming they are not. Go through and fix
//The fourier transform needs to be fixed due to angle displacement
double analyze_contour(struct Contour* cs, double* cc, int min_mode = 0, int max_mode = 20)
//void analyze_contour(double *c, int n)
{
	double L = 0; //Circumference
	double x_c = 0;
	double y_c = 0;
	double ds_new;
	double ds_old;
	for (int i = 0; i < cs->max_i - 1; i++)
	{
		ds_new = sqrt(pow((cs->contour[2*(i+1)] - cs->contour[2*i]), 2) + pow((cs->contour[2*(i+1) + 1] - cs->contour[2*i + 1]), 2));

		L += ds_new;
		if (i == 0)
		{
			ds_old = sqrt(pow((cs->contour[2*(cs->max_i - 1)] - cs->contour[0]), 2) + pow((cs->contour[2*(cs->max_i - 1) + 1] - cs->contour[1]), 2));
		}

		x_c += cs->contour[2*i]*(ds_new + ds_old);
		y_c += cs->contour[2*i + 1]*(ds_new + ds_old);
		ds_old = ds_new;
	}	
	x_c /= 2*L;
	y_c /= 2*L;

	//printf("R = %f nm\n", L*NM_PER_PX/(2*M_PI));
	//TODO The center is not correct. There is some weird problem
	//printf("Center = (%f, %f)\n", x_c, y_c);

	double r[cs->max_i];
	double theta[cs->max_i];

	double xt;
	double yt;
	for (int i = 0; i < cs->max_i ; i++)
	{
		xt = cs->contour[2*i] - x_c;
		yt = cs->contour[2*i + 1] - y_c;

		r[i] = sqrt(pow(xt, 2) + pow(yt, 2));

		if (xt > 0)
		{
			theta[i] = atan(yt/xt);
		}
		else if (xt < 0 && yt > 0)
		{
			theta[i] = atan(yt/xt) + M_PI;
		}
		else if (xt < 0 && yt < 0)
		{
			theta[i] = atan(yt/xt) - M_PI;
		}

		if (theta[i] < 0)
		{
			theta[i] += 2*M_PI;
		}

	}


	double a[max_mode - min_mode + 1];
	double b[max_mode - min_mode + 1];
//	double cc[max_mode - min_mode + 1];
	double r0 = 0;
	double w;
	double total_angle = 0;

	for (int i = 0; i < cs->max_i - 1; i++)
	{
		w = std::abs(theta[i+1] - theta[i]) < M_PI ? std::abs(theta[i+1] - theta[i]) : 2*M_PI - std::abs((theta[i+1] - theta[i]));
		total_angle += w;
		r0 += ((r[i] + r[i+1])/2)*w/(2*M_PI);
		for (int j = min_mode; j <= max_mode; j++)
		{
			if (i == 0)
			{
				a[j] = 0;
				b[j] = 0;
			}
			a[j] += 0.5*(r[i]*cos(j*theta[i]) + r[i+1]*cos(j*theta[i+1]))*w;
			b[j] += 0.5*(r[i]*sin(j*theta[i]) + r[i+1]*sin(j*theta[i+1]))*w;
		}
	}

	for (int i = min_mode; i <= max_mode; i++)
	{
		cc[i] = sqrt(pow(a[i], 2) + pow(b[i], 2))/(2*M_PI*r0);
	}
	return L;
	//MODE 0 = Should be normalized to 1
	//MODE 1 = How "heart" or "kidney" shaped is it?
	//MODE 2 = How dumbbell shaped is it?
	//MODE 3 = How tree-foil shaped is it?
	//Maybe from mode 3 and above, we check that the < 0.15
//	for (int i = min_mode; i <= max_mode; i++)
//	{
//		cc[i] = sqrt(pow(a[i], 2) + pow(b[i], 2))/(2*M_PI*r0);
//		printf("%f\t", cc[i]);
//	}
//	printf("\n");
//	printf("Total angle = %f\n", total_angle);
}

**************/
