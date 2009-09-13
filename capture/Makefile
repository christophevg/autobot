CC=gcc
DEV=/dev/video0

all: clean out.jpg

formats: v4l2-formats
	./v4l2-formats ${DEV}

v4l2-formats: v4l2-formats.c
	${CC} -o $@ $<

out.jpg: capture
	./capture -d ${DEV}

capture: capture.c
	${CC} -o $@ $<

out.pnm: out.jpg decode
	./decode $< $@

decode: decode.c .decode_lib_setup
	mv lib/jpeg-7/djpeg.o lib/jpeg-7/djpeg.o.tmp
	${CC} -Ilib/jpeg-7 lib/jpeg-7/*.o -o $@ $<
	mv lib/jpeg-7/djpeg.o.tmp lib/jpeg-7/djpeg.o

.decode_lib_setup: lib
	( cd lib; \
	  wget http://www.ijg.org/files/jpegsrc.v7.tar.gz; \
	  tar -zxvf jpegsrc.v7.tar.gz; \
	  cd jpeg-7; \
	  sh configure; \
	  make djpeg )
	touch .decode_lib_setup

lib:
	mkdir lib

dist: clean
	@rm -rf lib
	@rm -f .decode_lib_setup

clean:
	@rm -f *~ *.o djpeg.c
	@rm -f *.jpg *.pnm
	@rm -f a.out v4l2-formats capture decode
