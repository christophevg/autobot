#include "FrameStreamer.h"
#include "V4L2Device.h"

int main() {
	FrameStreamer* dev   = new V4L2Device("/dev/video0");
	unsigned char* frame = (unsigned char*)malloc( dev->getFrameSize() );

	dev->start();
	dev->getFrame(frame);
	dev->stop();

	FILE* out = fopen("test.rgb", "wb");
	fwrite(frame, 320 * 240 * 3, 1, out);
	fclose(out);
}
