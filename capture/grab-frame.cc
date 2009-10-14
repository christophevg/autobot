#include "FrameStreamer.h"
#include "YUV2RGB.h"
#include "V4L2Device.h"

int main() {
  FrameStreamer* yuv = new V4L2Device("/dev/video0"); // the source
  FrameStreamer* rgb = new YUV2RGB( yuv );            // we want RGB
  FrameStreamer* dev = new NullFrameDecorator( rgb ); // demo

  unsigned char* frame = (unsigned char*)malloc( dev->getFrameSize() );

	dev->start();
	dev->getFrame(frame);
	dev->stop();

	FILE* out = fopen("test.rgb", "wb");
	fwrite(frame, 320 * 240 * 3, 1, out);
	fclose(out);
}
