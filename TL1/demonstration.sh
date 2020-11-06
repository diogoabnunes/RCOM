#!/bin/sh

gnome-terminal --window-with-profile=RCOM -- sudo ./src/cable

echo "Sleeping for 10sec waiting for sudo password...";
sleep 5

cd src
make -s
if [ $? -eq 0 ] ; then
  gnome-terminal --window-with-profile=RCOM -- ./app recetor /dev/ttyS11 ../docs_recetor/
  gnome-terminal --window-with-profile=RCOM -- ./app emissor /dev/ttyS10 pinguim.gif
else
  echo "MAKE ERROR";
fi

sleep 2
make clean