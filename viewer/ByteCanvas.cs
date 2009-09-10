using Gtk;
using System;
using System.Drawing;

class ByteCanvas : DrawingArea {
    public ByteCanvas() : this( 200, 200 ) {}

    public ByteCanvas( int width, int height ) {
	SetSizeRequest( width, height );
    }

    private int[,] data;
    
    public void SetPixelData( int[,] data ) {
	this.data = data;
	QueueDraw();
    }

    protected override bool OnExposeEvent( Gdk.EventExpose args ) {
	using( Graphics g = Gtk.DotNet.Graphics.FromDrawable( args.Window ) ) {
	    for( int y=0; y<200; y++ ) {
		for( int x=0; x<200; x++ ) {
		    Pen pen = new Pen( Color.FromArgb(this.data[y,x], 
						      this.data[y,x],
						      this.data[y,x]), 1.0f);
		    g.DrawLine( pen, x, y, x+1, y );
		}
	    }
	}
	return true;
    }
}
