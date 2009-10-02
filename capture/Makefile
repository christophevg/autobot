DEV=/dev/video0
CC=g++
CFLAGS=-W -Wall
LDFLAGS=

all: clean out.png

mock: out.rgb
	mkdir -p mock
	cp 0-out.rgb mock/0.rgb
	cp 1-out.rgb mock/1.rgb
	cp 2-out.rgb mock/2.rgb
	cp 3-out.rgb mock/3.rgb
	cp 4-out.rgb mock/4.rgb

%.mock: grab-frame.exe
	LD_LIBRARY_PATH=. mono $< out.rgb ./mock
	convert -size 320x240 -depth 8 rgb:0-out.rgb 0-out.png
	convert -size 320x240 -depth 8 rgb:1-out.rgb 1-out.png
	convert -size 320x240 -depth 8 rgb:2-out.rgb 2-out.png
	convert -size 320x240 -depth 8 rgb:3-out.rgb 3-out.png
	convert -size 320x240 -depth 8 rgb:4-out.rgb 4-out.png

%.png: %.rgb
	convert -size 320x240 -depth 8 rgb:0-$< 0-$@
	convert -size 320x240 -depth 8 rgb:1-$< 1-$@
	convert -size 320x240 -depth 8 rgb:2-$< 2-$@
	convert -size 320x240 -depth 8 rgb:3-$< 3-$@
	convert -size 320x240 -depth 8 rgb:4-$< 4-$@

%.rgb: grab-frame.exe 
	LD_LIBRARY_PATH=. mono $< $@ ${DEV}

grab-frame.exe: grab-frame.cs Project.Autobot.Capture.dll capture.so
	mcs -r:Project.Autobot.Capture.dll -out:$@ $<

Project.Autobot.Capture.dll: Project.Autobot.Capture.cs V4L2Device.cs
	mcs -target:library -out:$@ Project.Autobot.Capture.cs V4L2Device.cs

%.o: %.cc
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
