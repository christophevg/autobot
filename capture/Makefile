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

out.pnm: out.jpg djpeg
	./djpeg -outfile $@ $<

djpeg: djpeg.c lib/jpeg-7/djpeg.o
	mv lib/jpeg-7/djpeg.o lib/jpeg-7/djpeg.o.tmp
	${CC} -Ilib/jpeg-7 lib/jpeg-7/*.o -o djpeg djpeg.c
	mv lib/jpeg-7/djpeg.o.tmp lib/jpeg-7/djpeg.o

djpeg.c: lib/jpeg-7
	cp lib/jpeg-7/djpeg.c .

lib/jpeg-7/djpeg.o: lib/jpeg-7
	( cd lib/jpeg-7; sh configure; make djpeg )

lib/jpeg-7: lib
	( cd lib; \
	  wget http://www.ijg.org/files/jpegsrc.v7.tar.gz; \
	  tar -zxvf jpegsrc.v7.tar.gz )

lib:
	mkdir lib

dist: clean
	@rm -rf lib

clean:
	@rm -f *~ *.o djpeg.c
	@rm -f *.jpg *.pnm
	@rm -f a.out v4l2-formats capture djpeg
