# Experience 4 questions

## 1. How to configure a static route in a commercial router?
Para adicionar uma rota ao router, recorremos aos seguintes comandos (no GTKTerm):
● configure terminal
● interface fastethernet 0/0
● ip address 172.16.41.254 255.255.255.0
● no shutdown
● ip nat inside
● exit
● interface fastethernet 0/1
● ip address 172.16.2.49 255.255.255.0
● no shutdown
● ip nat outside
● exit
● ip nat pool ovrld 172.16.2.49 172.16.2.49 prefix 24
● ip nat inside source list 1 pool ovrld overload
● access-list 1 permit 172.16.40.0 0.0.0.7
● access-list 1 permit 172.16.41.0 0.0.0.7
● ip route 0.0.0.0 0.0.0.0 172.16.2.254
● ip route 172.16.40.0 255.255.255.0 172.16.41.253
● end

## 2. What are the paths followed by the packets in the experiments carried out and why?
No caso da rota já existir, os pacotes seguem essa mesma rota. Caso não exista, os
pacotes vão ao router (rota caso não exista nenhuma definida).
No passo 4 desta experiência, aplicamos os comandos no tuxy2:
●
echo 0 > /proc/sys/net/ipv4/conf/eth0/accept_redirects
●
echo 0 > /proc/sys/net/ipv4/conf/all/accept_redirects
Estes comandos servem para que o tux não guarde os redirecionamentos na mesma
interface de rede. Para além destes comandos, foram adicionadas rotas por defeito para o
router RC no tuxy4 e no tuxy2.
O tuxy3 comunica com o tuxy4 através da vlany0. O tuxy4, tuxy2 e o router estão
ligados à vlany1, e o router é o único que sabe que para chegar ao tuxy3

## 3. How to configure NAT in a commercial router?
Para configurar o router, foi necessário configurar a interface interna do processo de
NAT. Para isso, seguimos explicitamente todos os comandos presentes no slide 46 do guião
deste trabalho laboratorial a partir do GTKTerm.

## 4. What does NAT do?
O NAT (Network Address Translation) tem como principal objetivo conservar endereços
IP e permitir que redes privadas que usam endereços IP não registados se conectem a uma
rede pública.
O NAT opera num router, onde conecta duas redes e traduz os endereços privados na
rede interna para endereços legais, antes que os pacotes sejam encaminhados para outra
rede. Este oferece ainda funções de segurança e é implementado em ambientes de acesso
remoto.
Se um dos computadores tux da rede local pretender efetuar uma ligação com uma rede
pública (ex 172.16.1.254), o router poderá alterar o endereço de origem para um endereço
exterior (172.16.1.39). Desta forma, o endereço da máquina será ocultado. Após um
eventual pacote ter sido enviado para a rede (172.16.1.254) esta devolve um outro ao
endereço que conhece (172.16.1.39) e o router será encarregue de o enviar à máquina
correspondente. Desta forma, tornando o processo mais seguro e permitindo uma
comunicação entre uma rede privada e uma rede pública.
Resumindo, permite que máquinas de uma rede interna, como a que foi criada, tenham
acesso ao exterior, sendo que apenas 1 único endereço IP é exigido para representar um
grupo de computadores fora da sua própria rede.