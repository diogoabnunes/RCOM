#!/bin/sh

gnome-terminal --window-with-profile=RCOM_socat -- sudo ./cable

echo "Sleeping for 10sec waiting for sudo password...";
sleep 10

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