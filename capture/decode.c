#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
#include "jversion.h"		/* for version message */

static const char * progname;	/* program name for error messages */
static char * infilename;	/* for -outfile switch */
static char * outfilename;	/* for -outfile switch */

int main( int argc, char **argv ) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  djpeg_dest_ptr dest_mgr = NULL;
  FILE * input_file;
  FILE * output_file;
  JDIMENSION num_scanlines;
  
  progname = argv[0];

  /* Initialize the JPEG decompression object with default error handling. */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  /* get in- and output filenames */
  if( argc < 3 ) {
    fprintf(stderr, "%s: must name one input and one output file\n", progname);
    fprintf(stderr, "usage: %s ", progname);
    fprintf(stderr, "inputfile outputfile\n");
    exit(EXIT_FAILURE);
  }
  infilename  = argv[1];
  outfilename = argv[2];
  if ((input_file = fopen(infilename, READ_BINARY)) == NULL) {
      fprintf(stderr, "%s: can't open %s\n", progname, infilename);
      exit(EXIT_FAILURE);
  }
  if ((output_file = fopen(outfilename, WRITE_BINARY)) == NULL) {
    fprintf(stderr, "%s: can't open %s\n", progname, outfilename);
    exit(EXIT_FAILURE);
  }

  /* Specify data source for decompression */
  jpeg_stdio_src(&cinfo, input_file);

  /* Read file header, set default decompression parameters */
  (void) jpeg_read_header(&cinfo, TRUE);

  /* force to ppm format, and replace afterwards by our own code */
  dest_mgr = jinit_write_ppm(&cinfo);
  dest_mgr->output_file = output_file;

  /* Start decompressor */
  (void) jpeg_start_decompress(&cinfo);
  (*dest_mgr->start_output) (&cinfo, dest_mgr);

  while (cinfo.output_scanline < cinfo.output_height) {
    num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
					dest_mgr->buffer_height);
    (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
  }

  (*dest_mgr->finish_output) (&cinfo, dest_mgr);
  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  if (input_file != stdin)   fclose(input_file);
  if (output_file != stdout) fclose(output_file);

  exit(jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
  return 0;
}
