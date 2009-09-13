CC=gcc
DEV=/dev/video0

all: clean capture

formats: v4l2-formats
	./v4l2-formats ${DEV}

v4l2-formats: v4l2-formats.c
	${CC} -o $@ $<

capture: capture.c
	${CC} -o $@ $<
	./capture -d ${DEV}

clean:
	@rm -f *~
	@rm -f *.jpg
	@rm -f a.out v4l2-formats capture
