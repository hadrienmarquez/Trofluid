#!/bin/bash

ps -aux | grep ffmpeg | awk '{print $2}' | xargs -i kill -9 {}