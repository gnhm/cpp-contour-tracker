# to pull from remote repo, delete the old bash executable, and compile the new imported cpp file
rm track_movie # if it exists
git pull

g++ -o track_movie track_movie.cpp

