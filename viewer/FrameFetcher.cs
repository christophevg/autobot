using System;

class FrameFetcher {
    private byte[] data;

    public event EventHandler NewDataEvent;

    public int Width {
	get { return 320; }
    }

    public int Height {
	get { return 240; }
    }

    public int Colors {
	get { return 3; }
    }
    
    public int LinearSize {
	get { return this.Width * this.Height * this.Colors; }
    }

    public FrameFetcher() {
	this.data = new byte[this.LinearSize];
	// init demo data
	for( int x=0; x<this.LinearSize; x++ ) {
	    this.data[x] = 255;
	}
    }

    public void Start() {
	// start loop to fetch new data
	GLib.Timeout.Add( 20, new GLib.TimeoutHandler( Fetch ));
    }

    private bool Fetch() {
	// update demo data to represent a new frame
	for( int x=0; x<this.LinearSize; x++ ) {
	    data[x] -= 5;
	    if( data[x] < 0 ) { data [x] = 255 ; }
	}
	
	// notify about availability of new data
	if (this.NewDataEvent != null){
	    this.NewDataEvent( this.data, EventArgs.Empty );
	}

	return true;
    }
}
