using System;
using System.IO;
using System.Runtime.InteropServices;

struct my_wrapper {
    [DllImport ("libcapture.so.1.0.1")]
    public static extern byte[] grab_frame( String device );
};

class caller {
    public static void Main( String[] args ) {
	String filename = args.Length > 0 ? args[0] : "out.rgb";
	String device   = args.Length > 1 ? args[1] : "/dev/video0";

	byte[] image = my_wrapper.grab_frame( device );

	FileStream stream = new FileStream(filename, FileMode.Create);
	BinaryWriter w = new BinaryWriter(stream);
	for(int i=0; i<320*240*3; i++ ) {
	    w.Write(image[i]);
	}
	w.Close();
    }
};
