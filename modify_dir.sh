# to pull from remote repo, delete the old bash executable, and compile the new imported cpp file
rm track_movie 
rm track_contours
git pull

g++ -o track_movie track_movie.cpp
g++ -o track_contours track_contours.cpp
