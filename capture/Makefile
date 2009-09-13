CC=gcc
DEV=/dev/video0

all: clean capture

formats: v4l2-formats
	./v4l2-formats ${DEV}

v4l2-formats: v4l2-formats.c
	${CC} -o $@ $<

capture: v4l2-capture-example
	./v4l2-capture-example -d ${DEV}

v4l2-capture-example: v4l2-capture-example.c
	${CC} -o $@ $<

clean:
	@rm -f *~
	@rm -f *.ppm
	@rm -f a.out v4l2-formats v4l2-capture-example
