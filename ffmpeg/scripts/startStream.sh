#!/bin/bash
cd /usr/local 

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64

export PATH=$PATH:/usr/local/bin

# Here goes the big command
ffmpeg