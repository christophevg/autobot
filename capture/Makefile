DEV=/dev/video0
CC=g++
CFLAGS=-W -Wall
LDFLAGS=
LIBOBJS=V4L2Device.o FrameStreamer.o YUV2RGB.o

all: clean out.png

mock: 0.rgb 1.rgb 2.rgb 3.rgb 4.rgb
	mkdir -p $@
	mv [0-4].rgb $@

%.png: %.rgb
	convert -size 320x240 -depth 8 rgb:$< $@

%.rgb: grab-frame.exe 
	LD_LIBRARY_PATH=. mono $< $@ ${DEV}

%.o: %.cc
	${CC} ${CFLAGS} -c $< -o $@

grab-frame: grab-frame.o ${LIBOBJS}
	${CC} -o $@ grab-frame.o ${LIBOBJS}

capture.so: ${LIBOBJS} capture.a
	${CC} -shared -Wl,-soname,lib$@.1 -o lib$@.1.0.0 ${LIBOBJS}

capture.a: ${LIBOBJS}
	ar rcs lib$@ ${LIBOBJS}

grab-frame.exe: grab-frame.cs Project.Autobot.Capture.dll capture.so
	mcs -r:Project.Autobot.Capture.dll -out:$@ $<

Project.Autobot.Capture.dll: Project.Autobot.Capture.cs V4L2Device.cs
	mcs -target:library -out:$@ Project.Autobot.Capture.cs V4L2Device.cs

formats: v4l2-formats
	./v4l2-formats ${DEV}

v4l2-formats: v4l2-formats.c
	${CC} -o $@ $<

clean:
	@rm -f *~ *.o *.a *.so* *.exe *.dll
	@rm -f *.raw *.rgb *.png
	@rm -f v4l2-formats grab-frame
