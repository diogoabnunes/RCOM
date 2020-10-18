#!/bin/sh

gnome-terminal --window-with-profile=RCOM_socat -- sudo socat -d  -d  PTY,link=/dev/ttyS10,mode=777   PTY,link=/dev/ttyS11,mode=777

sleep 5
cd src
gnome-terminal --window-with-profile=RCOM_recetor -- ./recetor.sh
gnome-terminal --window-with-profile=RCOM_emissor -- ./emissor.sh