# Experience 3 questions

## 1. What routes are there in the tuxes? What are their meaning?
Após terem sido efetuadas as ligações dos computadores às Vlans, o tux43 deve ter uma
rota para a vlan 40, o tux42 deve ter uma para a vlan 41 e o tux44 deve ter rotas para
ambas as vlans. O gateway destas rotas será 0.0.0.0 (valor por defeito).
Para além das rotas mencionadas anteriormente, adicionam-se também novas rotas.
A primeira, através do comando “route add -net 172.16.41.0/24 gw 172.16.40.254. Esta
permite ao tux43 (172.16.40.1) comunicar com a vlan 41 (172.16.41.0) através do tux44
usado como gateway (172.16.40.254) graças ao ip forwarding.
Utiliza-se também o comando “route add -net 172.16.40.0/24 gw 172.16.41.253” semelhante
ao comando anterior. Este permite envio de ping ou mensagens no sentido inverso, criando
uma rota desde o tux42 (172.16.41.1) até à vlan 40 (172.16.40.0) passando pelo tux44
(172.16.41.253).

## 2. What information does an entry of the forwarding table contain?
Destination​ : destino da rota.
● Gateway​ : IP do próximo ponto por onde vai passar a rota.
● Netmask​ : serve para determinar o ID da rede a partir do endereço IP do destino.
● Flags​ : mais informação sobre a rota.
● Metric​ : custo de cada rota.
● Ref​ : número de referências para a rota.
● Use​ : contador de pesquisas pela rota:
●
○ usando -F, indica o número de falhas da cache.
○ usando -C, indica o número de sucessos da cache.
Interface​ : placa de rede usada para enviar mensagens.

## 3. What ARP messages, and associated MAC addresses, are observed and why?
As mensagens ARP são observadas quando é enviado um tux (chamemos-lhe tux1)
envia um ping a outro tux (tux2) e o tux1 não conhece o endereço MAC do tux2.
O tux1 envia uma mensagem ARP ao tux2 a pedir o seu endereço MAC (a partir do endereço IP de destino). Nesta mensagem, o endereço MAC do tux1 está associado a essa
mensagem, para que o tux2 saiba a quem responder: “ ​ Who has [​ tux2 IP ​ ]? Tell [​ tux1 IP ] ​ ”​ .
Esta mensagem é enviada em modo broadcast, o que significa que o endereço MAC do
recetor tem o valor de 00:00:00:00:00:00, uma vez que ainda não é conhecido.
Quando o tux2 recebe esta mensagem, responde através de uma mensagem ARP (já
não em modo broadcast, uma vez que este sabe para quem tem de enviar a resposta), onde
já está “completo” o campo inicialmente “vazio” com o seu endereço MAC: ​ “[​ tux2 IP ​ ] is at
[​ tux2 MAC ​ ]”​ .
Esta troca de mensagens acontece, por exemplo, da interface eth1 do tuxy4 para o
tuxy2.

## 4. What ICMP packets are observed and why?
São observados pacotes ICMP do tipo ​ request e ​ reply , ​ uma vez que, estando todas as
rotas adicionadas e bem configuradas, todos os tux’s se reconhecem uns aos outros. Caso
não fosse esse o caso, os pacotes ICMP seriam do tipo Host Unreachable.

## 5. What are the IP and MAC addresses associated to ICMP packets and why? 
Os endereços IP e MAC associados aos pacotes ICMP são os endereços IP dos tux’s
de origem e destino das máquinas que enviam/recebem esses pacotes.
Quando um pacote ICMP é enviado do tuxy3 para o tuxy4, os endereços (IP e MAC) de
origem são os do tuxy3 e os endereços de destino são os do tuxy4, uma vez que estes
estão conectados à mesma subrede (interface eth0).
Contudo, quando um pacote ICMP é enviado do tuxy3 para o tuxy2, já não é tão linear,
uma vez que não estão conectados à mesma subrede. Quando isto acontece, o tuxy4
funciona como intermediário, uma vez que está conectado às 2 VLANs. É enviado um
pacote ICMP do tuxy3 para o tuxy4 através da interface eth0, tal como explicado no
parágrafo anterior, uma vez que estão conectados pela VLAN. Como o tuxy4 está
conectado com o tuxy2 pela outra VLAN, este reencaminha o pacote ICMP do tuxy3 para o
tuxy2, mantendo os endereços IP, mas alterando o endereço MAC de origem para o da
interface eth1 do tuxy4 (interface conectada à VLAN onde comunica com o tuxy2) e o
endereço MAC de destino para o endereço MAC do tuxy2.
