CC=gcc
CFLAGS=-Wall

all: clean out.png

out.png: out.rgb
	convert -size 320x240 -depth 8 rgb:$< $@

out.rgb: out.raw .yuv2rgb_setup
	./yuv2rgb 320 240 $< $@

out.raw: capture
	./capture

capture: capture.c
	${CC} ${CFLAGS} -o $@ $<

formats: v4l2-formats
	./v4l2-formats ${DEV}

v4l2-formats: v4l2-formats.c
	${CC} -o $@ $<

.yuv2rgb_setup:
	( mkdir -p lib; \
	  cd lib; \
	  wget http://www.quickcamteam.net/archive/Yuv2rgb.tar.gz; \
	  tar -zxvf Yuv2rgb.tar.gz; \
	  cd yuv2rgb; \
	  make yuv2rgb )
	mv lib/yuv2rgb/yuv2rgb .
	touch $@

dist: clean
	@rm -rf lib
	@rm -f yuv2rgb .yuv2rgb_setup

clean:
	@rm -f *~ *.o
	@rm -f *.raw *.rgb *.png
	@rm -f a.out v4l2-formats capture
