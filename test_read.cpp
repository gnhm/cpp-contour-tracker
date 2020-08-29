#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"contour_analyzer_lib.h"


int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		printf("%s\n", argv[i]);
		struct Contour cs;
		load_contour(argv[i], &cs);
		analyze_contour(&cs);
	}
	return 0;
}
