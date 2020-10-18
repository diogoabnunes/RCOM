#!/bin/sh

# corre num novo terminal o socat
gnome-terminal --window-with-profile=RCOM_socat -- sudo socat -d  -d  PTY,link=/dev/ttyS10,mode=777   PTY,link=/dev/ttyS11,mode=777
# tempo para dar input
sleep 5

cd src
make -s
if [ $? -eq 0 ] ; then
  gnome-terminal --window-with-profile=RCOM_recetor -- ./recetor /dev/ttyS10
  gnome-terminal --window-with-profile=RCOM_emissor -- ./emissor /dev/ttyS11
else
  echo "MAKE ERROR";
fi

sleep 2
make clean