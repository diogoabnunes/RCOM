#!/bin/bash

# Configuração de IPs
ifconfig eth0 up
ifconfig eth0 172.16.40.1/24
ifconfig eth0

# Configuração de VLANs (GtkTerm)
    # configure terminal
    # vlan 40
    # end

    # configure terminal
    # interface fastethernet 0/3 #(0/3 porque o tux43 está ligado na porta 3, depende da ligação dos cabos)
    # switchport mode access
    # switchport access vlan 40
    # end

    # configure terminal
    # interface fastethernet 0/13 #(0/13 porque o tux44 está ligado na porta 13)
    # switchport mode access
    # switchport access vlan 40
    # end

    # configure terminal
    # vlan 41
    # end

    # configure terminal
    # interface fastethernet 0/1 #(0/1 porque o tux42 está ligado na porta 1)
    # switchport mode access
    # switchport access vlan 41
    # end

# Configuração de um router em Linux
    # configure terminal
    # interface fastethernet 0/13
    # switchport mode access
    # switchport access vlan 41
    # end

route add -net 172.16.41.0/24 gw 172.16.40.254

route -n