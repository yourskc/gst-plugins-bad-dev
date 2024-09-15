#!/bin/bash
gst-launch-1.0 playbin uri=file:///home/skc/gst-work/gst-plugins-bad-dev/test_env/endo01.mp4 video-sink="videoconvert ! equirectangular ! videoconvert ! autovideosink"
