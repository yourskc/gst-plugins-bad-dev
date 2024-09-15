#!/bin/bash
gst-launch-1.0 -e filesrc location=./endo01.mp4 ! qtdemux ! queue ! \
h264parse ! omxh264dec ! videoconvert ! equirectangular ! videoconvert ! omxh264enc ! rtph264pay config-interval=10 ! udpsink host=192.168.0.105 port=5000