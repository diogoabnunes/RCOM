#!/bin/sh

if gcc emissor.c -o -Wall emissor; then
./emissor /dev/ttyS11;

else
echo "COMPILATION ERROR";
fi

rm emissor