using Gtk;
using System;
using System.Drawing;

class ByteCanvas : DrawingArea {
    private byte[] data;

    private int colors;
    public int Colors {
	get { return this.colors; }
    }

    public int LinearSize {
	get { return this.WidthRequest * this.HeightRequest * this.Colors; }
    }

    public ByteCanvas( int width, int height, int colors ) {
	SetSizeRequest( width, height );
	this.colors = colors;
    }
   
    public void SetPixelData( byte[] data ) {
	this.data = data;
	QueueDraw();
    }

    protected override bool OnExposeEvent( Gdk.EventExpose args ) {
	this.RenderUsingBitmap(Gtk.DotNet.Graphics.FromDrawable( args.Window ));
	return true;
    }

    private void RenderUsingBitmap( Graphics g ) {
	// cache these, because the getters are pretty slow
	int width  = this.WidthRequest;
	int height = this.HeightRequest;
	int colors = this.Colors;

	Bitmap bitmap = new Bitmap(width, height);

	for( int x=0; x<width*height*colors; x+=colors ) {
	    bitmap.SetPixel( (x / colors) % width, x / colors / width, 
			     Color.FromArgb( this.data[x], 
					     this.data[x+1], 
					     this.data[x+2] ) );
	}
	g.DrawImage(bitmap,0,0);
    }
}
