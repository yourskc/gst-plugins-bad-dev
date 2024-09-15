#!/bin/bash
gst-launch-1.0 -e filesrc location=./endo01.mp4 ! qtdemux ! queue ! \
h264parse ! omxh264dec ! videoconvert ! equirectangular ! videoconvert ! autovideosink