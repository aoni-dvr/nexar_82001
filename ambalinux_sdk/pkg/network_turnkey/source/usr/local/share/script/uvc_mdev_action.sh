#!/bin/sh

if [ "$ACTION" == "add" ]; then
	if [ "`pidof mjpg_streamer`" != "" ]; then
		exit 0
	fi
	mjpg_streamer -i "/usr/lib/mjpg-streamer/input_uvc.so" -o "/usr/lib/mjpg-streamer/output_http.so -p 8090 -w /usr/share/mjpg-streamer/www" &
else
	killall -9 mjpg_streamer
fi
