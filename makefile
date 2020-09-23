all: contour_tracker.cpp
	g++ -std=c++11 contour_tracker.cpp -o contour_tracker

movie: test_movie.cpp contour_analyzer_lib.h
	g++ -std=c++11 test_movie.cpp -o test_movie

