#include "cdjpeg.h"

static const char* progname;
static const char* infilename;

int main( int argc, char **argv ) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  FILE * input_file;
  JSAMPARRAY buffer;
  int row_stride;
  
  progname = argv[0];

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  if( argc < 2 ) {
    fprintf(stderr, "%s: must name one input JPEG file\n", progname);
    fprintf(stderr, "usage: %s ", progname);
    fprintf(stderr, "inputfile\n");
    exit(EXIT_FAILURE);
  }
  infilename  = argv[1];
  if ((input_file = fopen(infilename, READ_BINARY)) == NULL) {
      fprintf(stderr, "%s: can't open %s\n", progname, infilename);
      exit(EXIT_FAILURE);
  }

  jpeg_stdio_src( &cinfo, input_file );
  jpeg_read_header( &cinfo, TRUE );
  jpeg_start_decompress(&cinfo);

  row_stride = cinfo.output_width * cinfo.output_components;
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  printf( "%d x %d (%d components)\n", 
	  cinfo.output_width, cinfo.output_height, cinfo.output_components );

  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines( &cinfo, buffer, 1 );
    int c = 0;
    for( c=0; c<row_stride/3; c++ ) {
      printf( "%3d %3d %3d - ", 
	      buffer[0][c*3], 
	      buffer[0][c*3+1], 
	      buffer[0][c*3+2] );
    }
    printf( "\n" );
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  fclose(input_file);

  exit(jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
  return 0;
}
