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
	this.RenderUsingBitmap(Gtk.DotNet.Graphics.FromDrawable( args.Window ));
	return true;
    }

    private void RenderUsingBitmap( Graphics g ) {
	Bitmap bitmap = new Bitmap(200,200);
	for( int y=0; y<200; y++ ) {
	    for( int x=0; x<200; x++ ) {
		bitmap.SetPixel( x, y, Color.FromArgb( this.data[y,x], 
						       this.data[y,x], 
						       this.data[y,x] ) );
	    }
	}
	g.DrawImage(bitmap,0,0);
    }
    
}
