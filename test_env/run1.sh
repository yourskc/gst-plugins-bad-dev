#!/bin/bash
gst-launch-1.0 -e filesrc location=./endo01.mp4 ! qtdemux ! queue ! \
h264parse ! video/x-h264, stream-format=avc,alignment=au ! rtph264pay pt=96 name=pay0 \
config-interval=3 mtu=6000 ! udpsink host=192.168.0.105 port=5000