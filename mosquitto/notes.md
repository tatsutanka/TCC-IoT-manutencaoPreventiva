# Passo a passo 

- Preciso deinir as senhas com mosquitto_pswd 
  - são necessarios duas senhas uma pra 1883 que é responsavel pelo sensor e uma outra para o websocket

Pra gerar o passwd 
```
sudo mosquitto_passwd -c /endereço/do/arquivo esp_user
sudo mosquitto_passwd /endereço/do/arquivo web_user
```

gera os 3 arquivos necessarios para rodar o websocket com o cerificado ssl 

```
cd "/home/tanaka/Área de trabalho/TCC/mosquitto"

openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout server.key \
  -out server.crt \
  -subj "/CN=192.168.1.135"
```


- precisso fazer o ufw do firewaal do computador 
- o init.sh
  - Começa o servidor mosquitto 
- pagina na web 


Eu tenho que ter 3 arquivos 

- passwd
- server.crt
- sever.key

Um adendo de se lembrar que o para occorrer a aceitação do certificado é preciso se conectar ao ip aceitar a conexão 
Falha ao conectar: AMQJS0007E Socket error:undefined.
