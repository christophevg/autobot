using System;
using System.IO;
using System.Runtime.InteropServices;

struct capturer {
    [DllImport ("libcapture.so.1.0.1")]
    public static extern void prepare( String device );

    [DllImport ("libcapture.so.1.0.1")]
    public static extern void stop();

    [DllImport ("libcapture.so.1.0.1")]
    public static extern void fetch_frame( byte[] image );
};

class caller {
    public static void Main( String[] args ) {
	String filename = args.Length > 0 ? args[0] : "out.rgb";
	String device   = args.Length > 1 ? args[1] : "/dev/video0";

	capturer.prepare( device );

	for( int f=0; f<5; f++ ) {
	    byte[] image = new byte[320*240*3];
	    capturer.fetch_frame( image );

	    FileStream stream = new FileStream(f.ToString() + "-" + filename, 
					       FileMode.Create);
	    BinaryWriter w = new BinaryWriter(stream);
	    w.Write(image, 0, 320*240*3);
	    w.Close();
	}

	capturer.stop();
    }
};
