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
		public  int byteSize { 
			get { return this.width * this.height * this.colors; } 
		}

		public Frame( int width, int height, int colors ) {
			this.width  = width;
			this.height = height;
			this.colors = colors;
			this.data   = new byte[this.byteSize];
		}
	}

	public class V4L2Device {
		private IntPtr device;
		private Frame frame;

		public V4L2Device( String device ) {
			this.device = V4L2Device._Prepare( device );
			this.frame  = new Frame(V4L2Device._GetWidth(this.device),
			V4L2Device._GetHeight(this.device),
			V4L2Device._GetColors(this.device) );
		}

		public Frame GetFrame() {
			V4L2Device._GetFrame(this.device, this.frame.byteStream);
			return this.frame;
		}

		public void Start() {
			V4L2Device._Start(this.device);
		}	

		public void Stop() {
			V4L2Device._Stop(this.device);
		}

		public int width  { get { return V4L2Device._GetWidth(this.device);  } }
		public int height { get { return V4L2Device._GetHeight(this.device); } }
		public int colors { get { return V4L2Device._GetColors(this.device); } }

		[DllImport ("libcapture.so.1.0.0", EntryPoint="V4L2Device_new")]
		private static extern IntPtr _Prepare( String device );

		[DllImport ("libcapture.so.1.0.0", EntryPoint="V4L2Device_getHeight")]
		private static extern int _GetHeight( IntPtr device );

		[DllImport ("libcapture.so.1.0.0", EntryPoint="V4L2Device_getWidth")]
		private static extern int _GetWidth( IntPtr device );

		[DllImport ("libcapture.so.1.0.0", EntryPoint="V4L2Device_getColors")]
		private static extern int _GetColors( IntPtr device );

		[DllImport ("libcapture.so.1.0.0", EntryPoint="V4L2Device_stop")]
		private static extern void _Stop( IntPtr device );

		[DllImport ("libcapture.so.1.0.0", EntryPoint="V4L2Device_start")]
		private static extern void _Start( IntPtr device );

		[DllImport ("libcapture.so.1.0.0", EntryPoint="V4L2Device_getFrame")]
		private static extern void _GetFrame( IntPtr device, byte[] image );
	}
}