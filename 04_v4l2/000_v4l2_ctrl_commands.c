##########################################################################################
1. To list video devices

v4l2-ctl --list-devices
HD Web Camera: HD Web Camera (usb-0000:00:14.0-8.4):
        /dev/video0
        /dev/video1

- Next we need to find which device is what, like capture/output, and format supported
##########################################################################################
2. To list the formats supported

v4l2-ctl --list-formats
>> By default the video0 is considered, to change to specific video device : -d0, -d1

-----------------------------
v4l2-ctl -d0 --list-formats
				OR
v4l2-ctl --device 0 --list-formats

ioctl: VIDIOC_ENUM_FMT
        Type: Video Capture

        [0]: 'MJPG' (Motion-JPEG, compressed)
        [1]: 'NV12' (Y/CbCr 4:2:0)
        [2]: 'H264' (H.264, compressed)

- We got to know the pixel formats supported, next we need to check the resoultion
supported
##########################################################################################
3. To get the resolution and FPS supported

v4l2-ctl -d0 --list-formats-ext

ioctl: VIDIOC_ENUM_FMT
        Type: Video Capture

        [0]: 'MJPG' (Motion-JPEG, compressed)
                Size: Discrete 1920x1080
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x960
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x720
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 800x600
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 640x480
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 640x320
                        Interval: Discrete 0.033s (30.000 fps)
        [1]: 'NV12' (Y/CbCr 4:2:0)
                Size: Discrete 1920x1080
                        Interval: Discrete 0.200s (5.000 fps)
                Size: Discrete 1280x960
                        Interval: Discrete 0.100s (10.000 fps)
                Size: Discrete 1280x720
                        Interval: Discrete 0.067s (15.000 fps)
                Size: Discrete 800x600
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 640x480
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 640x320
                        Interval: Discrete 0.033s (30.000 fps)
        [2]: 'H264' (H.264, compressed)
                Size: Discrete 1920x1080
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x960
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x720
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 800x600
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 640x480
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 640x320
                        Interval: Discrete 0.033s (30.000 fps)

- Next for video device-1, we do not get anything, it may be used for META-DATA only
>> Need to check more and validate on this
>>>> yes its right, the -d1 is for meta-capture, can verify in "v4l2-ctl -d1 --info"

v4l2-ctl -d1 --list-formats-ext
ioctl: VIDIOC_ENUM_FMT
        Type: Video Capture

##########################################################################################
4. To check the setting/parameters set for the video device - we can check with -V

v4l2-ctl -V

Format Video Capture:
        Width/Height      : 1920/1080
        Pixel Format      : 'MJPG' (Motion-JPEG)
        Field             : None
        Bytes per Line    : 0
        Size Image        : 1572864
        Colorspace        : Default
        Transfer Function : Default (maps to Rec. 709)
        YCbCr/HSV Encoding: Default (maps to ITU-R 601)
        Quantization      : Default (maps to Full Range)
        Flags             :

-V, --get-fmt-video
                     query the video capture format [VIDIOC_G_FMT]

##########################################################################################
5. To get the driver info and driver capabilities

v4l2-ctl -d0 --info

Driver Info:
        Driver name      : uvcvideo
        Card type        : HD Web Camera: HD Web Camera
        Bus info         : usb-0000:00:14.0-8.4
        Driver version   : 5.15.149
        Capabilities     : 0x84a00001
                Video Capture
                Metadata Capture
                Streaming
                Extended Pix Format
                Device Capabilities
        Device Caps      : 0x04200001 ----------> different
                Video Capture ----> Note this is a "Video capture" device
                Streaming
                Extended Pix Format

-------------------------------------------
- Notice that for -d1 the "Device Caps" is different

v4l2-ctl -d1 --info

Driver Info:
        Driver name      : uvcvideo
        Card type        : HD Web Camera: HD Web Camera
        Bus info         : usb-0000:00:14.0-8.4
        Driver version   : 5.15.149
        Capabilities     : 0x84a00001
                Video Capture
                Metadata Capture
                Streaming
                Extended Pix Format
                Device Capabilities
        Device Caps      : 0x04a00000 -----------> different
                Metadata Capture ----> Note this is a "Metadata Capture" device
                Streaming
                Extended Pix Format

##########################################################################################
6. To get all the info in one-shot we can do "--all"

v4l2-ctl -d0 --all

Driver Info:
        Driver name      : uvcvideo
        Card type        : HD Web Camera: HD Web Camera
        Bus info         : usb-0000:00:14.0-8.4
        Driver version   : 5.15.149
        Capabilities     : 0x84a00001
                Video Capture
                Metadata Capture
                Streaming
                Extended Pix Format
                Device Capabilities
        Device Caps      : 0x04200001
                Video Capture
                Streaming
                Extended Pix Format
Priority: 2
Video input : 0 (Input 1: ok)
Format Video Capture:
        Width/Height      : 1920/1080
        Pixel Format      : 'MJPG' (Motion-JPEG)
        Field             : None
        Bytes per Line    : 0
        Size Image        : 1572864
        Colorspace        : Default
        Transfer Function : Default (maps to Rec. 709)
        YCbCr/HSV Encoding: Default (maps to ITU-R 601)
        Quantization      : Default (maps to Full Range)
        Flags             :
Crop Capability Video Capture:
        Bounds      : Left 0, Top 0, Width 1920, Height 1080
        Default     : Left 0, Top 0, Width 1920, Height 1080
        Pixel Aspect: 1/1
Selection Video Capture: crop_default, Left 0, Top 0, Width 1920, Height 1080, Flags:
Selection Video Capture: crop_bounds, Left 0, Top 0, Width 1920, Height 1080, Flags:
Streaming Parameters Video Capture:
        Capabilities     : timeperframe
        Frames per second: 30.000 (30/1)
        Read buffers     : 0

User Controls

                     brightness 0x00980900 (int)    : min=1 max=255 step=1 default=128 value=128
                       contrast 0x00980901 (int)    : min=1 max=200 step=1 default=128 value=128
                     saturation 0x00980902 (int)    : min=1 max=255 step=1 default=128 value=128
        white_balance_automatic 0x0098090c (bool)   : default=1 value=1

-----------------------------
v4l2-ctl -d1 --all

Driver Info:
        Driver name      : uvcvideo
        Card type        : HD Web Camera: HD Web Camera
        Bus info         : usb-0000:00:14.0-8.4
        Driver version   : 5.15.149
        Capabilities     : 0x84a00001
                Video Capture
                Metadata Capture
                Streaming
                Extended Pix Format
                Device Capabilities
        Device Caps      : 0x04a00000
                Metadata Capture
                Streaming
                Extended Pix Format
Priority: 2
Format Metadata Capture:
        Sample Format   : 'UVCH' (UVC Payload Header Metadata)
        Buffer Size     : 1024

##########################################################################################
7. To get all the user-controls

v4l2-ctl -l
v4l2-ctl --list-ctrls
v4l2-ctl -L
v4l2-ctl --list-ctrls-menus

User Controls

                     brightness 0x00980900 (int)    : min=1 max=255 step=1 default=128 value=128
                       contrast 0x00980901 (int)    : min=1 max=200 step=1 default=128 value=128
                     saturation 0x00980902 (int)    : min=1 max=255 step=1 default=128 value=128
        white_balance_automatic 0x0098090c (bool)   : default=1 value=1

- For these controls to be set and get
-C, --get-ctrl
-c, --set-ctrl

##########################################################################################
8. Need to check on the usage of :

--log-status
              Log the board status in the kernel log [VIDIOC_LOG_STATUS].

##########################################################################################
9. To get the help menu of the video capture options
v4l2-ctl --help-vidcap

##########################################################################################
10. To get the Framesizes/Resolution supported for a "particular Pixel Format"

v4l2-ctl -d0 --list-framesizes 0
ioctl: VIDIOC_ENUM_FRAMESIZES
        Size: Discrete 1920x1080
        Size: Discrete 1280x960
        Size: Discrete 1280x720
        Size: Discrete 800x600
        Size: Discrete 640x480
        Size: Discrete 640x320

>> Notice that we have given the "Pixel format index as 0", to get the list of pixel
formats and their index

v4l2-ctl --list-formats
ioctl: VIDIOC_ENUM_FMT
        Type: Video Capture

        [0]: 'MJPG' (Motion-JPEG, compressed)
        [1]: 'NV12' (Y/CbCr 4:2:0)
        [2]: 'H264' (H.264, compressed)

##########################################################################################
11. To get the FPS for a particular "Resolution" and "Pixel Format"

--list-frameintervals width=<w>,height=<h>,pixelformat=<f>
                     list supported frame intervals for pixelformat <f> and
                     the given width and height [VIDIOC_ENUM_FRAMEINTERVALS]
                     pixelformat is the fourcc value as a string


##########################################################################################
12. To check if the fields is supported

--list-fields      list supported fields for the current format

##########################################################################################
13. To set video format 

-v, --set-fmt-video
  --try-fmt-video width=<w>,height=<h>,pixelformat=<pf>,field=<f>,colorspace=<c>,
                  xfer=<xf>,ycbcr=<y>,hsv=<hsv>,quantization=<q>,
                  premul-alpha,bytesperline=<bpl>
                     set/try the video capture format [VIDIOC_S/TRY_FMT]
                     pixelformat is either the format index as reported by
                       --list-formats, or the fourcc value as a string.
                     The bytesperline option can be used multiple times, once for each plane 
##########################################################################################
14. v4l2-ctl --help-streaming

##########################################################################################
# viewfinder
v4l2-ctl --overlay=1 # enable viewfinder
v4l2-ctl --overlay=0 # disable viewfinder

# record video
v4l2-ctl --set-fmt-video=width=1920,height=1088,pixelformat=4
v4l2-ctl --stream-mmap=3 --stream-count=100 --stream-to=somefile.264

# capture jpeg
v4l2-ctl --set-fmt-video=width=2592,height=1944,pixelformat=3
v4l2-ctl --stream-mmap=3 --stream-count=1 --stream-to=somefile.jpg

# set bitrate
v4l2-ctl --set-ctrl video_bitrate=10000000


##########################################################################################


##########################################################################################


##########################################################################################

##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################

##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################

##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################

##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################

##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


