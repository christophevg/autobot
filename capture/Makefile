CPP=g++

all: clean run

run: capture
	./capture

capture: capture.cc
	${CPP} -o $@ $<

clean:
	@rm -f *~
	@rm -f *.ppm
	@rm -f capture