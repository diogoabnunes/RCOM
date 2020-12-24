#!/bin/sh

make
echo ./download ftp://ftp.up.pt/pub/apache/HEADER.html
./download ftp://ftp.up.pt/pub/apache/HEADER.html
make clean
