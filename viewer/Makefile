MONO=mono
CC=gmcs
CFLAGS=-pkg:gtk-sharp-2.0 -pkg:gtk-dotnet-2.0 -r:System.Drawing -r:Project.Autobot.Capture.dll
SRCS=Viewer.cs ByteCanvas.cs

all: run

run: Viewer.exe
	LD_LIBRARY_PATH=. ${MONO} $<

Viewer.exe: ${SRCS} Project.Autobot.Capture.dll
	${CC} ${CFLAGS} ${SRCS}

Project.Autobot.Capture.dll:
	(cd ../capture; make grab-frame.exe)
	cp ../capture/$@ .
	cp ../capture/*.so.* .

clean:
	@rm -rf *~
	@rm -rf *.exe *.dll *.so.*
