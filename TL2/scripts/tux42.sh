#!/bin/bash

# Configuração de IP: Exp1
ifconfig eth0 up
ifconfig eth0 172.16.41.1/24
ifconfig eth0 

route add -net 172.16.40.0/24 gw 172.16.41.253

route add -net 172.16.1.0/24 gw 172.16.41.254

route add default gw 172.16.41.254

route -n