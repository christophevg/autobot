using Gtk;
using System;
using System.Drawing;

class Viewer {
    static void Main () {
	new Viewer().Start();
    }

    Window       window;
    ByteCanvas   canvas;
    FrameFetcher fetcher;
    
    public Viewer() {
	this.SetupApplication();
	this.SetupFrameFetcher();
	this.SetupByteCanvas();
	this.SetupWindow();
    }

    public void Start() {
	this.window.ShowAll();
	this.fetcher.Start();
	Application.Run();
    }

    private void SetupApplication() {
	Application.Init ();
	
    }

    private void SetupFrameFetcher() {
	this.fetcher = new FrameFetcher();
	this.fetcher.NewDataEvent += delegate( object data, EventArgs e ) { 
	    this.canvas.SetPixelData((byte[])data);
	};
    }

    private void SetupByteCanvas() {
	this.canvas = new ByteCanvas( this.fetcher.Width, 
				      this.fetcher.Height,
				      this.fetcher.Colors );
    }

    private void SetupWindow() {
	this.window = new Window( "Project AutoBot - Viewer" );
	this.window.DeleteEvent += delegate { Application.Quit(); };
	Box layout = new HBox( true, 0 );
	layout.Add( this.canvas );
	this.window.Add( layout );
    }
}
