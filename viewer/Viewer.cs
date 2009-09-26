using Gtk;
using System;
using System.Drawing;

using Project.Autobot.Capture;

class Viewer {
    static void Main () {
	new Viewer().Start();
    }

    Window       window;
    ByteCanvas   canvas;
    IFrameFetcher fetcher;
    
    public Viewer() {
	this.SetupApplication();
	this.SetupFrameFetcher();
	this.SetupByteCanvas();
	this.SetupWindow();
    }

    public void Start() {
	this.window.ShowAll();
	this.StartLoop();
	Application.Run();
    }

    private void StartLoop() {
	GLib.Timeout.Add( 20, new GLib.TimeoutHandler( Refresh ));	
    }

    private bool Refresh() {
	this.canvas.SetPixelData( this.fetcher.GetNextFrame().byteStream );
	return true;
    }

    private void SetupApplication() {
	Application.Init ();
	
    }

    private void SetupFrameFetcher() {
	this.fetcher = FrameFetcher.Setup("/dev/video0");
    }

    private void SetupByteCanvas() {
	this.canvas = new ByteCanvas( this.fetcher.width, 
				      this.fetcher.height,
				      this.fetcher.colors );
    }

    private void SetupWindow() {
	this.window = new Window( "Project AutoBot - Viewer" );
	this.window.DeleteEvent += delegate { Application.Quit(); };
	Box layout = new HBox( true, 0 );
	layout.Add( this.canvas );
	this.window.Add( layout );
    }
}
