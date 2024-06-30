###########################################################################################
01. open(/dev/video0)
02. ioctl(VIDIOC_QUERYCAP) --> driver/device capabilities - CAPTURE/OUTPUT/MPLANE/M2M
03. ioctl(VIDIOC_CROPCAP)
04. ioctl(VIDIOC_ENUM_FMT) --> Pixel formats
05. ioclt(VIDIOC_G_FMT) --> Pixel format, resolution, stride
06. ioctl(VIDIOC_ENUM_FRAMESIZES) --> resolution for a particault pixel format
07. ioctl(VIDIOC_ENUM_FRAMEINTERVALS) --> FPS for a resolution and pixel format

08. ioctl(VIDIOC_REQBUFS)
struct v4l2_requestbuffers reqbuf;
reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
reqbuf.count = 1;
reqbuf.memory = V4L2_MEMORY_MMAP;
ioctl(fd, VIDIOC_REQBUFS, &reqbuf)
number_output_buffer = reqbuf.count;

09. ioctl(VIDIOC_QUERYBUF)
struct v4l2_buffer querybuf;
querybuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
querybuf.memory = V4L2_MEMORY_MMAP;
for(i = 0; i < reqbuf.count; i++) {
    void *ptr;
    querybuf.index = i;

    /* Memory mapping for input buffers */
    ioctl(fd, VIDIOC_QUERYBUF, &querybuf);

    ptr = mmap(NULL, querybuf.length, PROT_READ | PROT_WRITE,
            MAP_SHARED, fd, querybuf.m.offset);

    mmap_virtual_input[i] = ptr;
    mmap_size_input[i] = querybuf.length;
}

10. ioctl(VIDIOC_G_PARM) --> for FPS
struct v4l2_streamparm streamParams;
streamParams.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
ioctl(fd, VIDIOC_G_PARM, &streamParams);
printf("The FPS default %d \n",streamParams.parm.capture.timeperframe.denominator);

11. ioctl(VIDIOC_QBUF)
struct v4l2_buffer qbuf;
qbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
qbuf.memory = V4L2_MEMORY_MMAP;
for(i = 0; i < number_output_buffer; i++) {
    qbuf.index = i;
    qbuf.bytesused = 0;         /* enqueue it with no data */
    //#define VIDIOC_QBUF   _IOWR('V', 15, struct v4l2_buffer)
    ioctl(fd, VIDIOC_QBUF, &qbuf);
}

12. ioctl(VIDIOC_STREAMON) 
unsigned int type_stream = V4L2_BUF_TYPE_VIDEO_CAPTURE;
ioctl(fd, VIDIOC_STREAMON, &type_stream);

13. ioctl(VIDIOC_DQBUF)
struct v4l2_buffer dqbuf;
dqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
dqbuf.memory = V4L2_MEMORY_MMAP;
ioctl (fd, VIDIOC_DQBUF, &dqbuf);
index = dqbuf.index;
*ptr =  mmap_virtual_input[index];
*size = dqbuf.length;
*fb_index = index;

- At this point we can directly read the mmaped buffer to get the captured data
- save it to a file

14. ioctl(VIDIOC_QBUF)
struct v4l2_buffer qbuf;
qbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
qbuf.memory = V4L2_MEMORY_MMAP;
qbuf.index = index; ---------------------> Q BACK the SAME buffer, which was DEQUEUED
qbuf.bytesused = 0;         /* enqueue it with no data */
ioctl(fd, VIDIOC_QBUF, &qbuf);


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################

###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################

###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################

###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################

###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


