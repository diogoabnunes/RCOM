# Logs (Total: 15)

## Exp1: 1 Log
* Confirmar 2 mensagens ARP
    * Who has 172.16.40.254? Tell 172.16.40.1
    * 172.16.40.254 is at [tux44 MAC]
* Confirmar IP de origem: 172.16.40.1
* Confirmar IP de destino: 172.16.40.254
* Confirmar MAC de origem: 
* Confirmar MAC de destino: 
* Confirmar pings de request e reply entre 172.16.40.1 e 172.16.40.254

## Exp2: 4 Logs
* in tuxy3, ping tuxy4 and then ping tuxy2
* in tuxy3, do ping broadcast (ping -b 172.16.y0.255) for a few seconds (3 logs, 1 em cada tuxy (2/3/4))

## Exp3: 3 Logs
* From tuxy3, ping the other network interfaces (172.16.y0.254, 172.16.y1.253, 172.16.y1.1) and verify if there is connectivity.
* In tuxy4, 2 instances of Wireshark: eth0 e eth1

## Exp4: 4 Logs
* in tuxy3, ping tuxy2, eth0 tuxy4 and eth1 tuxy4
* in tuxy2, start capture at eth0. 
* in tuxy3, ping router (é suposto n funcionar porque precisa de NAT)
* in tuxy3, ping router

## Exp5: 1 Log
* ping ftp.up.pt
* ping google.com

## Exp6: 2 Logs
* in tuxy3, start capturing and run download app
* run app on tuxy3 again, butin the middle of the download, start a new one on tuxy2