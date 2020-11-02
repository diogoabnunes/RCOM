#!/bin/sh

gnome-terminal --window-with-profile=RCOM -- sudo ./cable

echo "Sleeping for 10sec waiting for sudo password...";
sleep 10


make -s
if [ $? -eq 0 ] ; then
  gnome-terminal --window-with-profile=RCOM -- ./app recetor /dev/ttyS11 data.txt
  gnome-terminal --window-with-profile=RCOM -- ./app emissor /dev/ttyS10 pinguim.gif
else
  echo "MAKE ERROR";
fi

sleep 2
make clean