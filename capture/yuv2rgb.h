#ifndef __YUV2RGB_H
#define __YUV2RGB_H

#include "FrameStreamer.h"

class YUV2RGB : public FrameDecorator {
private:
  void convert_yuv_to_rgb_buffer(unsigned char* in, unsigned char* out);
  int  convert_yuv_to_rgb_pixel(int y, int u, int v);
public:
  YUV2RGB(FrameStreamer* fs);
  int getFrameSize();
protected:
  void postProcessFrame( unsigned char* frame );
};

#endif
