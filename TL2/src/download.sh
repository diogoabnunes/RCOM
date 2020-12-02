#!/bin/sh

clear
make
echo ./download ftp://ftp.up.pt/pub/apache/README.html
echo
./download ftp://ftp.up.pt/pub/apache/README.html
echo
make clean