#!/bin/sh

clear
make
echo ./download ftp://ftp.up.pt/pub/apache/HEADER.html
echo
./download ftp://ftp.up.pt/pub/apache/HEADER.html
echo
make clean