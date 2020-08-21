#include <cmath>
#include <math.h>
#include <assert.h>
#include "running_slope.h"

#define VEC_X 0;
#define VEC_Y 1;
#define VEC_V 2;
#define VEC_W 3;

#define SQRT2 1.41421356237;

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

		//First test: Is it different from previous?
		if(next_point_px[-2] == next_point_px[0] && next_point_px[-2 + 1] == next_point_px[1])
		{
			//If not, use second method: Largest slope
			next_point_px[0] = (int)max_move[0];
			next_point_px[1] = (int)max_move[1];
		}
	}

	int get_contour(double *contour_fine, int *contour_px, double *im_array, int rows, int cols, int max, int burn, Vector center, int horizontal_window, int slope_window, int chirality)
	{
		int max_i = -1;

		for (int i = 1; i < max; i++)
		{
			ct::next_point(contour_px + 2*i, contour_fine + 2*i, im_array, rows, cols, contour_px, i, center, horizontal_window, slope_window, chirality);
			if (i > burn)
			{
				if (contour_px[2*i] == contour_px[2*burn] && contour_px[2*i + 1] == contour_px[2*burn + 1])
				{
					max_i = i;
					break;
				}
			}
		}

		return max_i;
	}
}
