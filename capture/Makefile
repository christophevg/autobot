DEV=/dev/video0
CC=gcc
CFLAGS=-W -Wall
LDFLAGS=

all: clean out.png

%.png: %.rgb
	convert -size 320x240 -depth 8 rgb:0-$< 0-$@
	convert -size 320x240 -depth 8 rgb:1-$< 1-$@
	convert -size 320x240 -depth 8 rgb:2-$< 2-$@
	convert -size 320x240 -depth 8 rgb:3-$< 3-$@
	convert -size 320x240 -depth 8 rgb:4-$< 4-$@

%.rgb: grab-frame.exe capture.so
	LD_LIBRARY_PATH=. mono $< $@ ${DEV}

grab-frame.exe: grab-frame.cs Project.Autobot.Capture.dll
	mcs -r:Project.Autobot.Capture.dll -out:$@ $<

Project.Autobot.Capture.dll: Project.Autobot.Capture.cs
	mcs -target:library -out:$@ $<

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

capture.so: capture.o yuv2rgb.o capture.a
	${CC} -shared -Wl,-soname,lib$@.1 -o lib$@.1.0.0 capture.o yuv2rgb.o

capture.a: capture.o yuv2rgb.o
	ar rcs lib$@ capture.o yuv2rgb.o

formats: v4l2-formats
	./v4l2-formats ${DEV}

v4l2-formats: v4l2-formats.c
	${CC} -o $@ $<

clean:
	@rm -f *~ *.o *.a *.so* *.exe *.dll
	@rm -f *.raw *.rgb *.png
	@rm -f v4l2-formats
