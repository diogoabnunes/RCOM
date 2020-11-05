# Compilação

## Para compilar usando a porta de série:
* Ligar porta de série;
* Clonar o repositório do projeto nos dois computadores a usar;
* cd RCOM/TL1/src && make
* **./app recetor /dev/ttyS0 ../docs_recetor/** (terminal 1)
* **./app emissor /dev/ttyS0 pinguim.gif** (terminal 2)

## Para compilar usando demonstration.sh:

* Abrir um terminal > Preferências > Criar perfil > Nome: RCOM;
* Comando > Ao terminar comando: Manter o terminal aberto;
* Clonar o repositório do projeto;
* cd RCOM/TL1/
* ./demonstration.sh
* Colocar password de sudo no terminal que vai aparecer;
* Passados 10seg da instrução acima, 2 terminais abrirão (emissor e recetor).
