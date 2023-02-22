#include "temika_header.h"
#include "get_movie_frame.h"


int main (int argc, char **argv) {
  
  char* moviefile = argv[1];
  struct camera_frame_struct *temika_frame;
  get_frame(moviefile, *temika_frame) // (FILE *moviefile, struct camera_frame_struct *temika_frame) ;
  
  
  
}
