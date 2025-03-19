#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configurações de rede Wi‑Fi
const char* ssid = "A535G";       // Nome da sua rede Wi‑Fi
const char* password = "mlki6803";  // Senha da rede

// Configuração do broker MQTT
const char* mqtt_server = "192.168.254.104";  // IP do broker MQTT (pode ser o Mosquitto ou o Node‑RED se configurado como broker)

WiFiClient espClient;
PubSubClient client(espClient);

// Função para conectar ao Wi‑Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// Função para reconectar ao MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Conectado");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" - Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Simula a leitura de um acelerômetro (valores aleatórios para x, y, z)
  float x = random(-100, 100) / 10.0;
  float y = random(-100, 100) / 10.0;
  float z = random(-100, 100) / 10.0;

  // Cria uma mensagem JSON com as chaves "x", "y" e "z"
  String payload = "{\"x\": " + String(x, 1) + ", \"y\": " + String(y, 1) + ", \"z\": " + String(z, 1) + "}";
  Serial.println("Enviando: " + payload);

  // Publica a mensagem no tópico "sensor/acelerometro"
  client.publish("sensor/acelerometro", payload.c_str());
  delay(5000);  // Aguarda 5 segundos para enviar novamente
}
