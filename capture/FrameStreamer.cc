#include "FrameStreamer.h"

FrameDecorator::FrameDecorator(FrameStreamer* fs) {
  this->fs = fs;
}

int FrameDecorator::getFrameSize() {
  return this->fs->getFrameSize();
}

void FrameDecorator::start() {
  this->fs->start();
}

void FrameDecorator::getFrame( unsigned char* frame ) {
  this->fs->getFrame(frame);
  this->postProcessFrame(frame);
}

void FrameDecorator::stop() {
  this->fs->stop();
}

void EchoFrameDecorator::postProcessFrame( unsigned char* frame ) {
  // do nothing ;-)
}
