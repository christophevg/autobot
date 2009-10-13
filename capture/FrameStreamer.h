#ifndef __FRAMESTREAMER_H
#define __FRAMESTREAMER_H

/* 
 * This is the FrameStreamer interface. It is implemened by the V4L2Device,
 * which produces the actual frames, but also by the FrameDecorators that 
 * allow to manipulate the frames, before they reach the end-user. 
 */
class FrameStreamer { 
public:
  virtual int  getFrameSize() = 0;
  virtual void start() = 0;
  virtual void getFrame( unsigned char* frame ) = 0; 
  virtual void stop() = 0;
};

/*
 * FrameDecorator is an abstract base class for decorators, which already
 * implements the basic decorator logic and defines one abstract method that
 * the actual decorators will have to implement: processFrame
 */
class FrameDecorator : public FrameStreamer {
private:
  FrameStreamer* fs;

protected:
  virtual void postProcessFrame( unsigned char* frame ) = 0;
  
public:    
  FrameDecorator(FrameStreamer* fs);
  int  getFrameSize();
  void start();
  void getFrame( unsigned char* frame ); 
  void stop();
};

class NullFrameDecorator : public FrameDecorator {
public:
  NullFrameDecorator(FrameStreamer* fs);
protected:
  void postProcessFrame( unsigned char* frame );
};

#endif
