/***************************************************
  Ler SW‑420 (vibração) e enviar via MQTT em JSON
  — Ramom Guth – abr/2025
 ***************************************************/
 #include <ESP8266WiFi.h>     // substitua por <WiFi.h> se usar ESP32
 #include <PubSubClient.h>
 
 // ---------- PINAGEM ----------
 const uint8_t VIB_PIN_DIG = D6;   // saída digital (DO) do módulo
 // se não usar leitura analógica, deixe ANALOG_PIN em -1
 const int8_t  VIB_PIN_ANA = A0;   // saída analógica (AO) do módulo
 
 // ---------- Wi‑Fi ----------
 const char* ssid     = "A535G";
 const char* password = "mlki6803";
 
 // ---------- MQTT ----------
 const char* mqtt_server = "192.168.66.104";
 const int   mqtt_port   = 1883;
 const char* mqtt_topic  = "sensor/vibracao";
 
 // ---------- OBJETOS ----------
 WiFiClient      espClient;
 PubSubClient    client(espClient);
 
 // ---------- PROTÓTIPOS ----------
 void setup_wifi();
 void reconnect();
 
 void setup() {
   Serial.begin(115200);
   pinMode(VIB_PIN_DIG, INPUT);          // digital: HIGH quando vibra
   if (VIB_PIN_ANA >= 0) analogRead(A0); // garante inicialização
 
   setup_wifi();
   client.setServer(mqtt_server, mqtt_port);
 
   Serial.println("SW‑420 pronto!");
 }
 
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // ---------- LEITURA ----------
  int digVal = digitalRead(VIB_PIN_DIG);         // 0 ou 1
  int anaVal = (VIB_PIN_ANA >= 0) ? analogRead(VIB_PIN_ANA) : -1; // 0‑1023
  
  // Escala o valor analógico (0-1023) para percentual (0-100)
  int valorEscalado = (anaVal >= 0) ? map(anaVal, 0, 1023, 0, 100) : -1;

  // ---------- JSON ----------
  String payload = "{";
  payload += "\"vib\":"      + String(digVal);          // obrigatório
  if (anaVal >= 0) {                                     // opcional
    payload += ",\"nivel\":" + String(valorEscalado);   // Valor já escalado (0-100%)
  }
  payload += "}";

  // ---------- ENVIO ----------
  Serial.print("Publicando em ");
  Serial.print(mqtt_topic);
  Serial.print(": ");
  Serial.println(payload);
  client.publish(mqtt_topic, payload.c_str());

  delay(500);  // ~2 Hz
}
 
 //--------------------------------------------------
 void setup_wifi() {
   Serial.print("Conectando ao Wi‑Fi: "); Serial.println(ssid);
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
     delay(500); Serial.print('.');
   }
   Serial.println("\nWi‑Fi conectado – IP: " + WiFi.localIP().toString());
 }
 
 //--------------------------------------------------
 void reconnect() {
   while (!client.connected()) {
     Serial.print("Conectando ao MQTT… ");
     if (client.connect("SW420Client")) {
       Serial.println("conectado!");
     } else {
       Serial.print("falhou (rc="); Serial.print(client.state());
       Serial.println("), tentando em 5 s");
       delay(5000);
     }
   }
 }
 