using System;
using System.IO;

using Project.Autobot.Capture;

class caller {
    public static void Main( String[] args ) {
	String filename = args.Length > 0 ? args[0] : "out.rgb";
	String device   = args.Length > 1 ? args[1] : "/dev/video0";

	IFrameFetcher fetcher = FrameFetcher.setup( device );

	for( int f=0; f<5; f++ ) {
	    Frame frame = fetcher.getNextFrame();
	    FileStream stream = new FileStream( f.ToString() + "-" + filename, 
						FileMode.Create );
	    BinaryWriter writer = new BinaryWriter( stream );
	    writer.Write( frame.byteStream, 0, frame.byteSize );
	    writer.Close();
	}
    }
}

