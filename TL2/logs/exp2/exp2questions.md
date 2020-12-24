# Experience 2 questions

## 1. How to configure vlany0?
Para configurar a vlan y0, através do terminal GTKTerm, invocamos os seguintes
comandos:
● para criar a VLAN:
● ○ configure terminal
○ vlan 40
○ end
para adicionar as portas do tuxy3 e tuxy4 à VLAN (repetimos esta sequência de
comandos, uma para cada tux):
○ configure terminal
○ interface fastethernet 0/[porta onde está ligado o tuxy respetivo]
○ switchport mode access
○ switchport access vlan 40
○ end

## 2. How many broadcast domains are there? How can you conclude it from the logs?
Quando se faz ping broadcast a partir do tuxy3, este recebe resposta do tuxy4, mas não
do tuxy2. Quando se faz o mesmo comando a partir do tuxy2, este não recebe nenhuma
resposta.
Concluímos assim que existem 2 domínios de transmissão (broadcast): o domínio que
contém o tuxy3 e tuxy4 (vlany0), e o que contém o tuxy2 (vlany1).