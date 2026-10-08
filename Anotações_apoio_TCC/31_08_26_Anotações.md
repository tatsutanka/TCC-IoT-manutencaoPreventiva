# TCC 
## Implementação MQTT ESP8266

### Oque é MQTT
É um protocolo de rede do tipo pub/sub que utiliza o TCP/IP, ou seja ele precisa de uma estrutura de 3 pontos 

- Broker: É o cérebro do sistema (ex: Eclipse Mosquitto, EMQX, HiveMQ). Ele não cria dados, apenas recebe todas as mensagens, filtra por interesse e as distribui para os dispositivos corretos.
  
- Pub: Qualquer cliente (como o ESP8266) que envia uma informação para o Broker associada a um "rótulo" chamado Tópico.
- Sub: Qualquer cliente (como um app, banco de dados ou outro ESP) que avisa ao Broker que deseja receber todas as mensagens publicadas em um Tópico específico.


Durante a configuração do mosquitto como o broker eu precisei abrir a porta 1883 no firewall do notebook 

No geral todos os passos funcionaram muito bem 

inicialmmente se coloca mosquitto -c config.conf -v

para dar inicio ao servidor mosquitto que representa o broker 

Utilizando mosquitto_sub para se subscrever no topico 
mosquitto_sub -h 192.168.1.135 -p 1883 -t "YOUR-NAME-esp8266-001/loopback" -v  

No arduino ele fica servindo atravez do mqtt 

