# Experience 6 questions

## 1. How many TCP connections are opened by your ftp application?
A aplicação FTP abre 2 conexões TCP:
● uma para mandar os comandos FTP ao servidor e receber as respectivas respostas.
● outra para fazer a transferência do ficheiro pretendido.

## 2. In what connection is transported the FTP control information?
O controlo de informação é transportado na primeira conexão criada, ou seja, na
conexão responsável pela troca de comandos FTP para preparação da transferência do
ficheiro.

## 3. What are the phases of a TCP connection?
Uma conexão TCP tem 3 fases:
● estabelecimento da conexão.
● troca de mensagens e de informação.
● finalização da conexão.

## 4. How does the ARQ TCP mechanism work? What are the relevant TCP fields? What relevant information can be observed in the logs?
O TCP (Transmission Control Protocol) utiliza o mecanismo ARQ (Automatic Repeat
Request) com o método da janela deslizante, que consiste no controlo de erros na
transmissão de dados. Para isso, utiliza:
●
●
●
acknowledgment numbers ​ que indicam que a trama foi recebida corretamente.
window size ​ que indica a gama de pacotes recebidos.
sequence number ​ , que indica o número do pacote a ser enviado.

## 5. How does the TCP congestion control mechanism work? What are the relevant fields. How did the throughput of the data connection evolve along the time? Is it according the TCP congestion control mechanism?
?

## 6. Is the throughput of a TCP data connections disturbed by the appearance of a second TCP connection? How?
Com o aparecimento de uma segunda conexão TCP, o fluxo de conexões de dados
TCP é afetado, uma vez que a taxa de transferência é distribuída de igual forma para cada
ligação. Assim, a existência de uma transferência de dados pode sofrer uma queda na taxa
de transmissão, uma vez que esta vai ser equilibrada com a nova conexão que acabou de
se estabelecer.