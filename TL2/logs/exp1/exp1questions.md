# Experience 1 questions

## 1. What are the ARP packets and what are they used for?
ARP stands for Address Resolution Protocol. ARP packets are used to get a MAC address, given an IP address. The RARP (Reverse ARP) does the opposite (gives an IP given a MAC).

## 2. What are the MAC and IP addresses of ARP packets and why?
MAC stands for Medium Access Control, and IP is Internet Protocol. An IP address is a public identifier as a numerical label used by devices to communicate in a network using the Internet Protocol. A MAC address is a unique identifier that every network card has. It is used as a network address in the data link layer.

## 3. What packets does the ping command generate?
The ping command sends Internet Control Message Protocol (ICMP) packets. It is used to evaluate the reachability of a host on the internet. The ping command measures the time for a round trip of a packet, reports packet loss, statistics, and errors.

## 4. What are the MAC and IP addresses of the ping packets?
The IP and MAC addresses of the source and destination of ping request.

## 5. How to determine if a receiving Ethernet frame is ARP, IP, ICMP?
In Ethernet II, by verifying the 2 bytes in the EtherType field right before the payload. The value there encodes a protocol (0x0800 for IPv4, 0x0806 for ARP, etc).

## 6. How to determine the length of a receiving frame?
In IEEE 802.3, the EtherType field represents the length. In Ethernet II, the physical layer detects the end of a frame. In any case, it can be seen on Wireshark and on the terminal after executing the "ping" command.

## 7. What is the loopback interface and why is it important?
The loopback interface is used to test if the network is well structured. The device sends itself a packet, which goes through every layer, and then returns.
