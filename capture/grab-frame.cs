using System;
using System.IO;
using System.Runtime.InteropServices;

struct my_wrapper {
    [DllImport ("libcapture.so.1.0.1")]
    public static extern void grab_frame( String filename, String device );
};

class caller {
    public static void Main( String[] args ) {
	my_wrapper.grab_frame( args.Length > 0 ? args[0] : "out.raw",
			       args.Length > 1 ? args[1] : "/dev/video0" );
    }
};
