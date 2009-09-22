DEV=/dev/video0
CC=gcc
CFLAGS=-W -Wall
LDFLAGS=

all: clean out.png

%.png: %.rgb
	convert -size 320x240 -depth 8 rgb:$< $@

%.rgb: grab-frame
	./grab-frame $@ ${DEV}

grab-frame: capture.so grab-frame.c
	${CC} grab-frame.c -o $@ -L. -lcapture

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

capture.so: capture.o yuv2rgb.o capture.a
	${CC} -shared -Wl,-soname,lib$@.1 -o lib$@.1.0.1 capture.o yuv2rgb.o

capture.a: capture.o yuv2rgb.o
	ar rcs lib$@ capture.o yuv2rgb.o

formats: v4l2-formats
	./v4l2-formats ${DEV}

v4l2-formats: v4l2-formats.c
	${CC} -o $@ $<

clean:
	@rm -f *~ *.o *.a *.so*
	@rm -f *.raw *.rgb *.png
	@rm -f a.out v4l2-formats capture
	@rm -f grab-frame
