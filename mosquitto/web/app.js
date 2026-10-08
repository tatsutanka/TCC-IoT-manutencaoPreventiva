// --- CONFIGURAÇÕES DO BROKER MOSQUITTO ---
const MQTT_CONFIG = {
  host: "192.168.1.135", // IP do seu notebook CachyOS
  port: 9001,            // Porta WebSocket
  username: "web_user",   // Usuário criado via mosquitto_passwd
  password: "talumu53",   // Senha definida no passwd
  useSSL: true,          // true para WSS (WebSocket Secure)
  topicSubscribe: "dht22-esp8266-001/loopback"  // Tópico que deseja escutar
};

// Gera um ID único de cliente para o navegador
const clientId = "WebClient-" + Math.random().toString(16).substring(2, 10);

// Cria o cliente Paho MQTT
const client = new Paho.MQTT.Client(
  MQTT_CONFIG.host,
  Number(MQTT_CONFIG.port),
  clientId
);

// --- CALLBACKS DE EVENTOS ---
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

// Inicia a conexão
conectarBroker();

function conectarBroker() {
  console.log("Tentando conectar ao Mosquitto via WebSocket...");
  
  const options = {
    userName: MQTT_CONFIG.username,
    password: MQTT_CONFIG.password,
    useSSL: MQTT_CONFIG.useSSL,
    timeout: 5,
    onSuccess: onConnectSuccess,
    onFailure: onConnectFailure
  };

  client.connect(options);
}

// Executado quando a conexão é bem-sucedida
function onConnectSuccess() {
  console.log("Conectado com sucesso ao Mosquitto!");
  
  const statusEl = document.getElementById("status");
  if (statusEl) {
    statusEl.textContent = "Conectado";
    statusEl.className = "status-connected";
  }

  // Inscreve-se no tópico configurado
  client.subscribe(MQTT_CONFIG.topicSubscribe);
  console.log(`Inscrito no tópico: ${MQTT_CONFIG.topicSubscribe}`);
}

// Executado caso ocorra falha na conexão
function onConnectFailure(response) {
  console.error("Falha ao conectar:", response.errorMessage);
  
  const statusEl = document.getElementById("status");
  if (statusEl) {
    statusEl.textContent = "Erro na Conexão";
    statusEl.className = "status-disconnected";
  }

  setTimeout(conectarBroker, 5000);
}

// Executado se a conexão for interrompida
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
    console.warn("Conexão perdida:", responseObject.errorMessage);
    
    const statusEl = document.getElementById("status");
    if (statusEl) {
      statusEl.textContent = "Reconectando...";
      statusEl.className = "status-disconnected";
    }

    setTimeout(conectarBroker, 3000);
  }
}

// Executado quando chega uma nova mensagem MQTT
function onMessageArrived(message) {
  console.log(`Mensagem recebida no tópico [${message.destinationName}]:`, message.payloadString);

  // Atualiza elementos estáticos topo caso existam
  const topicEl = document.getElementById("topic");
  const payloadEl = document.getElementById("payload");
  if (topicEl) topicEl.textContent = message.destinationName;
  if (payloadEl) payloadEl.textContent = message.payloadString;

  const container = document.getElementById("sensor-list");
  if (!container) return;

  // Remove a mensagem inicial de "Aguardando dados..." no primeiro recebimento
  const emptyMsg = container.querySelector(".empty-msg");
  if (emptyMsg) emptyMsg.remove();

  let dados;

  // 1. Tenta interpretar o conteúdo como JSON, se falhar trata como texto puro
  try {
    dados = JSON.parse(message.payloadString);
    if (typeof dados !== 'object' || dados === null) {
      dados = { "Valor": message.payloadString };
    }
  } catch (error) {
    dados = { "Mensagem": message.payloadString };
  }

  // 2. Cria um CARD ÚNICO para o envio completo recebido
  const cardEntry = document.createElement("div");
  cardEntry.className = "sensor-card";

  // Gera a lista interna de leituras contidas neste envio especificamente
  let camposHTML = "";
  Object.keys(dados).forEach(chave => {
    camposHTML += `
      <div class="sensor-field">
        <span class="sensor-title">${formatarChave(chave)}:</span>
        <span class="sensor-value">${dados[chave]}</span>
      </div>
    `;
  });

  // Adiciona o cabeçalho com hora de recepção e os dados do payload
  cardEntry.innerHTML = `
    <div class="card-header">
      <small>Recebido às ${obterHoraAtual()}</small>
    </div>
    <div class="card-body">
      ${camposHTML}
    </div>
  `;

  // 3. Adiciona o novo card no TOPO da lista (as mensagens mais recentes ficam em primeiro)
  container.prepend(cardEntry);
}



// Funções Auxiliares
function formatarChave(texto) {
  return texto.replace(/_/g, ' ').toUpperCase();
}

function obterHoraAtual() {
  const agora = new Date();
  return agora.toLocaleTimeString('pt-BR');
}
