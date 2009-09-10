using System;

class FrameFetcher {
    private int[,] data = new int[200,200];

    public event EventHandler NewDataEvent;

    public FrameFetcher() {
	// init demo data
	for( int y=0; y<200; y++ ) {
	    for( int x=0; x<200; x++ ) {
		this.data[y,x] = 255;
	    }
	}
	
	// start loop to fetch new data
	GLib.Timeout.Add( 20, new GLib.TimeoutHandler( Fetch ));
    }

    bool Fetch() {
	// update demo data to represent a new frame
	for( int y=0; y<200; y++ ) {
	    for( int x=0; x<200; x++ ) {
		data[y,x] -= 5;
		if( data[y,x] < 0 ) { data [y,x] = 255 ; }
	    }
	}

	// notify about availability of new data
	if (this.NewDataEvent != null){
	    this.NewDataEvent( this.data, EventArgs.Empty );
	}

	return true;
    }
}
