#!/bin/sh

if gcc recetor.c -o -Wall recetor; then
./recetor /dev/ttyS11;

else
echo "COMPILATION ERROR";
fi

rm recetor