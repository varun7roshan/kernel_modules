#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>


#define DEVICE_FILE_VIDEO	"/dev/video0"
#define V4L2_CAP_META_CAPTURE   0x00800000  /* Is a metadata capture device */
/**************************************************************************************************************************/
/*       v4l2_capability       */
/**************************************************************************************************************************/
#if 0
/*
 *  I N P U T   I M A G E   C R O P P I N G
 */
struct v4l2_rect {
  __s32   left;
  __s32   top;
  __u32   width;
  __u32   height;
};

struct v4l2_fract {
  __u32   numerator;
  __u32   denominator;
};

enum v4l2_buf_type {
  V4L2_BUF_TYPE_VIDEO_CAPTURE        = 1,
  V4L2_BUF_TYPE_VIDEO_OUTPUT         = 2,
};

struct v4l2_cropcap {
  __u32     type; /* enum v4l2_buf_type */
  struct v4l2_rect        bounds;
  struct v4l2_rect        defrect;
  struct v4l2_fract       pixelaspect;
};

struct v4l2_crop {
  __u32     type; /* enum v4l2_buf_type */
  struct v4l2_rect        c;
};
#endif

void print_crop_rect(struct v4l2_rect *r)
{
	printf("width:%d\n", r->width);
	printf("height:%d\n", r->height);
	printf("top:%d\n", r->top);
	printf("left:%d\n", r->left);
	printf("----------\n");
}

void print_crop_cap(struct v4l2_cropcap *crop)
{
	printf("crop->defrect:\n");
	print_crop_rect(&crop->defrect);	

	printf("crop->bounds:\n");
	print_crop_rect(&crop->bounds);

	printf("crop->pixelaspect.numerator:%d\n", crop->pixelaspect.numerator);
	printf("crop->pixelaspect.denominator:%d\n", crop->pixelaspect.denominator);	
}

int  crop_cap(int fd)
{
	struct v4l2_cropcap crop_cap;
	int ret = 0;
	
	memset(&crop_cap, 0, sizeof(crop_cap));
	crop_cap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_CROPCAP, &crop_cap);
	if(ret < 0)
	{
		printf("ERROR:ioctl(VIDIOC_CROPCAP):errno:%d, %s\n", errno, strerror(errno));
		return 0;
	}

	print_crop_cap(&crop_cap);
}



/**************************************************************************************************************************/
/*      v4l2_capability       */
/**************************************************************************************************************************/
typedef struct struct_cap_list
{
	int code;
	char *cap_str;
}cap_list_t;

cap_list_t cap_list [] = {
	{V4L2_CAP_VIDEO_CAPTURE, "V4L2_CAP_VIDEO_CAPTURE"},
	{V4L2_CAP_VIDEO_OUTPUT, "V4L2_CAP_VIDEO_OUTPUT"},
	{V4L2_CAP_VIDEO_OVERLAY, "V4L2_CAP_VIDEO_OVERLAY"},
	{V4L2_CAP_VIDEO_CAPTURE_MPLANE, "V4L2_CAP_VIDEO_CAPTURE_MPLANE"},
	{V4L2_CAP_VIDEO_M2M, "V4L2_CAP_VIDEO_M2M"},
	{V4L2_CAP_READWRITE, "V4L2_CAP_READWRITE"}, // --> This using IO read() write() of getting frames
	{V4L2_CAP_ASYNCIO, "V4L2_CAP_ASYNCIO"},
	{V4L2_CAP_STREAMING, "V4L2_CAP_STREAMING - MMAP and User Pointer supported"}, // --> This means MMAP and USER PTR is supported
	{V4L2_CAP_DEVICE_CAPS, "V4L2_CAP_DEVICE_CAPS"},
	{V4L2_CAP_META_CAPTURE, "V4L2_CAP_META_CAPTURE"},
	{V4L2_CAP_EXT_PIX_FORMAT, "V4L2_CAP_EXT_PIX_FORMAT"},
};

void query_cap(int fd)
{
	#if 0
/**
  * struct v4l2_capability - Describes V4L2 device caps returned by VIDIOC_QUERYCAP
  *
  * @driver:     name of the driver module (e.g. "bttv")
  * @card:     name of the card (e.g. "Hauppauge WinTV")
  * @bus_info:     name of the bus (e.g. "PCI:" + pci_name(pci_dev) )
  * @version:    KERNEL_VERSION
  * @capabilities: capabilities of the physical device as a whole
  * @device_caps:  capabilities accessed via this particular device (node)
  * @reserved:     reserved fields for future extensions
  */
struct v4l2_capability {
  __u8  driver[16];
  __u8  card[32];
  __u8  bus_info[32];
  __u32   version;
  __u32 capabilities;
  __u32 device_caps;
  __u32 reserved[3];
};
	#endif
	int ret = 0;	
	struct v4l2_capability cap;
	int i = 0;
	
	memset(&cap, 0, sizeof(cap));
	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if(ret < 0)
	{
		printf("ERROR:ioctl(VIDIOC_QUERYCAP):errno:%d, %s\n", errno, strerror(errno));
		return;
	}
	
	printf("cap.driver:%s\n", cap.driver);
	printf("cap.card:%s\n", cap.card);
	printf("cap.bus_info:%s\n", cap.bus_info);
	printf("cap.version:%d\n", cap.version);
	printf("cap.capabilities:0x%x\n", cap.capabilities);
	printf("cap.device_caps:0x%x\n", cap.device_caps);
	
	printf("%s has below capabilities:\n", DEVICE_FILE_VIDEO);
	for(i = 0; i < (sizeof(cap_list)/sizeof(cap_list_t)); i++)
	{
		if(cap.capabilities & cap_list[i].code)
			printf("%s\n", cap_list[i].cap_str);
	}
}
/**************************************************************************************************************************/
/*    FORMATS   */
/**************************************************************************************************************************/
#if 0
#define VIDIOC_ENUM_FMT         _IOWR('V',  2, struct v4l2_fmtdesc)
#define VIDIOC_G_FMT    _IOWR('V',  4, struct v4l2_format)
#define VIDIOC_S_FMT    _IOWR('V',  5, struct v4l2_format)
#define VIDIOC_TRY_FMT    _IOWR('V', 64, struct v4l2_format)

/*
 *  F O R M A T   E N U M E R A T I O N
 */
struct v4l2_fmtdesc {
  __u32       index;             /* Format number      */
  __u32       type;              /* enum v4l2_buf_type */
  __u32               flags;
  __u8        description[32];   /* Description string */
  __u32       pixelformat;       /* Format fourcc      */
  __u32       mbus_code;    /* Media bus code    */
  __u32       reserved[3];
};
#endif

void print_enum_formats(int fd)
{
	struct v4l2_fmtdesc fmtdesc;
	int i = 0;
	int ok = 0;
	
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	
	/*we have index here - which is the format number - starting from 0*/
	for(i = 0; ok == 0; i++)
	{
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmtdesc.index = i;
		
		ok = ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc);
		if(ok < 0)
		{
			printf("End of VIDIOC_ENUM_FMT:errno:%d, %s\n", errno, strerror(errno));
			break;
		}

		printf("Index:%d, flags:%d, description:%s, pixelformat fourcc:%c %c %c %c\n", fmtdesc.index, fmtdesc.flags, fmtdesc.description, 
																								fmtdesc.pixelformat & 0xff,
																								(fmtdesc.pixelformat >> 8) & 0xff,
																								(fmtdesc.pixelformat >> 16) & 0xff,
																								(fmtdesc.pixelformat >> 24) & 0xff);
	}
}

#if 0
/**
 * struct v4l2_format - stream data format
 * @type: enum v4l2_buf_type; type of the data stream
 * @pix:  definition of an image format
 * @pix_mp: definition of a multiplanar image format
 * @win:  definition of an overlaid image
 * @vbi:  raw VBI capture or output parameters
 * @sliced: sliced VBI capture or output parameters
 * @raw_data: placeholder for future extensions and custom formats
 */
struct v4l2_format {
  __u32  type;
  union {
    struct v4l2_pix_format    pix;     /* V4L2_BUF_TYPE_VIDEO_CAPTURE */
    struct v4l2_pix_format_mplane pix_mp;  /* V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE */
    struct v4l2_window    win;     /* V4L2_BUF_TYPE_VIDEO_OVERLAY */
    struct v4l2_vbi_format    vbi;     /* V4L2_BUF_TYPE_VBI_CAPTURE */
    struct v4l2_sliced_vbi_format sliced;  /* V4L2_BUF_TYPE_SLICED_VBI_CAPTURE */
    struct v4l2_sdr_format    sdr;     /* V4L2_BUF_TYPE_SDR_CAPTURE */
    __u8  raw_data[200];                   /* user-defined */
  } fmt;
};

/*
 *  V I D E O   I M A G E   F O R M A T
 */
struct v4l2_pix_format {
  __u32             width;
  __u32     height;
  __u32     pixelformat;
  __u32     field;    /* enum v4l2_field */
  __u32             bytesperline; /* for padding, zero if unused */
  __u32             sizeimage;
  __u32     colorspace; /* enum v4l2_colorspace */
  __u32     priv;   /* private data, depends on pixelformat */
  __u32     flags;    /* format flags (V4L2_PIX_FMT_FLAG_*) */
  __u32     ycbcr_enc;  /* enum v4l2_ycbcr_encoding */
  __u32     quantization; /* enum v4l2_quantization */
  __u32     xfer_func;  /* enum v4l2_xfer_func */
};

#endif

/*VIDIOC_G_FMT - Note this will get teh current format*/
int print_get_format(int fd)
{
	struct v4l2_format format;
	int ret = 0;

	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if(ret < 0)
	{
		printf("ERROR:ioctl(VIDIOC_G_FMT):errno:%d, %s\n", errno, strerror(errno));
		return 0;
	}
	printf("format.fmt.pix.width:%d\n", format.fmt.pix.width);
	printf("format.fmt.pix.height:%d\n", format.fmt.pix.height);
	printf("format.fmt.pix.field:%d - V4L2_FIELD_NONE:%d Device has no fields\n", format.fmt.pix.field, V4L2_FIELD_NONE);
	printf("format.fmt.pix.bytesperline:%d, for padding, 0 if unused\n", format.fmt.pix.bytesperline);
	printf("format.fmt.pix.sizeimage:%d\n", format.fmt.pix.sizeimage);
	printf("format.fmt.pix.colorspace:%d, V4L2_COLORSPACE_SRGB:%d - For RGB colorspaces such as produces by most webcams\n", format.fmt.pix.colorspace, V4L2_COLORSPACE_SRGB);
	printf("format.fmt.pix.priv:%d\n", format.fmt.pix.priv);
	printf("format.fmt.pix.flags:%d\n", format.fmt.pix.flags);
	printf("format.fmt.pix.ycbcr_enc:%d - V4L2_YCBCR_ENC_DEFAULT:%d\n", format.fmt.pix.ycbcr_enc, V4L2_YCBCR_ENC_DEFAULT);
	printf("format.fmt.pix.quantization:%d, V4L2_QUANTIZATION_DEFAULT:%d\n", format.fmt.pix.quantization, V4L2_QUANTIZATION_DEFAULT);
	printf("format.fmt.pix.xfer_func:%d, V4L2_XFER_FUNC_DEFAULT:%d\n", format.fmt.pix.xfer_func, V4L2_XFER_FUNC_DEFAULT);
	
}
/**************************************************************************************************************************/
/*        VIDIOC_ENUM_FRAMESIZES      */
/**************************************************************************************************************************/

#if 0
enum v4l2_frmsizetypes {
  V4L2_FRMSIZE_TYPE_DISCRETE  = 1,
  V4L2_FRMSIZE_TYPE_CONTINUOUS  = 2,
  V4L2_FRMSIZE_TYPE_STEPWISE  = 3,
};

struct v4l2_frmsize_discrete {
  __u32     width;    /* Frame width [pixel] */
  __u32     height;   /* Frame height [pixel] */
};

struct v4l2_frmsize_stepwise {
  __u32     min_width;  /* Minimum frame width [pixel] */
  __u32     max_width;  /* Maximum frame width [pixel] */
  __u32     step_width; /* Frame width step size [pixel] */
  __u32     min_height; /* Minimum frame height [pixel] */
  __u32     max_height; /* Maximum frame height [pixel] */
  __u32     step_height;  /* Frame height step size [pixel] */
};

struct v4l2_frmsizeenum {
  __u32     index;    /* Frame size number */
  __u32     pixel_format; /* Pixel format */
  __u32     type;   /* Frame size type the device supports. enum v4l2_frmsizetypes*/

  union {         /* Frame size */
    struct v4l2_frmsize_discrete  discrete;
    struct v4l2_frmsize_stepwise  stepwise;
  };

  __u32   reserved[2];      /* Reserved space for future use */
};
#define V4L2_PIX_FMT_MJPEG    v4l2_fourcc('M', 'J', 'P', 'G') /* Motion-JPEG   */
#endif

typedef struct frame_size_struct
{
	unsigned int width;
	unsigned int height;
}framesize_list_t;
framesize_list_t framesize_supported[20];
/*VIDIOC_ENUM_FRAMESIZES*/
void print_framesize(int fd)
{
	struct v4l2_frmsizeenum framesize;
	int i = 0, ret = 0;

	printf("---------%s\n", __FUNCTION__);	
	for(;;i++)
	{
		memset(&framesize, 0, sizeof(framesize));
		framesize.index = i;
		framesize.pixel_format = V4L2_PIX_FMT_MJPEG;
		ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &framesize);
		if(ret < 0){
			//printf("ERROR:ioctl(VIDIOC_ENUM_FRAMESIZES):errno:%d, %s\n", errno, strerror(errno));
			if(ret == EINVAL)
				break;
			else
				return;
		}
		printf("framesize.index:%d, fourcc:%c%c%c%c framesize.type:%d V4L2_FRMSIZE_TYPE_DISCRETE:%d\n", framesize.index, (framesize.pixel_format & 0xff), (framesize.pixel_format >> 8) & 0xff, (framesize.pixel_format >> 16) & 0xff, (framesize.pixel_format >> 24) & 0xff, framesize.type, V4L2_FRMSIZE_TYPE_DISCRETE);
		printf("width:%d x height:%d\n", framesize.discrete.width, framesize.discrete.height);
		framesize_supported[i].width = framesize.discrete.width;
		framesize_supported[i].height = framesize.discrete.height;
	}//for(;;)
}
/**************************************************************************************************************************/
/*        VIDIOC_ENUM_FRAMEINTERVALS       */
/**************************************************************************************************************************/

#if 0
#define VIDIOC_ENUM_FRAMEINTERVALS _IOWR('V', 75, struct v4l2_frmivalenum)

/*
 *  F R A M E   R A T E   E N U M E R A T I O N
 */
enum v4l2_frmivaltypes {
  V4L2_FRMIVAL_TYPE_DISCRETE  = 1,
  V4L2_FRMIVAL_TYPE_CONTINUOUS  = 2,
  V4L2_FRMIVAL_TYPE_STEPWISE  = 3,
};

struct v4l2_frmival_stepwise {
  struct v4l2_fract min;    /* Minimum frame interval [s] */
  struct v4l2_fract max;    /* Maximum frame interval [s] */
  struct v4l2_fract step;   /* Frame interval step size [s] */
};

struct v4l2_frmivalenum {
  __u32     index;    /* Frame format index */
  __u32     pixel_format; /* Pixel format */
  __u32     width;    /* Frame width */
  __u32     height;   /* Frame height */
  __u32     type;   /* Frame interval type the device supports. */

  union {         /* Frame interval */
    struct v4l2_fract   discrete;
    struct v4l2_frmival_stepwise  stepwise;
  };

  __u32 reserved[2];      /* Reserved space for future use */
};

struct v4l2_fract {
  __u32   numerator;
  __u32   denominator;
};

#endif

void print_frameintervals(int fd)
{
	struct v4l2_frmivalenum fint;
	int i = 0, ret = 0, j = 0;

	printf("--------%s\n", __FUNCTION__);
	for(j = 0; framesize_supported[j].width; j++)
	{
		memset(&fint, 0, sizeof(fint));
		fint.width = framesize_supported[j].width;
    fint.height = framesize_supported[j].height;
		fint.pixel_format = V4L2_PIX_FMT_MJPEG;
		printf("fint.width:%d, fint.height:%d\n", fint.width, fint.height);
		for(i = 0;;i++)
		{
			fint.index = i;
			/*for each fixed width, and height we can get many FPS supported, by incrementing the index to get each FPS value*/
			ret = ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fint);
			if(ret < 0){
				//printf("ERROR:ioctl(VIDIOC_ENUM_FRAMESIZES):errno:%d, %s, EINVAL:%d\n", errno, strerror(errno), EINVAL);
				if(errno == EINVAL)
					break;
				else
					return;
			}
			printf("fint.discrete.numerator:%d, fint.discrete.denominator:%d\n", fint.discrete.numerator, fint.discrete.denominator);
		}//for(;;i++)
	}//for(; !framesize_supported[i].width; j++)	
}



int get_formats(int fd)
{
	print_enum_formats(fd);
	print_get_format(fd);
	print_framesize(fd);
	print_frameintervals(fd);	
	return 0;
}
/**************************************************************************************************************************/
/*start_streaming*/
/**************************************************************************************************************************/

int open_file(char *out)
{
	int fd_file;
	fd_file = open(out, O_CREAT | O_APPEND | O_RDWR, 0777);
	if(fd_file < 0)
		printf("ERROR:open(%s)\n", out);
#if 0
	char *sample_text = "This is sample text";
	write(fd, sample_text, strlen(sample_text));
#endif
	return fd_file;
}

char *mmap_virtual_input[10];
unsigned int mmap_size_input[10];


//The function to capture raw camera output
int ReadCameraInput(int fd, void **ptr, int *size,int *fb_index)
{
  //We reached here where camera is started
  //printf("Read the camera output now\n");
  struct v4l2_buffer dqbuf;
  int index  = -1;
  memset (&dqbuf, 0, sizeof dqbuf);
  dqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  dqbuf.memory = V4L2_MEMORY_MMAP;
  if (ioctl (fd, VIDIOC_DQBUF, &dqbuf) < 0) {
    printf("ERROR: VIDIOC_DQBUF Failed \n");
		return -1;
  }else {
    index = dqbuf.index;
    *ptr =  mmap_virtual_input[index];
    //*size  = dqbuf.bytesused;
		*size = dqbuf.length;
    *fb_index = index;
  }
	return 0;
}

//Function to queue back the buffer for new buffers
void EnqueCameraBuffer(int fd, int index)
{
  struct v4l2_buffer qbuf;
  memset (&qbuf, 0, sizeof qbuf);
  qbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  qbuf.memory = V4L2_MEMORY_MMAP;
  qbuf.index = index;
  qbuf.bytesused = 0;         /* enqueue it with no data */
  if (ioctl (fd, VIDIOC_QBUF, &qbuf) < 0) {
    printf("ERROR: VIDIOC_DQBUF Failed \n");
  }
}

#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 720
#define EXPECTED_SIZE ((FRAME_WIDTH* FRAME_HEIGHT*3)/2)

int start_streaming(int fd, int nframes, char *out, int fps)
{
	/*- VIDIOC_QUERYCAP - This is done to get to know:
					V4L2_CAP_STREAMING --> Supports MMAP and User Pointer
		- VIDIOC_CROPCAP - We can check the cropping capability
					VIDIOC_G_CROP, VIDIOC_S_CROP --> for getting and setting the crop values
		-	VIDIOC_ENUM_FMT - This can get us the supported Pixel Formats, indexing from 0
		- VIDIOC_G_FMT -> This is the current Pixel format, width, height, image size, bytesperline, color-space is got
		- VIDIOC_ENUM_FRAMESIZES -> This will give all the resolutions supported for a pixel format
		- VIDIOC_S_FMT -> We can set the format we need
		- VIDIOC_ENUM_FRAMEINTERVALS -> This is to get the FPS - Need to check how we can set the FPS

		1. VIDIOC_REQBUFS : We need to request the number of buffers needed, to be set in count field, which will be allocated in driver
		2. VIDIOC_QUERYBUF : Next we need to QUERYBUF to get the mmap offset, length to call mmap() to map the buffers
		3. VIDIOC_S_PARM : To set FPS mainly
		4. VIDIOC_G_PARM : to check is FPS is set correctly
		5. VIDIOC_QBUF : Queue all the buffers
		6. VIDIOC_STREAMON : Stat the streaming
		7. VIDIOC_DQBUF : to get the frame
		8. VIDIOC_QBUF : Queue back the buffers
	*/

	int fd_out;
	int number_output_buffer;	
	int i = 0;
	int ret = 0;
	
	if((fd_out = open_file(out)) < 0)
		return fd_out;

	//set format, width, height
	
#if 0
struct v4l2_requestbuffers {
  __u32     count;
  __u32     type;   /* enum v4l2_buf_type */
  __u32     memory;   /* enum v4l2_memory */
  __u32     reserved[2];
};
#define VIDIOC_REQBUFS    _IOWR('V',  8, struct v4l2_requestbuffers)
#endif
	struct v4l2_requestbuffers reqbuf;
	memset (&reqbuf, 0, sizeof reqbuf);
  reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  reqbuf.count = 1;
  reqbuf.memory = V4L2_MEMORY_MMAP;
  if (ioctl(fd, VIDIOC_REQBUFS, &reqbuf) < 0) {
    printf("ERROR: VIDIOC_REQBUFS Failed\n");
  }
	printf("after ioctl(VIDIOC_REQBUFS):reqbuf.count:%d\n", reqbuf.count);
	number_output_buffer = reqbuf.count;

#if 0
/**
 * struct v4l2_buffer - video buffer info
 * @index:  id number of the buffer
 * @type: enum v4l2_buf_type; buffer type (type == *_MPLANE for
 *    multiplanar buffers);
 * @bytesused:  number of bytes occupied by data in the buffer (payload);
 *    unused (set to 0) for multiplanar buffers
 * @flags:  buffer informational flags
 * @field:  enum v4l2_field; field order of the image in the buffer
 * @timestamp:  frame timestamp
 * @timecode: frame timecode
 * @sequence: sequence count of this frame
 * @memory: enum v4l2_memory; the method, in which the actual video data is
 *    passed
 * @offset: for non-multiplanar buffers with memory == V4L2_MEMORY_MMAP;
 *    offset from the start of the device memory for this plane,
 *    (or a "cookie" that should be passed to mmap() as offset)
 * @userptr:  for non-multiplanar buffers with memory == V4L2_MEMORY_USERPTR;
 *    a userspace pointer pointing to this buffer
 * @fd:   for non-multiplanar buffers with memory == V4L2_MEMORY_DMABUF;
 *    a userspace file descriptor associated with this buffer
 * @planes: for multiplanar buffers; userspace pointer to the array of plane
 *    info structs for this buffer
 * @length: size in bytes of the buffer (NOT its payload) for single-plane
 *    buffers (when type != *_MPLANE); number of elements in the
 *    planes array for multi-plane buffers
 *
 * Contains data exchanged by application and driver using one of the Streaming
 * I/O methods.
 */
struct v4l2_timecode {
  __u32 type;
  __u32 flags;
  __u8  frames;
  __u8  seconds;
  __u8  minutes;
  __u8  hours;
  __u8  userbits[4];
};

/**
 * struct v4l2_plane - plane info for multi-planar buffers
 * @bytesused:    number of bytes occupied by data in the plane (payload)
 * @length:   size of this plane (NOT the payload) in bytes
 * @mem_offset:   when memory in the associated struct v4l2_buffer is
 *      V4L2_MEMORY_MMAP, equals the offset from the start of
 *      the device memory for this plane (or is a "cookie" that
 *      should be passed to mmap() called on the video node)
 * @userptr:    when memory is V4L2_MEMORY_USERPTR, a userspace pointer
 *      pointing to this plane
 * @fd:     when memory is V4L2_MEMORY_DMABUF, a userspace file
 *      descriptor associated with this plane
 * @data_offset:  offset in the plane to the start of data; usually 0,
 *      unless there is a header in front of the data
 *
 * Multi-planar buffers consist of one or more planes, e.g. an YCbCr buffer
 * with two planes can have one plane for Y, and another for interleaved CbCr
 * components. Each plane can reside in a separate memory buffer, or even in
 * a completely separate memory node (e.g. in embedded devices).
 */
struct v4l2_plane {
  __u32     bytesused;
  __u32     length;
  union {
    __u32   mem_offset;
    unsigned long userptr;
    __s32   fd;
  } m;
  __u32     data_offset;
  __u32     reserved[11];
};

struct v4l2_buffer {
  __u32     index;
  __u32     type;
  __u32     bytesused;
  __u32     flags;
  __u32     field;
  struct timeval    timestamp;
  struct v4l2_timecode  timecode;
  __u32     sequence;

  /* memory location */
  __u32     memory;
  union {
    __u32           offset;
    unsigned long   userptr;
    struct v4l2_plane *planes;
    __s32   fd;
  } m;
  __u32     length;
  __u32     reserved2;
  __u32     reserved;
};
#define VIDIOC_QUERYBUF   _IOWR('V',  9, struct v4l2_buffer)
#endif	
	struct v4l2_buffer querybuf;
	memset(&querybuf, 0, sizeof querybuf);
  querybuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  querybuf.memory = V4L2_MEMORY_MMAP;
  for(i = 0; i < reqbuf.count; i++) {
    void *ptr;
    querybuf.index = i;

    /* Memory mapping for input buffers */
    if(ioctl(fd, VIDIOC_QUERYBUF, &querybuf) < 0) {
      printf("ERROR: VIDIOC_QUERYBUF Failed : %d and str %s\n",
                errno,strerror(errno));
    }

    ptr = mmap(NULL, querybuf.length, PROT_READ | PROT_WRITE,
            MAP_SHARED, fd, querybuf.m.offset);
    if (ptr == MAP_FAILED) {
      printf("ERROR: mmap Failed\n");
			return 0;
    }

    mmap_virtual_input[i] = ptr;
    mmap_size_input[i] = querybuf.length;
		printf("from query buf, ptr:%p length:%d\n", ptr, querybuf.length);;
  }

	struct v4l2_streamparm streamParams;
	
	memset(&streamParams, 0, sizeof(streamParams));
	streamParams.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  ret = ioctl(fd, VIDIOC_G_PARM, &streamParams);
        if (ret < 0) {
                printf("ERROR: VIDIOC_S_PARM filed on input\n");
                close(fd);
                return -1;
        }
  printf("The FPS default %d \n",streamParams.parm.capture.timeperframe.denominator);

#if 0
  streamParams.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
  //streamParams.parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;
  streamParams.parm.capture.timeperframe.denominator = fps;
	printf("TYRING FPS : %d ------\n", fps);
  streamParams.parm.capture.timeperframe.numerator= 1;
  ret = ioctl(fd, VIDIOC_S_PARM, &streamParams);
  if (ret < 0) {
    printf("ERROR: VIDIOC_S_PARM filed on input\n");
    close(fd);
    return -1;
  }
  memset(&streamParams, 0, sizeof(streamParams));
  ret = ioctl(fd, VIDIOC_G_PARM, &streamParams);
        if (ret < 0) {
                printf("ERROR: VIDIOC_S_PARM filed on input\n");
                close(fd);
                return -1;
        }
  printf("The FPS is configured as %d \n",streamParams.parm.capture.timeperframe.denominator);
#endif

	struct v4l2_buffer qbuf;
	memset(&qbuf, 0, sizeof querybuf);
  qbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  qbuf.memory = V4L2_MEMORY_MMAP;
  for(i = 0; i < number_output_buffer; i++) {
    qbuf.index = i;
    qbuf.bytesused = 0;         /* enqueue it with no data */
		//#define VIDIOC_QBUF   _IOWR('V', 15, struct v4l2_buffer)
    if (ioctl(fd, VIDIOC_QBUF, &qbuf) < 0) {
      close(fd);
      return -1;
    }
  }

	/* Start stream on input */
  unsigned int type_stream = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(ioctl(fd, VIDIOC_STREAMON, &type_stream) < 0) {
    printf("ERROR: Stream ON filed on input\n");
    close(fd);
    return -1;
  }
  printf("Started stream on Capture..\n");

	char *ptr_buf;
	int size; int index;
	char *sample_text = "This is sample text";

	while(nframes)
	{
		if(ReadCameraInput(fd, (void **)&ptr_buf, &size, &index) < 0)
		{
			printf("ERROR : ReadCameraInput\n");
			break;
		}
		printf("ptr_buf:%p size:%d, EXPECTED_SIZE:%d, (size-EXPECTED_SIZE):%d\n", ptr_buf, size, EXPECTED_SIZE, (size - EXPECTED_SIZE));
		ret = write(fd_out, ptr_buf, size);
  	//write(fd_out, sample_text, strlen(sample_text));
		fsync(fd_out);
		EnqueCameraBuffer(fd, index);
		nframes--;
	}

}

/**************************************************************************************************************************/
/**************************************************************************************************************************/

#define CAPTURE_FILE_NAME	"cap"
#define IMAGE_CAP	0
#define VIDEO_CAP 1

int get_out_file_name(char **out, int type)
{
	time_t t;
	struct tm *tm;
	
	t = time(NULL);
	tm = localtime(&t);
	
	*out = (char*)calloc(1, 100);
	sprintf(*out, "%s_%d_%d_%d_%d.%s", CAPTURE_FILE_NAME, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, type ? "mp4" : "jpg");
	printf("File Name generated : %s\n", *out);	
	return 0;
}
int main(int argc, char **argv)
{
	int fd = 0;
	int nframes = 1;
	char *out_file_name = NULL;
	int type = IMAGE_CAP;
	int fps = 30;

	if(argc > 1)
	{
		if(argc == 2)
		{
			//nframes given from user - so its a video capture
			nframes = atoi(argv[1]);
			printf("nframes :%d will be captured in a video file\n", nframes);
			type = VIDEO_CAP;
		}
		if(argc == 3)
		{
			fps = atoi(argv[2]);
			printf("fps to be set : %d\n", fps);
		}
	}
	get_out_file_name(&out_file_name, type);
	
	fd = open(DEVICE_FILE_VIDEO, O_RDWR);
	if(fd < 0)
	{
		printf("ERROR:open(%s):errno:%d, %s\n", DEVICE_FILE_VIDEO, errno, strerror(errno));
		return 0;
	}

	query_cap(fd);
	crop_cap(fd);
	//set_crop_cap(fd);
	get_formats(fd);

	start_streaming(fd, nframes, out_file_name, fps);
	
	return 0;
}
