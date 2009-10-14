using System;
using System.IO;

using Project.Autobot.Capture;

class caller {
  public static void Main( String[] args ) {
    String filename = args.Length > 0 ? args[0] : "out.rgb";
    String device   = args.Length > 1 ? args[1] : "/dev/video0";

    IFrameFetcher fetcher = FrameFetcher.Setup( device );

    Frame frame = fetcher.GetNextFrame();

    FileStream stream = new FileStream( filename, FileMode.Create );
    BinaryWriter writer = new BinaryWriter( stream );
    writer.Write( frame.byteStream, 0, frame.byteSize );
    writer.Close();
  }
}
