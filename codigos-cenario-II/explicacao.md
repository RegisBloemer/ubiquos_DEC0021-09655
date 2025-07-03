## 🎯 Objetivo

Coletar dados de vibração do sensor MPU-6050 conectado a um Arduino UNO, transmitir esses dados via LoRa pelo módulo Heltec ESP32 WiFi LoRa para um LoRaWAN Gateway, que encaminha os dados para a plataforma **The Things Stack Sandbox** (The Things Network). Esta plataforma, por sua vez, encaminha os dados para o **ThingSpeak**, que é utilizado para visualização e análise dos dados em dashboards.

---

## 📦 Componentes e Bibliotecas Utilizadas

- **Arduino UNO** com sensor **MPU-6050** (acelerômetro e giroscópio 3 eixos)
- **Heltec ESP32 WiFi LoRa 32 (V3)** (módulo transmissor via LoRa)
- **Dragino LPS8N** (LoRaWAN Gateway)
- Biblioteca **Wire.h** para comunicação I2C com MPU-6050
- Biblioteca e arquivos relacionados à pilha **LoRaWan_APP** para ESP32 Heltec, provenientes da biblioteca Heltec ESP32 Dev-Boards  
- Comunicação serial UART entre Arduino UNO e Heltec ESP32 (velocidade 115200 bps)

---

## ⚙️ Configurações e Comunicação entre módulos

| Módulo                  | Função                                     | Comunicação                            |
|-------------------------|--------------------------------------------|--------------------------------------|
| Arduino UNO             | Coleta os dados brutos do MPU-6050         | I2C (Wire) para sensor               |
| Arduino UNO - Heltec   | Envia dados via Serial UART (pinos definidos) | UART1 RX no GPIO43 do Heltec         |
| Heltec ESP32 WiFi LoRa  | Recebe dados via UART, empacota e transmite via LoRa | LoRa para LoRaWAN Gateway            |
| LoRaWAN Gateway (Dragino) | Recebe pacote LoRa e encaminha para The Things Stack Sandbox | The Things Stack realiza o processamento |
| The Things Stack Sandbox| Plataforma LoRaWAN que gerencia dispositivos, segurança e roteamento | Encaminha dados para ThingSpeak via integração MQTT/Webhook |
| ThingSpeak              | Plataforma de visualização e análise dos dados | Dashboards e gráficos em tempo real |

---

## Fluxo Geral do Sistema Atualizado

1. **Sensor + Arduino UNO**: coleta e processa dados do MPU-6050, enviando valores via serial para o Heltec ESP32.
2. **Heltec ESP32 WiFi LoRa**: recebe dados da serial, conecta-se à rede LoRaWAN, empacota os dados e transmite para o LoRaWAN Gateway Dragino LPS8N.
3. **LoRaWAN Gateway Dragino LPS8N**: recebe pacotes LoRa e encaminha para a plataforma **The Things Stack Sandbox**.
4. **The Things Stack Sandbox**: gerencia a rede LoRaWAN, faz a autenticação e encaminha os dados para a plataforma **ThingSpeak** usando integração (MQTT, HTTP webhook, etc).
5. **ThingSpeak**: recebe os dados, armazena e exibe dashboards e gráficos em tempo real para análise.
6. **(Opcional)** Algoritmos de IA podem ser aplicados sobre os dados para detectar anomalias e gerar alertas de manutenção preditiva.

---

## Observações Importantes

- A plataforma **The Things Stack Sandbox** funciona como um backend completo para dispositivos LoRaWAN, com suporte a OTAA, ADR e roteamento.
- O **ThingSpeak** é especialmente útil para visualizar séries temporais e pode ser integrado com MATLAB para análises avançadas.
- A comunicação do Heltec ESP32 com o gateway e a The Things Stack segue o protocolo LoRaWAN, garantindo segurança e confiabilidade na transmissão.
- É importante configurar corretamente os aplicativos na The Things Stack para receber e encaminhar os dados para o ThingSpeak, geralmente via webhook (usado neste cenário) ou MQTT.
