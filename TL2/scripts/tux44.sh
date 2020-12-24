#!/bin/bash

ifconfig eth0 up
ifconfig eth0 172.16.40.254/24

ifconfig eth1 up
ifconfig eth1 172.16.41.253/24

echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

route add -net 172.16.1.0/24 gw 172.16.41.254
route add default gw 172.16.41.254