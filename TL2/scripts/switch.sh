#!/bin/bash

configure terminal
vlan 40
interface fastethernet 0/1
switchport mode access
switchport access vlan 40
exit

interface fastethernet 0/2
switchport mode access
switchport access vlan 40
exit

vlan 41
interface fastethernet 0/3
switchport mode access
switchport access vlan 41
exit

interface fastethernet 0/4
switchport mode access
switchport access vlan 41
exit

interface fastethernet 0/5
switchport mode access
switchport access vlan 41
end