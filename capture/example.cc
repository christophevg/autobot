//
// this program demonstrates simple capture
// it is only moderately robust in its error handling, and
// if an error is encountered, the program does not perform proper cleanup
//
// for simplicity, the program is written entirely in the main() function
// essentially it is cut & pasted from the tutorial
//


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

char* deviceName = "/dev/video0";
int deviceHandle = 0;
int i; // temporary counter
struct video_capability capability;
struct video_channel queryChannel;
struct video_channel selectedChannel;
struct video_window captureWindow;
struct video_picture imageProperties;
struct video_mbuf memoryBuffer;
struct video_mmap* mmaps;
char* memoryMap;
int channelNumber = 0;
int width = 320;
int height = 240;
int depth;
int palette;
int bufferIndex;

char* NextFrame()
{
        // send a request to begin capturing to the currently indexed buffer
        if (ioctl (deviceHandle, VIDIOCMCAPTURE, &mmaps[bufferIndex]) == -1)
        {       // capture request failed
        }

        // move bufferIndex to the next frame
        ++ bufferIndex;
        if (bufferIndex == memoryBuffer.frames)
        {       // bufferIndex is indexing beyond the last buffer
                // set it to index the first buffer
                bufferIndex = 0;
        }

        // wait for the currently indexed frame to complete capture
        if (ioctl (deviceHandle, VIDIOCSYNC, &mmaps[bufferIndex]) == -1)
        {       // sync request failed
        }

        // return the address of the frame data for the current buffer index
        return (memoryMap + memoryBuffer.offsets[bufferIndex]);
}



int main()
{
        // open the device
        deviceName = "/dev/video0";
        deviceHandle = open (deviceName, O_RDWR);
        if (deviceHandle == -1)
        {       // could not open device
                printf ("Could not open device %s - %s\n", deviceName, sys_errlist[errno]);
                return -1;
        }

        // get device capabilities
        if (ioctl (deviceHandle, VIDIOCGCAP, &capability) == -1)
        {       // query failed
                printf ("could not obtain device capabilities\n");
                return -1;
        }
        if ((capability.type & VID_TYPE_CAPTURE) == 0)
        {       // this device cannot capture video to memory, exit
                printf ("this device cannot capture video to memory\n");
                return -1;
        }

        // enumerate and print out the channels
        printf ("Select a channel:\n");
        i = 0;
        while (i < capability.channels)
        {
                queryChannel.channel = i;
                if (ioctl (deviceHandle, VIDIOCGCHAN, &queryChannel) != -1)
                {       // ioctl success, queryChannel contains information about this channel
                        printf ("%d. %s\n", queryChannel.channel, queryChannel.name);
                }
                ++ i;
        }

        // have the user select a channel
        printf (": ");
        fflush (stdout);
        scanf ("%d", &channelNumber);

        // set the selected channel
        selectedChannel.channel = channelNumber;
        selectedChannel.norm = VIDEO_MODE_NTSC;
        if (ioctl (deviceHandle, VIDIOCSCHAN, &selectedChannel) == -1)
        {       // could not set the selected channel
                printf ("Could not set channel #%d\nNot a fatal error.", channelNumber);
        }

        // set the desired width and height
        if ((capability.type & VID_TYPE_SCALES) != 0)
        {       // supports the ability to scale captured images

                // have the user enter a desired width
                printf ("Enter the desired width (e.g. 320): ");
                fflush (stdout);
                scanf ("%d", &width);
                printf ("Enter the desired height (e.g. 240): ");
                fflush (stdout);
                scanf ("%d", &height);

                captureWindow.x = 0;
                captureWindow.y = 0;
                captureWindow.width = width;
                captureWindow.height = height;
                captureWindow.chromakey = 0;
                captureWindow.flags = 0;
                captureWindow.clips = 0;
                captureWindow.clipcount = 0;
                if (ioctl (deviceHandle, VIDIOCSWIN, &captureWindow) == -1)
                {       // could not set window values for capture
                        printf ("Could not set desired dimensions\nNot a fatal error.\n");
                }
        }


        // retrieve the actual width and height of the capturing images
        if (ioctl (deviceHandle, VIDIOCGWIN, &captureWindow) == -1)
        {       // could not obtain specifics of capture window
                printf ("Could not obtain capture window dimensions.\n");
        }
        width = captureWindow.width;
        height = captureWindow.height;
        printf ("Capturing dimensions are : %d, %d\n", width, height);


        // request that we capture to 24bit RGB

        // get image properties
        if (ioctl (deviceHandle, VIDIOCGPICT, &imageProperties) != -1)
        {       // successfully retrieved the default image properties

                // the following values are for requesting 24bit RGB
                imageProperties.depth = 24;
                imageProperties.palette = VIDEO_PALETTE_RGB24;
                if (ioctl (deviceHandle, VIDIOCSPICT, &imageProperties) == -1)
                {       // failed to set the image properties
                        printf ("Could not set the video depth and palette.\nPerhaps not a fatal error.\n");
                }
        }

        // verify that the requested capture pixel depth and palette succeeded
        if (ioctl (deviceHandle, VIDIOCGPICT, &imageProperties) == -1)
        {       // failed to retrieve default image properties
                printf ("Failed to retrieve the video depth and palette.\n");
                return -1;
        }
        depth = imageProperties.depth;
        palette = imageProperties.palette;
        if ((depth != 24) || (palette != VIDEO_PALETTE_RGB24))
        {       // not a format our program supports
                printf ("Format is not 24bit RGB.\n");
                return -1;
        }
        printf ("Capture depth is 24bit RGB\n");

        // obtain memory about capture space
        if (ioctl (deviceHandle, VIDIOCGMBUF, &memoryBuffer) == -1)
        {       // failed to retrieve information about capture memory space
                printf ("Failed to retrieve information about MMIO space.\n");
                return -1;
        }


        // obtain memory mapped area
        memoryMap = (char*)mmap (0, memoryBuffer.size, PROT_READ | PROT_WRITE, MAP_SHARED, deviceHandle, 0);
        if ((int)memoryMap == -1)
        {       // failed to retrieve pointer to memory mapped area
                printf ("Failed to obtain MMIO space.\n");
                return -1;
        }


        // allocate structures
        mmaps = (struct video_mmap*)(malloc (memoryBuffer.frames * sizeof (struct video_mmap)));

        // fill out the fields
        i = 0;
        while (i < memoryBuffer.frames)
        {
                mmaps[i].frame = i;
                mmaps[i].width = width;
                mmaps[i].height = height;
                mmaps[i].format = palette;
                ++ i;
        }

        // request capture to each buffer except the last one
        i = 0;
        while (i < (memoryBuffer.frames-1))
        {
                if (ioctl (deviceHandle, VIDIOCMCAPTURE, &mmaps[i]) == -1)
                {       // capture request failed
                }
                ++ i;
        }

        // set our index to the last buffer
        bufferIndex = memoryBuffer.frames-1;


        // capture and write out ten frames
        printf ("Capture is ready; capturing 10 images.\n");
        int i = 0;
        while (i < 10)
        {
                char* frame = NextFrame();

                // write out PPM file
                char fname[80];
                sprintf (fname, "output%02d.ppm", i);
                printf ("Writing out PPM file %s\n", fname);

                FILE* fp;
                if ((fp = fopen (fname, "w")) == NULL)
                {
                        printf ("Could not open file %s for writing.\n", fname);
                        return -1;
                }

                fprintf (fp, "P6\n%d %d\n255\n", width, height);

                int n = width * height;

                for (int index = 0;  index < n;  ++ index)
                {
                        putc (frame[index*3+2], fp);
                        putc (frame[index*3+1], fp);
                        putc (frame[index*3+0], fp);
                }

                fflush (fp);
                fclose (fp);

                ++ i;
        }
        printf ("Use 'xv output*' to view output.\n");

        // free the video_mmap structures
        free (mmaps);

        // unmap the capture memory
        munmap (memoryMap, memoryBuffer.size);

        // close the device
        close (deviceHandle);

        return 0;
}

