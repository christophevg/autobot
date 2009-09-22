#include "capture.h"

int main( int argc, char** argv ) {
  grab_frame( argc > 1 ? argv[1] : "out.raw", argc > 2 ? argv[2] : "/dev/video0" );
  return 0;
}
