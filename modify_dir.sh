# to pull from remote repo, delete the old bash executable, and compile the new imported cpp file
rm track_movie 
rm track_contours
git pull
echo "pulled"

g++ -o track_movie track_movie.cpp
echo "track_movie compiled"

g++ -o track_contours track_contours.cpp
echo "track_contours compiled"
