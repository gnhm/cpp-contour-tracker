#include <cmath>
#include <math.h>
#include <assert.h>
#include "running_slope.h"

#define VEC_X 0;
#define VEC_Y 1;
#define VEC_V 2;
#define VEC_W 3;

#define SQRT2 1.41421356237;

#define INITIAL_POINTS 10

// these two both from contour_analyzer-trackmovie_lib.h
#define NM_PER_PX 97.6
#define MAX_BUFFER 1000

namespace ct
{
	class Vector
	{
		public:

			double x;
			double y;
			Vector(double a, double b)
			{
				x = a;
				y = b;
			}
			double norm()
			{
				double n = sqrt(x*x + y*y);
				return n;
			}
	};

	template <typename T> int sgn(T val)
	{
		return (T(0) < val) - (val < T(0));
	}

	double dot(Vector v, Vector w)
	{
		return v.x*w.x + v.y*w.y;
	}

	double angle_between(Vector v, Vector w)
	{
		double s = sgn(v.x*w.y - v.y*w.x);
		double dt = dot(v, w)/(v.norm()*w.norm());
		double d = acos(dt <= 1 ? dt : 1);
		return s < 0 ? 2*M_PI - d : d;
	}

	enum axes {x, y, v, w};

	/* Vim's error detection thing will flag these Vectors because the way we define them. Nevertheless, it's fine so long
	 * as you compile with the -std=c++11 fla */
	axes axes_int[4] = {x, y, v, w};
	Vector unit_vectors[4] = { {1.,0.}, {0., 1.}, {1./sqrt(2), 1./sqrt(2)}, {-1./sqrt(2), 1./sqrt(2)}}; //This is fine, but vim gets annoyed because of c++11 bug
	Vector unit_vectors_px[4] = { {1.,0.}, {0., 1.}, {1., 1.}, {-1., 1.}}; //This is fine, but vim gets annoyed because of c++11 bug
	int perpendicular[4] = {1,0,3,2}; 

	int profile(double *image, int rows, int cols, double *full_profile, Vector c, axes axis, int horizontal_window)
	{
		int coordinates_width;

		if (c.x < 0 || c.y < 0 || c.x >= rows || c.y >= cols)
		{
			return -1;
		}

		switch(axis)
		{
			case x: 
				{
					int start = c.x - horizontal_window > 0 ? c.x - horizontal_window : 0;
					int end = c.x + horizontal_window + 1 < cols ? c.x + horizontal_window + 1 : cols;
					coordinates_width = end - start;
					for (int i = 0; i < coordinates_width; i++)
					{
                                               // full_profile[i*3] = i + start;
                                               // full_profile[i*3 + 1] = c.y;
                                               // full_profile[i*3 + 2] = image[i + start + ((int) c.y)*rows];

						full_profile[i] = i + start;
						full_profile[i + coordinates_width] = c.y;
						full_profile[i + 2*coordinates_width] = image[i + start + ((int) c.y)*rows];

					}
					return coordinates_width;
					break;
				}
			case y: 
				{
					int start = c.y - horizontal_window > 0 ? c.y - horizontal_window : 0;
					int end = c.y + horizontal_window + 1 < rows ? c.y + horizontal_window + 1 : rows;
					coordinates_width = end - start;
					for (int i = 0; i < coordinates_width; i++)
					{
						//full_profile[i*3] = c.x;
						//full_profile[i*3 + 1] = i + start;
						//full_profile[i*3 + 2] = image[((int) c.x) + (i + start)*rows];
						full_profile[i] = c.x;
						full_profile[i + coordinates_width] = i + start;
						full_profile[i + 2*coordinates_width] = image[((int) c.x) + (i + start)*rows];

					}
					return coordinates_width;
					break;
				}
			case v: 
				{
					int start_x = c.x - horizontal_window;
					int start_y = c.y - horizontal_window;
					if (start_x < 0)
					{
						start_y += -start_x;
						start_x += -start_x;
					}
					if (start_y < 0)
					{
						start_x += -start_y;
						start_y += -start_y;
					}

					int end_x = c.x + horizontal_window + 1;
					int end_y = c.y + horizontal_window + 1;
					if (end_x >= rows)
					{
						end_y -= end_x - rows + 1;
						end_x -= end_x - rows + 1;
					}
					if (end_y >= cols)
					{
						end_x -= end_y - cols + 1;
						end_y -= end_y - cols + 1;
					}

					coordinates_width = end_x - start_x;
					for (int i = 0; i < coordinates_width; i++)
					{
						//full_profile[i*3] = i + start_x;
						//full_profile[i*3 + 1] = i + start_y;
						//full_profile[i*3 + 2] = image[i + start_x + (i + start_y)*rows];
						full_profile[i] = i + start_x;
						full_profile[i + coordinates_width] = i + start_y;
						full_profile[i + 2*coordinates_width] = image[i + start_x + (i + start_y)*rows];
					}	
					return coordinates_width;
					break;
				}
			case w:
				{
					int start_x = c.x + horizontal_window;
					int start_y = c.y - horizontal_window;
					if (start_x >= rows)
					{
						start_y += start_x - rows + 1;
						start_x -= start_x - rows + 1; 
					}
					if (start_y < 0)
					{
						start_x -= -start_y;
						start_y += -start_y;
					}

					int end_x = c.x - horizontal_window - 1;
					int end_y = c.y + horizontal_window + 1;
					if (end_x < 0)
					{
						end_y -= -end_x;
						end_x += -end_x;
					}
					if (end_y >= cols)
					{
						end_x += end_y - cols + 1;
						end_y -= end_y - cols + 1;
					}

					coordinates_width = end_y - start_y;
					for (int i = 0; i < coordinates_width; i++)
					{
					//	full_profile[i*3] = start_x - i;
					//	full_profile[i*3 + 1] = i + start_y;
					//	full_profile[i*3 + 2] = image[start_x - i + (i + start_y)*rows];
						full_profile[i] = start_x - i;
						full_profile[i + coordinates_width] = i + start_y;
						full_profile[i + 2*coordinates_width] = image[start_x - i + (i + start_y)*rows];
					}	
					return coordinates_width;
					break;
				}
		}
		return -1;
	}

	void max_slope(double *bar_point_v_slope, double *image, int rows, int cols, Vector c, axes axis, Vector center, int horizontal_window, int slope_window)
	{
		//TODO Heap or stack is better?
		//double *full_profile = (double*) malloc(sizeof(double)*(2*horizontal_window + 1)*3);
		double full_profile[(2*horizontal_window + 1)*3];

		int coordinates_width = profile(image, rows, cols, full_profile, c, axis, horizontal_window);
		int orientation = unit_vectors[axis].x*(center.x - c.x) + unit_vectors[axis].y*(center.y - c.y) < 0 ? -1 : 1;
//		Vector unit_vector(unit_vectors[axis].x*orientation, unit_vectors[axis].y*orientation);

		/*
		for (int j = 0; j < 3; j++)
		{
			for (int i = 0; i < coordinates_width; i++)
			{
				printf("%f\t", full_profile[j*coordinates_width + i]);
			}
			printf("\n");
		}
		*/

		double slope_intercept[2];

		//TODO Finding the mean. This is slow, since we've already gone through the profile.
		double p = 0;
		for (int i = 0; i < coordinates_width; i++)
		{
			p += full_profile[2*coordinates_width + i];
		}
		
		p /= (float) coordinates_width;

		double bar_point_v[2];
		double bar_point;

		switch(axis)
		{
			case x:
				get_maximum_slope(slope_intercept, full_profile, full_profile + 2*coordinates_width, coordinates_width, slope_window, orientation);
				bar_point = (p - slope_intercept[1])/slope_intercept[0];
				bar_point_v[0] = bar_point;
				bar_point_v[1] = c.y;
				break;
			case y:
				get_maximum_slope(slope_intercept, full_profile + coordinates_width, full_profile + 2*coordinates_width, coordinates_width, slope_window, orientation);
				bar_point = (p - slope_intercept[1])/slope_intercept[0];
				bar_point_v[0] = c.x;
				bar_point_v[1] = bar_point;
				break;
			case v:
				get_maximum_slope(slope_intercept, full_profile, full_profile + 2*coordinates_width, coordinates_width, slope_window, orientation);
				bar_point = (p - slope_intercept[1])/slope_intercept[0];
				bar_point_v[0] = bar_point;
				bar_point_v[1] = (bar_point - c.x) + c.y;
				slope_intercept[0] /= SQRT2;
				break;
			case w:
				get_maximum_slope(slope_intercept, full_profile + coordinates_width, full_profile + 2*coordinates_width, coordinates_width, slope_window, orientation);
				bar_point = (p - slope_intercept[1])/slope_intercept[0];
				bar_point_v[1] = bar_point;
				bar_point_v[0] = c.y - bar_point + c.x;
				slope_intercept[0] /= SQRT2;
				break;
		}

		bar_point_v_slope[0] = bar_point_v[0];
		bar_point_v_slope[1] = bar_point_v[1];
		bar_point_v_slope[2] = -slope_intercept[0]*orientation;

		//TODO If the axes are v or w, then the slope should be divided by sqrt(2)
		//printf("Orientation = %d\n", orientation);
		//printf("Slope = %f\n", slope_intercept[0]*orientation); //The slope * orientation should be a large negative number
		//printf("Bar point = %f\n", bar_point);
		//printf("Bar point v = (%f, %f)\n", bar_point_v[0], bar_point_v[1]);
	}

	void next_point(int *next_point_px, double *next_point_fine, double *image, int rows, int cols, int *contour, int contour_i, Vector center, int horizontal_window, int slope_window, int chirality)
	{
		Vector c((double)contour[contour_i*2 - 2], (double)contour[contour_i*2 - 1]);
		Vector r(c.x - center.x, c.y - center.y);
		Vector t(chirality*r.y, -chirality*r.x);

		double max_slope_found = 0;
		double bar_point_v_slope[3];

		double numerator_x = 0;
		double numerator_y = 0;
		double denominator = 0;

		double max_move[2];

		for (int i = 0; i < 4; i++)
		{
			axes ax = axes_int[i];
			Vector v = unit_vectors[ax];
			int projection = dot(v,t) > 0 ? 1 : - 1;
			Vector candidate(projection*unit_vectors_px[ax].x + c.x, projection*unit_vectors_px[ax].y + c.y);
			max_slope(bar_point_v_slope, image, rows, cols, candidate, axes_int[perpendicular[i]], center, horizontal_window, slope_window);
			numerator_x += bar_point_v_slope[0]*std::abs(bar_point_v_slope[2]);
			numerator_y += bar_point_v_slope[1]*std::abs(bar_point_v_slope[2]);
			denominator += std::abs(bar_point_v_slope[2]);
			if (i == 0 || bar_point_v_slope[2] > max_slope_found)
			{
				max_slope_found = bar_point_v_slope[2];
				max_move[0] = bar_point_v_slope[0];
				max_move[1] = bar_point_v_slope[1];
			}

		}
		next_point_fine[0] = numerator_x/denominator;
		next_point_fine[1] = numerator_y/denominator;

		//First method: Round
		next_point_px[0] = (int) next_point_fine[0];
		next_point_px[1] = (int) next_point_fine[1];

		//First test: Is it different from previous ten points?
		//for (int i = 1; i++; i < min(contour_i, INITIAL_POINTS + 1))
		int iter_max = INITIAL_POINTS + 1 < contour_i ? INITIAL_POINTS : contour_i;
		for (int i = 1; i < iter_max; i++)

		{
			if(next_point_px[-2*i] == next_point_px[0] && next_point_px[-2*i + 1] == next_point_px[1])
			{
				//If not, use second method: Largest slope
				next_point_px[0] = (int)max_move[0];
				next_point_px[1] = (int)max_move[1];
			}
		}
		/*
		if(next_point_px[-2] == next_point_px[0] && next_point_px[-2 + 1] == next_point_px[1])
		{
			//If not, use second method: Largest slope
			next_point_px[0] = (int)max_move[0];
			next_point_px[1] = (int)max_move[1];
		}
		*/
	}

	struct ContourStruct
	{
		double *contour_fine;
		int *contour_px;
		double *im_array;
		int rows;
		int cols;
		int max;
		int burn;
		Vector *center;
		Vector *position_vector;
		int horizontal_window;
		int slope_window;
		int chirality;
		int max_i;
		int done;
		int start; // added
	};

	void *get_contour(struct ContourStruct* ct_st)
	{
		ct_st->max_i = -1;

		for (int i = 1; i < ct_st->max; i++)
		{
			ct::next_point(ct_st->contour_px + 2*i, ct_st->contour_fine + 2*i, ct_st->im_array, ct_st->rows, ct_st->cols, ct_st->contour_px, i, *(ct_st->center), ct_st->horizontal_window, ct_st->slope_window, ct_st->chirality);
			if (i > ct_st->burn)
			{
				if (ct_st->contour_px[2*i] == ct_st->contour_px[2*ct_st->burn] && ct_st->contour_px[2*i + 1] == ct_st->contour_px[2*ct_st->burn + 1])
				{
					ct_st->max_i = i;
					break;
				}
			}
		}

		if (ct_st->max_i != -1)
		{
			ct_st->contour_fine += (ct_st->burn - 1)*2;
			ct_st->contour_px += (ct_st->burn - 1)*2;
			ct_st->max_i -= ct_st->burn - 1;
		}

		ct_st->done = 1;

		return NULL;
	}

	void *v_get_contour(void* ct_st)
	{
		return get_contour((struct ContourStruct*) ct_st);
	}

	int save_contour(char *contour_filename, struct ct::ContourStruct ct_st)
	{
		FILE *fptr = fopen(contour_filename,"w");
		if (fptr == NULL)
		{
			return -1;
		}
		else
		{
			fprintf(fptr, "Center = (%f, %f)\n", ct_st.center->x, ct_st.center->y);
			fprintf(fptr, "Position vector = (%f, %f)\n", ct_st.position_vector->x, ct_st.position_vector->y);
			fprintf(fptr, "max_i = %d\n", ct_st.max_i);

			fprintf(fptr, "<contour_fine>\n");
			int n_max = ct_st.max_i == -1 ? ct_st.max : ct_st.max_i;
			for (int i = 0; i < n_max; i++)
			{
				fprintf(fptr, "%f\t%f\n", ct_st.contour_fine[2*i], ct_st.contour_fine[2*i + 1]);
			}
			fprintf(fptr, "</contour_fine>\n");
			fprintf(fptr, "<contour_px>\n");
			for (int i = 0; i < n_max; i++)
			{
				fprintf(fptr, "%d\t%d\n", ct_st.contour_px[2*i], ct_st.contour_px[2*i + 1]);
			}
			fprintf(fptr, "</contour_px>\n");
			fclose(fptr);
			return 0;
		}
	}
}







/* *******************
**********************



Everything below here added from contour_analyzer-trackmovie.h to try and neaten things up a bit



**********************
********************** */


struct Contour
{
        double *contour;
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
