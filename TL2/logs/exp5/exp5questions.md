# Experience 5 questions

## 1. How to configure the DNS service at an host?
De forma a configurar o serviço DNS no servidor do laboratório, é necessário mudar o
ficheiro resolv.conf no diretório /etc/. Este ficheiro tem de conter a seguinte informação:
● search netlab.fe.up.pt
● nameserver 172.16.2.1
O nome do servidor DNS é netlab.fe.up.pt e 172.16.2.1 é o seu endereço IP.

## 2. What packets are exchanged by DNS and what information is transported
O host envia um pacote para o servidor com o hostname, pedindo o seu endereço IP.
Como pedido, o servidor responde com um pacote que contém o endereço IP do hostname.