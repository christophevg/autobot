/*
 *  V4L2 camera query program
 *
 *  This program is loosely based on the sample application found at:
 *  http://www.linuxtv.org/downloads/video4linux/API/V4L2_API/spec-single/v4l2.html#CAPTURE-EXAMPLE
 *  which bears the notice: "This program can be used and distributed without restrictions."
 *
 *  Compile with:
 *    gcc -Wall v4l2-formats.c -o v4l2-formats
 *
 * Copyright (c) 2008 Adobe Systems Incorporated
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

typedef struct {
  unsigned int format;
  char *format_string;
} format_record; 

/* copied out of the videodev2.h header */
format_record pixel_formats[] = {
  { V4L2_PIX_FMT_RGB332,   "V4L2_PIX_FMT_RGB332" },
  { V4L2_PIX_FMT_RGB555,   "V4L2_PIX_FMT_RGB555" },
  { V4L2_PIX_FMT_RGB565,   "V4L2_PIX_FMT_RGB565" },
  { V4L2_PIX_FMT_RGB555X,  "V4L2_PIX_FMT_RGB555X" },
  { V4L2_PIX_FMT_RGB565X,  "V4L2_PIX_FMT_RGB565X" },
  { V4L2_PIX_FMT_BGR24,    "V4L2_PIX_FMT_BGR24" },
  { V4L2_PIX_FMT_RGB24,    "V4L2_PIX_FMT_RGB24" },
  { V4L2_PIX_FMT_BGR32,    "V4L2_PIX_FMT_BGR32" },
  { V4L2_PIX_FMT_RGB32,    "V4L2_PIX_FMT_RGB32" },
  { V4L2_PIX_FMT_GREY,     "V4L2_PIX_FMT_GREY" },
  { V4L2_PIX_FMT_YVU410,   "V4L2_PIX_FMT_YVU410" },
  { V4L2_PIX_FMT_YVU420,   "V4L2_PIX_FMT_YVU420" },
  { V4L2_PIX_FMT_YUYV,     "V4L2_PIX_FMT_YUYV" },
  { V4L2_PIX_FMT_UYVY,     "V4L2_PIX_FMT_UYVY" },
  { V4L2_PIX_FMT_YUV422P,  "V4L2_PIX_FMT_YUV422P" },
  { V4L2_PIX_FMT_YUV411P,  "V4L2_PIX_FMT_YUV411P" },
  { V4L2_PIX_FMT_Y41P,     "V4L2_PIX_FMT_Y41P" },
  { V4L2_PIX_FMT_NV12,     "V4L2_PIX_FMT_NV12" },
  { V4L2_PIX_FMT_NV21,     "V4L2_PIX_FMT_NV21" },
  { V4L2_PIX_FMT_YUV410,   "V4L2_PIX_FMT_YUV410" },
  { V4L2_PIX_FMT_YUV420,   "V4L2_PIX_FMT_YUV420" },
  { V4L2_PIX_FMT_YYUV,     "V4L2_PIX_FMT_YYUV" },
  { V4L2_PIX_FMT_HI240,    "V4L2_PIX_FMT_HI240" },
  { V4L2_PIX_FMT_HM12,     "V4L2_PIX_FMT_HM12" },
  { V4L2_PIX_FMT_RGB444,   "V4L2_PIX_FMT_RGB444" },
  { V4L2_PIX_FMT_SBGGR8,   "V4L2_PIX_FMT_SBGGR8" },
  { V4L2_PIX_FMT_MJPEG,    "V4L2_PIX_FMT_MJPEG" },
  { V4L2_PIX_FMT_JPEG,     "V4L2_PIX_FMT_JPEG" },
  { V4L2_PIX_FMT_DV,       "V4L2_PIX_FMT_DV" },
  { V4L2_PIX_FMT_MPEG,     "V4L2_PIX_FMT_MPEG" },
  { V4L2_PIX_FMT_WNVA,     "V4L2_PIX_FMT_WNVA" },
  { V4L2_PIX_FMT_SN9C10X,  "V4L2_PIX_FMT_SN9C10X" },
  { V4L2_PIX_FMT_PWC1,     "V4L2_PIX_FMT_PWC1" },
  { V4L2_PIX_FMT_PWC2,     "V4L2_PIX_FMT_PWC2" },
  { V4L2_PIX_FMT_ET61X251, "V4L2_PIX_FMT_ET61X251" }
};

int format_count = sizeof(pixel_formats) / sizeof(format_record);

int main(int argc, char *argv[])
{
	struct stat st; 
	int fd;
	char *dev_name;
	int i;
	int format_index;
	int error;
	struct v4l2_capability cap;
	struct v4l2_fmtdesc format;

	if (argc < 2)
	{
		printf("USAGE: v4l2-formats <video-device>\n");
		return 1;
	}
	dev_name = argv[1];

	/* check if file exists */
	if (-1 == stat (dev_name, &st)) {
		fprintf (stderr, "Cannot identify '%s': %d, %s\n",
				dev_name, errno, strerror (errno));
		return 2;
	}

	/* check if file is a character device file */
	if (!S_ISCHR (st.st_mode)) {
		fprintf (stderr, "%s is not a character device file\n", dev_name);
		return 2;
	}

	/* open the device */
	fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
	if (-1 == fd) {
		fprintf (stderr, "Cannot open '%s': %d, %s\n",
				dev_name, errno, strerror (errno));
		return 2;
	}

	/* check capabilities */
	if (-1 == ioctl (fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf (stderr, "%s is not a V4L2 device\n",
					dev_name);
			return 2;
		} else {
			return 2;
		}
	}
	printf("%s is a V4L2 device named '%s'\n", dev_name, cap.card);

	/* video capture? */
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf (stderr, "%s is not a video capture device\n",
				dev_name);
		return 2;
	}
	printf("%s is capable of video capture\n", dev_name);

	/* streaming? */
	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf (stderr, "%s does not support streaming i/o\n",
				dev_name);
		return 2;
	}
	printf("%s is capable of streaming capture\n", dev_name);

	/* ask for a pixel format enumeration */
	error = 0;
	format_index = 0;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while (error == 0) {
		format.index = format_index++;
		error = ioctl (fd, VIDIOC_ENUM_FMT, &format);
		if (error == 0) {
			for (i = 0; i < format_count; i++) {
				if (format.pixelformat == pixel_formats[i].format) {
					printf("%s supports '%s' (%s format, %s)\n", 
						dev_name,
						format.description, 
						(format.flags & V4L2_FMT_FLAG_COMPRESSED) ? "compressed" : "raw",
						pixel_formats[i].format_string);
					break;
				}
			}
		}
	}

	close (fd);

        return 0;
}
