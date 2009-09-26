using System;
using System.IO;
using System.Runtime.InteropServices;

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

    public class Frame {
	private byte[] data;
	public byte[] byteStream { 
	    get { return this.data;  } 
	    set { this.data = value; }
	}
	
	private int width;
	private int height;
	private int colors;
	public int byteSize { 
	    get { return this.width * this.height * this.colors; } 
	}
	
	public Frame( int width, int height, int colors ) {
	    this.width  = width;
	    this.height = height;
	    this.colors = colors;
	    this.data   = new byte[this.byteSize];
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
	private Frame frame;

	public int width  { get { return V4L2Fetcher.__get_width() ; } }
	public int height { get { return V4L2Fetcher.__get_height(); } }
	public int colors { get { return V4L2Fetcher.__get_colors(); } }

	public V4L2Fetcher() : this( "/dev/video0" ) {}
	public V4L2Fetcher( String device ) {
	    V4L2Fetcher.__prepare( device );
	    this.frame = new Frame( this.width, this.height, this.colors ); 
	}
	
	~V4L2Fetcher() {
	    V4L2Fetcher.__stop();
	}
	
	public Frame GetNextFrame() {
	    V4L2Fetcher.__fetch_frame( this.frame.byteStream );
	    return this.frame;
	}
	
	[DllImport ("libcapture.so.1.0.0", EntryPoint="prepare")]
        private static extern void __prepare( String device );
	
	[DllImport ("libcapture.so.1.0.0", EntryPoint="get_height")]
	private static extern int __get_height();
	
	[DllImport ("libcapture.so.1.0.0", EntryPoint="get_width")]
	private static extern int __get_width();

	[DllImport ("libcapture.so.1.0.0", EntryPoint="get_colors")]
	private static extern int __get_colors();
	
	[DllImport ("libcapture.so.1.0.0", EntryPoint="stop")]
	private static extern void __stop();
	
	[DllImport ("libcapture.so.1.0.0", EntryPoint="fetch_frame")]
	private static extern void __fetch_frame( byte[] image );
    }
}
