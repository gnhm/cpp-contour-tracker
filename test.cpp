#include "temika_header.h"
#include "get_movie_frame.h"


int main (int argc, char **argv) {
  
  char* moviefile = argv[1];
  get_frame(moviefile, struct camera_frame_struct *temika_frame) // (FILE *moviefile, struct camera_frame_struct *temika_frame) ;
  
  
  
}
