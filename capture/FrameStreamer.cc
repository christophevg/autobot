#include "FrameStreamer.h"

FrameDecorator::FrameDecorator(FrameStreamer* fs) {
  this->fs = fs;
}

// some pass-through functions
int FrameDecorator::getWidth()     { return this->fs->getWidth();     }
int FrameDecorator::getHeight()    { return this->fs->getHeight();    }
int FrameDecorator::getColors()    { return this->fs->getColors();    }
int FrameDecorator::getFrameSize() { return this->fs->getFrameSize(); }

void FrameDecorator::start()       {  this->fs->start();              }
void FrameDecorator::stop()        { this->fs->stop();                }

// pass-through and decorate
void FrameDecorator::getFrame( unsigned char* frame ) {
  this->fs->getFrame(frame);
  this->postProcessFrame(frame);
}

// sample decorator, doing nothing
NullFrameDecorator::NullFrameDecorator(FrameStreamer* fs)
  : FrameDecorator(fs)
{
    // nothing else todo, just passing on ;-)
};

void NullFrameDecorator::postProcessFrame( unsigned char* ) {
  // doing NULL
}
