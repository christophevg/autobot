using Gtk;
using System;
using System.Drawing;

class Viewer {
    static ByteCanvas canvas;
    static FrameFetcher fetcher;
    
    static void Main () {
	Application.Init ();
	Window w = new Window( "Project AutoBot - Viewer" );
	w.DeleteEvent += OnDelete;
	
	canvas = new ByteCanvas();

	fetcher = new FrameFetcher();
	fetcher.NewDataEvent += OnNewData;

	Box layout = new HBox( true, 0 );
	layout.Add( canvas );
	w.Add( layout );
	
	w.ShowAll();
	Application.Run();
    }

    static void OnNewData( object data, EventArgs e ) {
	canvas.SetPixelData( (int[,])data );
    }
    
    static void OnDelete( object o, DeleteEventArgs e ) {
	Application.Quit ();
    }
}
