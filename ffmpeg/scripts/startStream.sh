#!/bin/bash
cd /usr/local 

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64

export PATH=$PATH:/usr/local/bin

while getopts p:s: flag
do 
  case "${flag}" in
    p)   PASSPHRASE=${OPTARG};;
    s)       SOURCE=${OPTARG};;
    esac
done

echo $SOURCE;
echo $PASSPHRASE;


# Check if ffmpeg running, if running kill it
SERVICE="ffmpeg"
if ps -C $SERVICE >/dev/null
then
    echo "$SERVICE is running. Killing it...."
    ps xao pgid,comm | grep ffmpeg | awk '{print $1}' | xargs -i kill -9 -- -{}
    echo "$SERVICE is killed........."
fi


if [[ $SOURCE = decklink ]]
then
  ffmpeg -sources decklink
  if [ -z "${PASSPHRASE}" ]
  then
    until ffmpeg -format_code Hp25 -f decklink -i 'DeckLink Studio 2' -c:v libx265 -an -crf 27 -preset ultrafast -tune zerolatency -bsf:v hevc_mp4toannexb -f mpegts 'srt://0.0.0.0:7000?pkt_size=1316&mode=listener&nakreport=1&listen_timeout=-1'; do
      echo "Stream disrupted, retrying in 3 seconds..."
      sleep 3
    done
  else
    until ffmpeg -format_code Hp25 -f decklink -i 'DeckLink Studio 2' -c:v libx265 -an -crf 27 -preset ultrafast -tune zerolatency -bsf:v hevc_mp4toannexb -f mpegts "srt://0.0.0.0:7000?pkt_size=1316&mode=listener&nakreport=1&listen_timeout=-1&pbkeylen=16&passphrase=$PASSPHRASE"; do
      echo "Stream disrupted, retrying in 3 seconds..."
      sleep 3
    done
  fi
else
  if [ -z "${PASSPHRASE}" ] 
  then
    until ffmpeg -re -loop 1 -f image2pipe -i ./sources/$SOURCE -c:v libx265 -an -crf 27 -preset ultrafast -tune zerolatency -bsf:v hevc_mp4toannexb -f mpegts 'srt://0.0.0.0:7000?pkt_size=1316&mode=listener&nakreport=1&listen_timeout=-1'; do
      echo "Stream disrupted, retrying in 3 seconds..."
      sleep 3
    done
  else
    until ffmpeg -re -loop 1 -f image2pipe -i ./sources/$SOURCE -c:v libx265 -an -crf 27 -preset ultrafast -tune zerolatency -bsf:v hevc_mp4toannexb -f mpegts "srt://0.0.0.0:7000?pkt_size=1316&mode=listener&nakreport=1&listen_timeout=-1&pbkeylen=16&passphrase=$PASSPHRASE"; do
      echo "Stream disrupted, retrying in 3 seconds..."
      sleep 3
    done
  fi
fi