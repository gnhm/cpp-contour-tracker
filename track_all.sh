#!/bin/bash

SUB='bad'
SUB2='screen'

for d in $1/auto_flickering*;
	do for m in $d/*.movie;
		do if [[ ($m != *"$SUB"*) && ($m != *"$SUB2"*) ]]; then $HOME/cpp-contour-tracker/track_movie $m; fi
done;
done;
