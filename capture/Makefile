CC=gcc
CFLAGS=-Wall

all: clean dump-jpeg

dump-jpeg: out.small.jpg decode
	./decode $<

out.small.jpg: out.jpg
	convert $< -crop 5x5+0+0 $@

out.jpg: capture
	./capture

capture: capture.c
	${CC} ${CFLAGS} -o $@ $<

decode: decode.c .decode_lib_setup
	${CC} ${CFLAGS} -Ilib/jpeg-7 lib/jpeg-7/*.o -o $@ $<

formats: v4l2-formats
	./v4l2-formats ${DEV}

v4l2-formats: v4l2-formats.c
	${CC} -o $@ $<

.decode_lib_setup: lib
	( cd lib; \
	  wget http://www.ijg.org/files/jpegsrc.v7.tar.gz; \
	  tar -zxvf jpegsrc.v7.tar.gz; \
	  cd jpeg-7; \
	  sh configure; \
	  make djpeg )
	mv lib/jpeg-7/djpeg.o lib/jpeg-7/djpeg.o.tmp
	mv lib/jpeg-7/wrppm.o lib/jpeg-7/wrppm.o.tmp
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
