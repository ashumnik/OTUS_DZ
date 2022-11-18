#!/bin/sh

GST_PLUGIN_PATH=build gst-launch-1.0 wavplayer filepath=files/test.wav ! "audio/x-raw,format=S16LE,rate=48000" ! autoaudiosink