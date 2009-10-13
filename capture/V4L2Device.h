#ifndef __V4L2DEVICE_H
#define __V4L2DEVICE_H

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

#include "FrameStreamer.h"
#include "yuv2rgb.h"

#include <string>
#include <iostream>

#include "FrameStreamer.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

const int FRAME_WIDTH  = 320;
const int FRAME_HEIGHT = 240;
const int FRAME_COLORS = 3;

using namespace std;

struct buffer {
	void*  start;
	size_t length;
};

class V4L2Device : public FrameStreamer { 
private:
  string name;
	int    fd;

	struct v4l2_capability cap;
	struct v4l2_format     fmt;

	struct v4l2_requestbuffers req;

	struct buffer* buffers;
	unsigned int   n_buffers;

	bool capturingIsActive;
	
  int xioctl(int request, void* arg);
  void retrieveCapabilities();
  bool can( __u32 capability );
  bool hasCapabilities();
  void setupMemoryMapping();
  void allocateBuffers();
  void mapMemory();
  bool hasMappedMemory();
  bool hasFormatSet();
  void unmapMemory();
  void readFrame(unsigned char* image);
  void convertToRGB( unsigned char* yuv, unsigned char* rgb );
	
public:
  V4L2Device(string dev);
  ~V4L2Device();
  
  void start();
  void open();
  void init();
  void setFormat( v4l2_buf_type type, __u32 width, __u32 height, 
                  __u32 format, v4l2_field field );
  void startCapturing();
  void getFrame( unsigned char* frame ); 
  void stop();
  void stopCapturing();
  
  bool isOpen();
  bool isValidDevice();
  bool canCapture();
  bool canStream();
  bool isCapturing();

  int  getWidth();
  int  getHeight();
  int  getColors();
  int  getFrameSize();
};

#endif
