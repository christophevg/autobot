MONO=mono
CC=gmcs
CFLAGS=-pkg:gtk-sharp-2.0 -pkg:gtk-dotnet-2.0 -r:System.Drawing
SRCS=Viewer.cs ByteCanvas.cs FrameFetcher.cs

all: run

run: Viewer.exe
	${MONO} $<

Viewer.exe: ${SRCS}
	${CC} ${CFLAGS} ${SRCS}

clean:
	@rm -rf *~
	@rm -rf *.exe
