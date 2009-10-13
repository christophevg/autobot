#include "V4L2Device.h"

int V4L2Device::xioctl(int request, void* arg) {
  int r;
  do r = ioctl( this->fd, request, arg );
  while( -1 == r && EINTR == errno );
  return r;
}

void V4L2Device::retrieveCapabilities() {
  if( !this->isOpen() ) { 
    cerr << "Can't get capabilities of " << this->name << endl;
    return; 
  }

  if( -1 == this->xioctl( VIDIOC_QUERYCAP, &this->cap ) ) { 
    cerr << "Couldn't get capabilities from " << this->name << "." << endl;
    CLEAR(this->cap);
  }
}

bool V4L2Device::can( __u32 capability ) {
  return this->hasCapabilities() && 
    ( this->cap.capabilities & capability );
}

bool V4L2Device::hasCapabilities() {
  if( !this->cap.capabilities ) {
    cerr << "Need to get capabilities first." << endl;
    return false;
  }
  return true;
}

void V4L2Device::setupMemoryMapping() {
  if( !this->hasFormatSet() ) {
    cerr << "Need to set format before setting up memory mapping." << endl;
    return;
  }

  CLEAR (this->req);

  this->req.count               = 4;
  this->req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  this->req.memory              = V4L2_MEMORY_MMAP;

  if( -1 == this->xioctl(VIDIOC_REQBUFS, &req) ) {
    if( EINVAL == errno ) {
      cerr << this->name << " does not support memory mapping" << endl;
    }
    CLEAR(this->req);
    return;
  }

  if( req.count < 2 ) {
    cerr << "Insufficient buffer memory on " << this->name << "." << endl;
    CLEAR(this->req);
    return;
  }
}

void V4L2Device::allocateBuffers() {
  if( !this->hasMappedMemory() ) {
    cerr << "Setup memory mapping before allocating buffers." << endl;
    return;
  }

  this->buffers = (buffer*)calloc(this->req.count, sizeof(*this->buffers));

  if( !this->buffers ) {
    cerr << "Out of memory" << endl;
    return;
  }

  for( this->n_buffers=0; this->n_buffers < this->req.count; 
  ++this->n_buffers ) 
  {
    struct v4l2_buffer buf;

    CLEAR(buf);
    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = this->n_buffers;

    if( -1 == this->xioctl(VIDIOC_QUERYBUF, &buf) ) {
      cerr << "Can't query buffer " << this->n_buffers << "." << endl;
      return;
    }

    this->buffers[this->n_buffers].length = buf.length;
    this->buffers[this->n_buffers].start =
      mmap( NULL /* start anywhere */,
      buf.length,
      PROT_READ | PROT_WRITE /* required */,
      MAP_SHARED /* recommended */,
      this->fd, buf.m.offset);

    if( MAP_FAILED == this->buffers[this->n_buffers].start ) {
      cerr << "Allocation of buffer " << this->n_buffers 
        << " failed." << endl;
      return;
    }
  }
}

void V4L2Device::mapMemory() {
  if( this->hasMappedMemory() ) {
    this->unmapMemory();
  }
  this->setupMemoryMapping();
  this->allocateBuffers();
}

bool V4L2Device::hasMappedMemory() {
  if( this->req.count < 1 ) {
    cerr << "Memory hasn't been mapped yet." << endl;
    return false;
  }
  return true;
}

bool V4L2Device::hasFormatSet() {
  if( this->fmt.fmt.pix.width < 1 ) {
    cerr << "Format hasn't been set." << endl;
    return false;
  }
  return true;
}

void V4L2Device::unmapMemory() {
  for( unsigned int i = 0; i < this->n_buffers; ++i ) {
    if( -1 == munmap(this->buffers[i].start, this->buffers[i].length) ) {
      cerr << "Could not unmap buffer " << i << "." << endl;
    }
  }
  free( this->buffers );
}

void V4L2Device::readFrame(unsigned char* image) {
  struct v4l2_buffer buf;

  CLEAR (buf);
  buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  if( -1 == this->xioctl(VIDIOC_DQBUF, &buf ) ) {
    cerr << "Can't prepare frame data." << endl;
    return;
  }

  assert(buf.index < n_buffers);

  if( -1 == this->xioctl(VIDIOC_QBUF, &buf)) {
    cerr << "Can't get frame data." << endl;
    return;
  }

  this->convertToRGB( (unsigned char*)buffers[buf.index].start, image );
}

void V4L2Device::convertToRGB( unsigned char* yuv, unsigned char* rgb ) {
  ::convert_yuv_to_rgb_buffer(yuv, rgb, FRAME_WIDTH, FRAME_HEIGHT);
}

V4L2Device::V4L2Device(string dev) {
  this->name = dev;

  this->fd   = -1;
  CLEAR(this->cap);
  CLEAR(this->fmt);

  this->buffers   = NULL;
  this->n_buffers = 0;

  this->capturingIsActive = false;
}

V4L2Device::~V4L2Device() {
  this->stop();
}

void V4L2Device::start() {
  this->open();
  this->init();
  this->startCapturing();
}

void V4L2Device::open() {
  if( this->isOpen() ) { 
    cerr << "Device is already open." << endl;
    return;
  }

  if( !this->isValidDevice() ) { 
    cerr << "Can't open " << this->name << ", it's no valid device." << endl;
    return; 
  }

  this->fd = ::open( this->name.c_str(), O_RDWR | O_NONBLOCK, 0 );

  this->retrieveCapabilities();
}

void V4L2Device::init() {
  this->setFormat( V4L2_BUF_TYPE_VIDEO_CAPTURE,
    FRAME_WIDTH, FRAME_HEIGHT,
    V4L2_PIX_FMT_YUYV,
    V4L2_FIELD_NONE );
}

void V4L2Device::setFormat( v4l2_buf_type type, __u32 width, __u32 height, 
                            __u32 format, v4l2_field field ) 
{
  if( !this->canCapture() ) {
    cerr << this->name << " can't capture video." << endl;
    return;
  }

  if( !this->canStream() ) {
    cerr << this->name << " can't stream video." << endl;
    return;
  }

  CLEAR(this->fmt);

  this->fmt.type                = type;
  this->fmt.fmt.pix.width       = width; 
  this->fmt.fmt.pix.height      = height;
  this->fmt.fmt.pix.pixelformat = format;
  this->fmt.fmt.pix.field       = field;

  if( -1 == this->xioctl(VIDIOC_S_FMT, &fmt) ) {
    cerr << "Can't set format." << endl;
    CLEAR(this->fmt);
    return;
  }

  this->mapMemory();
}

void V4L2Device::startCapturing() {
  if( this->isCapturing() ) { return; }

  for( unsigned int i = 0; i < this->n_buffers; ++i ) {
    struct v4l2_buffer buf;

    CLEAR (buf);
    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = i;

    if( -1 == this->xioctl(VIDIOC_QBUF, &buf) ) {
      cerr << "Can't query buffer " << i << "." << endl;
      return;
    }
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if( -1 == this->xioctl(VIDIOC_STREAMON, &type) ) {
    cerr << "Can't turn on streaming." << endl;
    return;
  }

  this->capturingIsActive = true;
}

void V4L2Device::getFrame( unsigned char* frame ) {
  fd_set fds;
  struct timeval tv;

  FD_ZERO(&fds);
  FD_SET(this->fd, &fds);

  tv.tv_sec  = 1;
  tv.tv_usec = 0;

  if( 0 == select( this->fd + 1, &fds, NULL, NULL, &tv ) ) {
    cerr << "Timeout while getting frame." << endl;
    return;
  }

  this->readFrame(frame);
}

void V4L2Device::stop() {
  this->stopCapturing();
  this->unmapMemory();
  ::close(this->fd);
}

void V4L2Device::stopCapturing() {
  if( !this->isCapturing() ) { return; }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if( -1 == this->xioctl(VIDIOC_STREAMOFF, &type) ) {
    cerr << "Can't turn off streaming." << endl;
    return;
  }

  this->capturingIsActive = false;
}

  // utility functions

bool V4L2Device::isOpen() {
  if( this->fd == -1 ) {
    cerr << this->name << " is not open." << endl;
    return false;
  }
  return true;
}

bool V4L2Device::isValidDevice() {
  struct stat st; 

  if( -1 == stat(this->name.c_str(), &st) ) {
    cerr << "Cannot identify file " << this->name << "." << endl;
    return false;
  }

  if( !S_ISCHR(st.st_mode) ) {
    cerr << this->name << " is no device file." << endl;
    return false;
  }

  return true;
}

bool V4L2Device::canCapture()  { return this->can( V4L2_CAP_VIDEO_CAPTURE ); }
bool V4L2Device::canStream()   { return this->can( V4L2_CAP_STREAMING );     }
bool V4L2Device::isCapturing() { return this->capturingIsActive;             }

int V4L2Device::getWidth()  { return FRAME_WIDTH;  }
int V4L2Device::getHeight() { return FRAME_HEIGHT; }
int V4L2Device::getColors() { return FRAME_COLORS; }

int V4L2Device::getFrameSize() { 
  return this->getWidth() * this->getHeight() * this->getColors();
}

// C interface
extern "C" V4L2Device* V4L2Device_new(const char* name) {
  return new V4L2Device(name);
}

extern "C" void V4L2Device_start(V4L2Device* dev) {
  return dev->start();
}

extern "C" void V4L2Device_getFrame(V4L2Device* dev, unsigned char* frame) {
  return dev->getFrame( frame );
}

extern "C" void V4L2Device_stop(V4L2Device* dev) {
  return dev->stop();
}

extern "C" int V4L2Device_getHeight(V4L2Device* dev) {
  return dev->getHeight();
}

extern "C" int V4L2Device_getWidth(V4L2Device* dev) {
  return dev->getWidth();
}

extern "C" int V4L2Device_getColors(V4L2Device* dev) {
  return dev->getColors();
}

extern "C" int V4L2Device_getFrameSize(V4L2Device* dev) {
  return dev->getFrameSize();
}
