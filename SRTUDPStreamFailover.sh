until ffmpeg -format_code Hp25 -f decklink -i 'DeckLink Studio 2' -c:v libx265 -an -crf 27 -preset ultrafast -tune zerolatency -bsf:v hevc_mp4toannexb -f mpegts - | ffmpeg -f mpegts -i - -c copy -f mpegts 'srt://0.0.0.0:7000?pkt_size=1316&mode=listener&nakreport=1&listen_timeout=-1' -c copy -f mpegts 'udp://224.2.2.2:8888?pkt_size=1316&localaddr=192.168.1.31' ; do
  echo Stream disrupted, retrying in 3 seconds...
  sleep 3
done
