using System;
using System.IO;

namespace Project.Autobot.Capture {

  public interface IFrameFetcher {
    int width  { get; }
    int height { get; }
    int colors { get; }
    Frame GetNextFrame();
  }

  public struct FrameFetcher {
    public static IFrameFetcher Setup( String device ) {
      if( device.StartsWith( "/dev/video" ) ) {
        return new V4L2Fetcher( device );
      }
      return new MockFetcher();
    }
  }

  public class MockFetcher : IFrameFetcher {
    private int frameId;
    private String path;
    private Frame frame;

    public int width  { get { return 320; } }
    public int height { get { return 240; } }
    public int colors { get { return 3;   } }

    public String currentMockFile {
      get { return this.path + "/" + this.frameId.ToString() + ".rgb"; }
    }

    public MockFetcher() : this( "./mock" ) {}
    public MockFetcher( String path ) {
      this.frameId = 0;
      this.path = path;
      this.frame = new Frame( this.width, this.height, this.colors );
    }

    public Frame GetNextFrame() {
      long numBytes = new FileInfo(this.currentMockFile).Length;
      FileStream stream = new FileStream( this.currentMockFile, 
        FileMode.Open, FileAccess.Read );
        BinaryReader reader = new BinaryReader(stream);
        this.frame.byteStream = reader.ReadBytes((int)numBytes);
        return this.frame;
      }
    }

    public class V4L2Fetcher : IFrameFetcher {
      private V4L2Device device;

      public int width  { get { return this.device.width;  } }
      public int height { get { return this.device.height; } }
      public int colors { get { return this.device.colors; } }

      public V4L2Fetcher() : this( "/dev/video0" ) {}
      public V4L2Fetcher( String device ) {
        this.device = new V4L2Device( device );
        this.device.Start();
      }

      ~V4L2Fetcher() {
        this.device.Stop();
      }

      public Frame GetNextFrame() {
        return this.device.GetFrame();
      }
    }
  }
