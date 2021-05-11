#!/bin/bash

for d in $1/auto_flickering*;
	do for m in $d/cell_*.movie;
		do echo $m;
		#/home/gn299/cpp-contour-tracker/track_movie $m;
		$HOME/cpp-contour-tracker/track_movie $m;
done;
done;
