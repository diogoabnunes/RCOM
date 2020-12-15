#!/bin/bash

# Configuração de IP: Exp1
ifconfig eth0 172.16.40.254/24

# Configuração de tuxy4.eth1
ifconfig eth1 172.16.41.253/24

# Enable IP forwarding
echo 1 > /proc/sys/net/ipv4/ip_forward
# Disable icmp echo-ignore-broadcasts
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

route -n