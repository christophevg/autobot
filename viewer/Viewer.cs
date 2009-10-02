using Gtk;
using System;
using System.Drawing;
using System.Diagnostics;

using Project.Autobot.Capture;

class Viewer {
  static void Main () {
    new Viewer().Start();
  }

  Window        window;

  ByteCanvas    canvas0;
  ByteCanvas    canvas1;

  IFrameFetcher fetcher0;
  IFrameFetcher fetcher1;

  long      frames;
  Stopwatch stopwatch;

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
    this.StartFPSCounter();
    GLib.Timeout.Add( 20, new GLib.TimeoutHandler( Refresh ));	
  }

  private bool Refresh() {
    this.canvas0.SetPixelData( this.fetcher0.GetNextFrame().byteStream );
    this.CountNextFrame();
    this.canvas1.SetPixelData( this.fetcher1.GetNextFrame().byteStream );
    this.CountNextFrame();

    while( Application.EventsPending() ) { Application.RunIteration (); }
    return true;
  }

  private void SetupApplication() {
    Application.Init ();
  }

  private void SetupFrameFetcher() {
    this.fetcher0 = FrameFetcher.Setup("/dev/video0");
    this.fetcher1 = FrameFetcher.Setup("/dev/video1");
  }

  private void SetupByteCanvas() {
    this.canvas0 = new ByteCanvas( this.fetcher0.width, 
                                   this.fetcher0.height,
                                   this.fetcher0.colors );
    this.canvas1 = new ByteCanvas( this.fetcher1.width, 
                                   this.fetcher1.height,
                                   this.fetcher1.colors );
  }

  private void SetupWindow() {
    this.window = new Window( "Project AutoBot - Viewer" );
    this.window.DeleteEvent += delegate { Application.Quit(); };
    Box layout = new HBox( true, 0 );
    layout.Add( this.canvas0 );
    layout.Add( this.canvas1 );
    this.window.Add( layout );
  }

  private void StartFPSCounter() {
    this.frames = 0;
    this.stopwatch = new Stopwatch();
    this.stopwatch.Start();
  }

  private void CountNextFrame() {
    this.frames++;
    if( this.frames % 100 == 0 ) {
      long fps = this.frames / (this.stopwatch.ElapsedMilliseconds / 1000);
      Console.WriteLine( "{0} FPS", fps );
    }
  }
}
