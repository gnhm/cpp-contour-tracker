void running_slope(double *m_l, double *b_l, double *x, double *y, int n, int window)
{
	double x_m = 0;
	double y_m = 0;
	double s_xx = 0;
	double s_xy = 0;

	for (int i = 0; i < window; i++)
	{
		x_m += x[i];
		y_m += y[i];
		s_xx += x[i]*x[i];
		s_xy += x[i]*y[i];
	}

	x_m /= (double) window;
	y_m /= (double) window;

	s_xx /= (double) window;
	s_xx -= x_m*x_m;


	s_xy /= (double) window;
	s_xy -= x_m*y_m;

	m_l[0] = s_xy/s_xx;
	b_l[0] = y_m - (s_xy/s_xx)*x_m;

	double d_xx;
	double d_xy;
	double e_x;
	double e_y;
	for (int i = 0; i < n - window; i++)
	{
		d_xx = (x[i + window]*x[i + window] - x[i]*x[i])/((float) window);
		d_xy = (x[i + window]*y[i + window] - x[i]*y[i])/((float) window);
		e_x = (x[i + window] - x[i])/((float) window);
		e_y = (y[i + window] - y[i])/ ((float) window);

		s_xx += d_xx - 2*e_x*x_m - e_x*e_x;
		s_xy += d_xy - e_x*y_m - e_y*x_m - e_x*e_y;

		x_m += e_x;
		y_m += e_y;

		m_l[i+1] = s_xy/s_xx;
		b_l[i+1] = y_m - m_l[i+1]*x_m;
	}
}

void get_maximum_slope(double *slope_intercept, double *x, double *y, int n, int window, int orientation)
{
	double m_l[n - window + 1];
	double b_l[n - window + 1];
	running_slope(m_l, b_l, x, y, n, window);
	slope_intercept[0] = m_l[0];
	slope_intercept[1] = b_l[0];
	for (int i = 0; i < n - window; i++)
	{
		if (((orientation == 1) && (m_l[i] < slope_intercept[0])) || ((orientation == -1) && (m_l[i] > slope_intercept[0])))
//		if (m_l[i] < slope_intercept[0])
		{
			slope_intercept[0] = m_l[i];
			slope_intercept[1] = b_l[i];
		}
	}

}

void get_maximum_slope_broken(double *slope_intercept, double *x, double *y, int n, int window, int orientation)
{
	double x_m = 0;
	double y_m = 0;
	double s_xx = 0;
	double s_xy = 0;

	for (int i = 0; i < window; i++)
	{
		x_m += x[i];
		y_m += y[i];
		s_xx += x[i]*x[i];
		s_xy += x[i]*y[i];
	}

	x_m /= (double) window;
	y_m /= (double) window;

	s_xx /= (double) window;
	s_xx -= x_m*x_m;


	s_xy /= (double) window;
	s_xy -= x_m*y_m;

	slope_intercept[0] = s_xy/s_xx;
	slope_intercept[1] = y_m - (s_xy/s_xx)*x_m;

	double d_xx;
	double d_xy;
	double e_x;
	double e_y;
	for (int i = 0; i < n - window; i++)
	{
		d_xx = (x[i + window]*x[i + window] - x[i]*x[i])/((float) window);
		d_xy = (x[i + window]*y[i + window] - x[i]*y[i])/((float) window);
		e_x = (x[i + window] - x[i])/((float) window);
		e_y = (y[i + window] - y[i])/ ((float) window);

		s_xx += d_xx - 2*e_x*x_m - e_x*e_x;
		s_xy += d_xy - e_x*y_m - e_y*x_m - e_x*e_y;

		x_m += e_x;
		y_m += e_y;


		if ( ( (s_xy/s_xx < slope_intercept[0]) && orientation == 1) || ( (s_xy/s_xx > slope_intercept[0]) && orientation == -1) )
		{
			slope_intercept[0] = s_xy/s_xx;
			slope_intercept[1] = y_m - (s_xy/s_xx)*x_m;
		}

	}
}
