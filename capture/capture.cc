#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int deviceHandle = 0;
int i; // temporary counter
struct video_capability capability;
struct video_channel queryChannel;
struct video_channel selectedChannel;
struct video_window captureWindow;
struct video_picture imageProperties;
struct video_mbuf memoryBuffer;
struct video_mmap* mmaps;
char* memoryMap;
int channelNumber = 0;
int width = 320;
int height = 240;
int depth;
int palette;
int bufferIndex;

char* NextFrame() {
  // send a request to begin capturing to the currently indexed buffer
  if (ioctl (deviceHandle, VIDIOCMCAPTURE, &mmaps[bufferIndex]) == -1) {
    // capture request failed
    printf( "capture request failed\n" );
    exit( -1 );
  }
  
  // move bufferIndex to the next frame
  ++bufferIndex;
  if( bufferIndex == memoryBuffer.frames ) {
    // bufferIndex is indexing beyond the last buffer
    // set it to index the first buffer
    bufferIndex = 0;
  }
  
  // wait for the currently indexed frame to complete capture
  if (ioctl (deviceHandle, VIDIOCSYNC, &mmaps[bufferIndex]) == -1) {
    // sync request failed
    printf( "Sync request failed\n" );
    exit( -1 );
  }
  
  // return the address of the frame data for the current buffer index
  return( memoryMap + memoryBuffer.offsets[bufferIndex] );
}

int main() {
  // open the device
  deviceHandle = open( "/dev/video0", O_RDWR );
  if( deviceHandle == -1 ) {
    printf( "Could not open device\n" );
    return -1;
  }
  
  if( ioctl( deviceHandle, VIDIOCGCAP, &capability ) == -1 ) {
    printf ("could not obtain device capabilities\n");
    return -1;
  }
  if( (capability.type & VID_TYPE_CAPTURE) == 0 ) {
    printf ("this device cannot capture video to memory\n");
    return -1;
  }
  
  if( capability.channels > 1 ) {
    printf ("Select a channel:\n");
    i = 0;
    while( i < capability.channels ) {
      queryChannel.channel = i;
      if( ioctl( deviceHandle, VIDIOCGCHAN, &queryChannel ) != -1) {
	// ioctl success, queryChannel contains information about this channel
	printf( "%d. %s\n", queryChannel.channel, queryChannel.name );
      }
      ++ i;
    }
    printf( ": ");
    fflush( stdout);
    scanf( "%d", &channelNumber);
  } else {
    channelNumber = 0;
  }
  // set the selected channel
  selectedChannel.channel = channelNumber;
  selectedChannel.norm = VIDEO_MODE_NTSC;
  if (ioctl (deviceHandle, VIDIOCSCHAN, &selectedChannel) == -1) {
    // could not set the selected channel
    printf ("Could not set channel #%d\nNot a fatal error.", channelNumber);
  }
  
  if( ioctl( deviceHandle, VIDIOCGWIN, &captureWindow ) == -1 ) {
    printf ("Could not obtain capture window dimensions.\n");
  }
  width = captureWindow.width;
  height = captureWindow.height;
  printf ("Capturing dimensions are : %d, %d\n", width, height);
  
  /*
  // get image properties
  if (ioctl (deviceHandle, VIDIOCGPICT, &imageProperties) != -1) {
    // successfully retrieved the default image properties
    
    // the following values are for requesting 24bit RGB
    imageProperties.depth = 24;
    imageProperties.palette = VIDEO_PALETTE_RGB24;
    if (ioctl (deviceHandle, VIDIOCSPICT, &imageProperties) == -1) {
      // failed to set the image properties
      printf ("Could not set the video depth and palette.\nPerhaps not a fatal error.\n");
    }
  }
  */
  
  // verify that the requested capture pixel depth and palette succeeded
  if( ioctl( deviceHandle, VIDIOCGPICT, &imageProperties ) == -1 ) {
    printf ("Failed to retrieve the video depth and palette.\n");
    return -1;
  }
  depth = imageProperties.depth;
  palette = imageProperties.palette;
  printf( "Format is: depth=%d palette=%d\n", depth, palette );

  if( ioctl( deviceHandle, VIDIOCGMBUF, &memoryBuffer ) == -1 ) {
    printf ("Failed to retrieve information about MMIO space.\n");
    return -1;
  }
  
  // obtain memory mapped area
  memoryMap = (char*)mmap( 0, memoryBuffer.size, PROT_READ 
			   | PROT_WRITE, MAP_SHARED, deviceHandle, 0 );
  if( (int)memoryMap == -1 ) {
    printf ("Failed to obtain MMIO space.\n");
    return -1;
  }
  
  // allocate structures
  mmaps = (struct video_mmap*)(malloc( memoryBuffer.frames 
				       * sizeof (struct video_mmap)) );
  
  // fill out the fields
  i = 0;
  while( i < memoryBuffer.frames ) {
    mmaps[i].frame = i;
    mmaps[i].width = width;
    mmaps[i].height = height;
    mmaps[i].format = palette;
    ++ i;
  }
  
  // request capture to each buffer except the last one
  i = 0;
  while( i < ( memoryBuffer.frames-1 ) ) {
    if( ioctl( deviceHandle, VIDIOCMCAPTURE, &mmaps[i] ) == -1 ) {
      // capture request failed
    }
    ++i;
  }
  
  // set our index to the last buffer
  bufferIndex = memoryBuffer.frames-1;
  
  // capture and write out one frames
  printf( "Capture is ready; capturing 1 image.\n" );
  char* frame = NextFrame();
  
  i = 0;
  
  // write out PPM file
  char fname[80];
  sprintf (fname, "output%02d.ppm", i);
  printf ("Writing out PPM file %s\n", fname);
  
  FILE* fp;
  if( (fp = fopen(fname, "w")) == NULL ) {
    printf ("Could not open file %s for writing.\n", fname);
    return -1;
  }
  
  fprintf( fp, "P6\n%d %d\n255\n", width, height );
  
  int n = width * height;
  
  for( int index=0; index < n; ++index ) {
    putc( frame[index], fp );
  }
  
  fflush (fp);
  fclose (fp);
  
  // free the video_mmap structures
  free (mmaps);
  
  // unmap the capture memory
  munmap (memoryMap, memoryBuffer.size);
  
  // close the device
  close (deviceHandle);
  
  return 0;
}
