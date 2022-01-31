README for jamie branch. Aim: produce the text file contour as in Guil's sample. NB currently much easier to work with c++ on my own laptop as windows is annoying
(and no visual studio etc)


Initial thoughts: temika_header probably not needed for local work, not sure what 
the makefile and the shell script are for. It seems the `trackmovie` libs are used in `track_movie.cpp`. 
Remember all .h files are "headers" (and therefore don't contain any actions?) and .cpp are "source files".
Also the trackmovie libs seem to be more general versions of the non-trackmovie versions (check this)

Will need to isolate modules from actual scripts that are running something, just as in the python case

Good chance to get back into c++!

Not sure (intuitively) what is the difference between track_contours and track_movie... surely the contours have to be tracked over a few frames? Unless the first one is setting up the (multiple for multiple modes) contours in a single frame...
