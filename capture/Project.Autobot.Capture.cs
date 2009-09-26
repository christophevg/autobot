using System;
using System.Runtime.InteropServices;

namespace Project.Autobot.Capture {
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
    
    public class FrameFetcher {
	private Frame frame;
	
	public FrameFetcher() : this( "/dev/video0" ) {}
	public FrameFetcher( String device ) {
	    FrameFetcher.prepare( device );
	    this.frame = new Frame( FrameFetcher.get_width(), 
				    FrameFetcher.get_height(), 
				    FrameFetcher.get_colors() );
	}
	
	~FrameFetcher() {
	    FrameFetcher.stop();
	}
	
	public Frame getNextFrame() {
	    FrameFetcher.fetch_frame( this.frame.byteStream );
	    return this.frame;
	}
	
	[DllImport ("libcapture.so.1.0.0")]
        private static extern void prepare( String device );
	
	[DllImport ("libcapture.so.1.0.0")]
	private static extern int get_height();
	
	[DllImport ("libcapture.so.1.0.0")]
	private static extern int get_width();

	[DllImport ("libcapture.so.1.0.0")]
	private static extern int get_colors();
	
	[DllImport ("libcapture.so.1.0.0")]
	private static extern void stop();
	
	[DllImport ("libcapture.so.1.0.0")]
	private static extern void fetch_frame( byte[] image );
    }
}
