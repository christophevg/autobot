#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>

#include "yuv2rgb.h"
#include "capture.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
  void*  start;
  size_t length;
};

char*           dev_name        = NULL;
int             fd              = -1;
struct buffer*  buffers         = NULL;
unsigned int    n_buffers       = 0;

void errno_exit(const char* s) {
  fprintf (stderr, "%s error %d, %s\n", s, errno, strerror (errno));
  exit(EXIT_FAILURE);
}

int xioctl(int fd, int request, void* arg) {
  int r;
  
  do r = ioctl( fd, request, arg );
  while (-1 == r && EINTR == errno);
  
  return r;
}

void process_image( char* filename, unsigned char* yuv ) {
  FILE* out;
  if( (out = fopen(filename, "wb")) == NULL ) {
    printf ("Could not open file for writing.\n");
  }
  
  unsigned char* rgb = malloc( 320 * 240 * 3 );
  convert_yuv_to_rgb_buffer(yuv, rgb, 320, 240);
  
  fwrite(rgb, 320 * 240 * 3, 1, out);
  fclose(out);
}

int read_frame( char* filename ) {
  struct v4l2_buffer buf;
  
  CLEAR (buf);
  
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  
  if( -1 == xioctl( fd, VIDIOC_DQBUF, &buf ) ) {
    switch (errno) {
    case EAGAIN:
      return 0;
      
    case EIO:
      /* Could ignore EIO, see spec. */
      
      /* fall through */
      
    default:
      errno_exit ("VIDIOC_DQBUF");
    }
  }
  
  assert(buf.index < n_buffers);
  
  process_image( filename, buffers[buf.index].start );
  
  if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
    errno_exit ("VIDIOC_QBUF");
  
  return 1;
}

void fetch_frame(char* filename) {
  fd_set fds;
  struct timeval tv;
  int r;
  
  FD_ZERO (&fds);
  FD_SET (fd, &fds);
  
  /* Timeout: needed to get image from webcam */
  tv.tv_sec  = 1;
  tv.tv_usec = 0;
  
  r = select( fd + 1, &fds, NULL, NULL, &tv );
  
  if( 0 == r ) {
    fprintf (stderr, "select timeout\n");
    exit (EXIT_FAILURE);
  }
  
  read_frame(filename);
}

void stop_capturing() {
  enum v4l2_buf_type type;
  
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  
  if( -1 == xioctl (fd, VIDIOC_STREAMOFF, &type) ) {
    errno_exit ("VIDIOC_STREAMOFF");
  }
}

void start_capturing() {
  unsigned int i;
  enum v4l2_buf_type type;

  for( i = 0; i < n_buffers; ++i ) {
    struct v4l2_buffer buf;
    
    CLEAR (buf);
    
    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = i;
    
    if( -1 == xioctl (fd, VIDIOC_QBUF, &buf) ) {
      errno_exit ("VIDIOC_QBUF");
    }
  }
  
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  
  if( -1 == xioctl (fd, VIDIOC_STREAMON, &type) ) {
    errno_exit ("VIDIOC_STREAMON");
  }
}

void uninit_device() {
  unsigned int i;
  
  for( i = 0; i < n_buffers; ++i ) {
    if( -1 == munmap (buffers[i].start, buffers[i].length) ) {
      errno_exit ("munmap");
    }
  }

  free( buffers );
}

void init_mmap() {
  struct v4l2_requestbuffers req;
  
  CLEAR (req);
  
  req.count               = 4;
  req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory              = V4L2_MEMORY_MMAP;
  
  if( -1 == xioctl (fd, VIDIOC_REQBUFS, &req) ) {
    if( EINVAL == errno ) {
      fprintf(stderr, "%s does not support memory mapping\n", dev_name);
      exit(EXIT_FAILURE);
    } else {
      errno_exit("VIDIOC_REQBUFS");
    }
  }
  
  if( req.count < 2 ) {
    fprintf( stderr, "Insufficient buffer memory on %s\n", dev_name );
    exit(EXIT_FAILURE);
  }
  
  buffers = calloc(req.count, sizeof (*buffers));
  
  if( !buffers ) {
    fprintf( stderr, "Out of memory\n" );
    exit(EXIT_FAILURE);
  }
  
  for( n_buffers = 0; n_buffers < req.count; ++n_buffers ) {
    struct v4l2_buffer buf;

    CLEAR (buf);
    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = n_buffers;
    
    if( -1 == xioctl (fd, VIDIOC_QUERYBUF, &buf) ) {
      errno_exit ("VIDIOC_QUERYBUF");
    }
    
    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start =
      mmap (NULL /* start anywhere */,
	    buf.length,
	    PROT_READ | PROT_WRITE /* required */,
	    MAP_SHARED /* recommended */,
	    fd, buf.m.offset);
    
    if( MAP_FAILED == buffers[n_buffers].start ) {
      errno_exit("mmap");
    }
  }
}

void init_device() {
  struct v4l2_capability cap;
  struct v4l2_format fmt;
  
  if( -1 == xioctl (fd, VIDIOC_QUERYCAP, &cap) ) {
    if( EINVAL == errno ) {
      fprintf (stderr, "%s is no V4L2 device\n", dev_name);
      exit (EXIT_FAILURE);
    } else {
      errno_exit ("VIDIOC_QUERYCAP");
    }
  }
  
  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf (stderr, "%s is no video capture device\n", dev_name);
    exit (EXIT_FAILURE);
  }
  
  if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    fprintf (stderr, "%s does not support streaming i/o\n", dev_name);
    exit (EXIT_FAILURE);
  }
  
  /* Select video input, video standard and tune here. */
  
  CLEAR (fmt);
  
  fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width       = 320; 
  fmt.fmt.pix.height      = 240;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field       = V4L2_FIELD_NONE;
  
  if( -1 == xioctl (fd, VIDIOC_S_FMT, &fmt) ) {
    errno_exit( "VIDIOC_S_FMT" );
  }
  
  init_mmap ();
}

void close_device() {
  if( -1 == close(fd) ) {
    errno_exit("close");
  }
  fd = -1;
}

void open_device( char* dev ) {
  struct stat st; 

  dev_name = dev;

  if( -1 == stat(dev_name, &st) ) {
    fprintf (stderr, "Cannot identify '%s': %d, %s\n",
	     dev_name, errno, strerror (errno));
    exit(EXIT_FAILURE);
  }
  
  if( !S_ISCHR(st.st_mode) ) {
    fprintf( stderr, "%s is no device\n", dev_name);
    exit(EXIT_FAILURE);
  }
  
  fd = open( dev_name, O_RDWR /* required */ | O_NONBLOCK, 0 );

  if( -1 == fd ) {
    fprintf( stderr, "Cannot open '%s': %d, %s\n",
	     dev_name, errno, strerror (errno));
    exit(EXIT_FAILURE);
  }
}

void grab_frame( char* filename, char* device ) {
  open_device( device );
  init_device();
  start_capturing();
  fetch_frame( filename );
  stop_capturing();
  uninit_device();
  close_device();
}
