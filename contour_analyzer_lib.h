#include <cmath>
#include <math.h>

#define NM_PER_PX 97.6

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
