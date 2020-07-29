#include <math.h>
#include <assert.h>

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

	/* TODO We want this function to return or modify x-coordinates, y-coordinates, and profile.
	 * In Python, easiest way is to return a 2-d numpy array. In C I'm not so sure.
	 * Perhaps easiest thing is to have a void function that modifies a 2-d array, and a "length" int.
	 * Additionally, horizontal_window determines the maximum length of the profile, so (TODO) it should 
	 * not be buried as an optional parameter, but defined outside of it. */
	/* TODO Finish this function!!! */
	int profile(double *image, int rows, int cols, double *full_profile, Vector c, axes axis, int horizontal_window)
	{
		int coordinates_width;

		/*This very complicated switch-case structure is just for defining the different axes along which to take the profile.
		 * 99% of the complexity is truncating the profile at the boundaries of the image. */

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
						full_profile[i*3] = i + start;
						full_profile[i*3 + 1] = c.y;
						full_profile[i*3 + 2] = image[i + start + ((int) c.y)*rows];

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
						full_profile[i*3] = c.x;
						full_profile[i*3 + 1] = i + start;
						full_profile[i*3 + 2] = image[((int) c.x) + (i + start)*rows];

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
						full_profile[i*3] = i + start_x;
						full_profile[i*3 + 1] = i + start_y;
						full_profile[i*3 + 2] = image[i + start_x + (i + start_y)*rows];
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
						full_profile[i*3] = start_x - i;
						full_profile[i*3 + 1] = i + start_y;
						full_profile[i*3 + 2] = image[start_x - i + (i + start_y)*rows];
					}	
					return coordinates_width;
					break;
				}
		}

		return -1;
	}
}
