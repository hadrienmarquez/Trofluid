#!/bin/bash

# Check if ffmpeg running, if running kill it
SERVICE="ffmpeg"
if ps -C $SERVICE >/dev/null
then
    echo "$SERVICE is running. Killing it...."
    ps xao pgid,comm | grep ffmpeg | awk '{print $1}' | xargs -i kill -9 -- -{}
    echo "$SERVICE is killed........."
fi