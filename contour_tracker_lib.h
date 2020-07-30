#include <math.h>
#include <assert.h>
#include "running_slope.h"

#define VEC_X 0;
#define VEC_Y 1;
#define VEC_V 2;
#define VEC_W 3;

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

	void max_slope(double *image, int rows, int cols, Vector c, axes axis, Vector center, int horizontal_window, int slope_window)
	{
		double *full_profile = (double*) malloc(sizeof(double)*(2*horizontal_window + 1)*3);
		int coordinates_width = profile(image, rows, cols, full_profile, c, axis, horizontal_window);
		int orientation = unit_vectors[axis].x*(center.x - c.x) + unit_vectors[axis].x*(center.y - c.y) < 0 ? -1 : 1;
		Vector unit_vector(unit_vectors[axis].x*orientation, unit_vectors[axis].y*orientation);

		int window = 5;

		double *m_l = (double*) malloc( (coordinates_width - window + 1)*sizeof(double));
		double *b_l = (double*) malloc( (coordinates_width - window + 1)*sizeof(double));


		//TODO Flip sign of slope based on orientation
		switch(axis)
		{
			case x:
				running_slope(m_l, b_l, full_profile, full_profile + 2*coordinates_width, coordinates_width, window);
				break;
			case y:
				running_slope(m_l, b_l, full_profile + coordinates_width, full_profile + 2*coordinates_width, coordinates_width, window);
				break;
			//TODO Scale by sqrt(2)
			case v:
				running_slope(m_l, b_l, full_profile + coordinates_width, full_profile + 2*coordinates_width, coordinates_width, window);
				break;
			//TODO Scale by sqrt(2)
			case w:
				running_slope(m_l, b_l, full_profile + coordinates_width, full_profile + 2*coordinates_width, coordinates_width, window);
				break;
		}

		//TODO We find all the slopes, and then we sift to find the max arg. This is inefficient. Just return max

		for (int i = 0; i < coordinates_width - window + 1; i++)
		{
			printf("%f\n", m_l[i]);
		}
	}
}
