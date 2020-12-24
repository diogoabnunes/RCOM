# Experience 1 questions

## 1. What are the ARP packets and what are they used for?
O ARP (Address Resolution Protocol) é um protocolo de comunicação cuja função é
mapear um endereço de rede IP de uma máquina a um endereço físico MAC. Assim sendo,
os pacotes ARP são usados para que o endereço IP do utilizador comunique com o
endereço físico MAC correspondente ao endereço IP do destinatário.

## 2. What are the MAC and IP addresses of ARP packets and why?
Como a entrada da tabela ARP referente ao tuxy4 foi apagada, ao tentar enviar um
pacote de tuxy3 para tuxy4, o tuxy3 não sabe o endereço MAC associado ao IP do tuxy4.
Assim, o tuxy3 envia um pacote ARP em broadcast (para toda a rede local) com o seu
endereço IP (172.16.40.1) e endereço MAC (00:21:5a:61:2f:13). O endereço MAC do tuxy4
não se sabe, por isso tem o valor de 00:00:00:00:00:00.
Por conseguinte, o tuxy4 envia um pacote ARP para o tuxy3 com os seus respectivos
endereços IP (172.16.40.254) e MAC (00:21:5a:c3:78:76).
Conclui-se que os pacotes ARP contêm os endereços IP e MAC das máquinas de
origem e de destino dos pacotes em questão.

## 3. What packets does the ping command generate?
Este comando gera inicialmente pacotes ARP para obter o endereço MAC do
destinatário, seguidos de pacotes ICMP (Internet Control Message Protocol).

## 4. What are the MAC and IP addresses of the ping packets?
Ao efetuar um ping do tuxy3 para o tuxy4, os endereços MAC e IP de origem e destino
dos pacotes são: (página seguinte)
Pacotes do pedido Origem Destinatário
Endereço IP 172.16.40.1 172.16.40.254
Endereço MAC 00:21:5a:61:2f:13 00:21:5a:c3:78:76
Pacotes da resposta Origem Destinatário
Endereço IP 172.16.40.254 172.16.40.1
Endereço MAC 00:21:5a:c3:78:76 00:21:5a:61:2f:13

## 5. How to determine if a receiving Ethernet frame is ARP, IP, ICMP?
Pode-se determinar uma trama do tipo Ethernet a partir dos bytes Type dessa trama:
* caso este tenha o valor 0x0806, é uma trama do tipo ARP;
* caso este tenha o valor 0x0800, é uma trama do tipo ICMP.

## 6. How to determine the length of a receiving frame?
O comprimento de uma trama recebida é visível através do Wireshark.

## 7. What is the loopback interface and why is it important?
A interface loopback é uma interface virtual da rede que serve para verificar se a rede
está corretamente configurada: é enviado um pacote de um endereço para um endereço de destino igual, se este for recebido então a rede está organizada.
