# Obiquos - Sistema de Monitoramento de Sensores

## Visão Geral do Projeto

Obiquos é um sistema de monitoramento IoT que coleta dados de vários sensores (acelerômetros e sensores de vibração), publica-os via MQTT e os encaminha para o Zabbix para monitoramento e alertas. O sistema permite o monitoramento em tempo real das condições físicas do seu ambiente.

## Arquitetura

O sistema consiste em três componentes principais:

1. **Nós Sensores**: Microcontroladores ESP8266/ESP32 com sensores conectados
2. **Broker MQTT**: Recebe e distribui dados dos sensores
3. **Ponte MQTT-Zabbix**: Scripts Python que se inscrevem em tópicos MQTT e encaminham dados para o Zabbix

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  ESP8266/   │     │             │     │  Python     │     │             │
│   ESP32     ├────►│ MQTT Broker ├────►│  Bridge     ├────►│   Zabbix    │
│ com Sensores│     │             │     │             │     │             │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

## Sensores Suportados

- **MPU6050**: Acelerômetro e giroscópio de 3 eixos
- **SW-420**: Sensor de vibração (leituras digitais e analógicas)

## Requisitos de Hardware

- Placas de desenvolvimento ESP8266 ou ESP32
- Módulos de sensor MPU6050
- Módulos de sensor de vibração SW-420
- Fontes de alimentação para as placas ESP
- Fios de conexão
- Computador executando broker MQTT e servidor Zabbix

## Requisitos de Software

- Arduino IDE (para programar as placas ESP)
- Bibliotecas para ESP:
  - `Wire.h`
  - `ESP8266WiFi.h` ou `WiFi.h`
  - `PubSubClient.h`
  - `Adafruit_MPU6050.h` e `Adafruit_Sensor.h` (ou `MPU6050.h`)
- Broker MQTT (ex.: Mosquitto)
- Python 3.x
- Bibliotecas Python:
  - `paho-mqtt`
  - `json`
  - `subprocess`
- Servidor Zabbix com hosts e itens configurados adequadamente

## Instalação

### 1. Configurando os Nós Sensores

1. Conecte os sensores à sua placa ESP:
   - Para MPU6050: Conecte os pinos SDA, SCL, VCC e GND
   - Para SW-420: Conecte os pinos DO (saída digital), AO (saída analógica), VCC e GND

2. Instale as bibliotecas Arduino necessárias através do Gerenciador de Bibliotecas do Arduino IDE:
   - PubSubClient
   - Adafruit MPU6050
   - Adafruit Unified Sensor

3. Carregue o sketch apropriado para sua placa ESP:
   - `mqtt.ino` para o sensor MPU6050
   - `mqtt_sw_420.ino` para o sensor de vibração SW-420

### 2. Configurando o Broker MQTT

1. Instale o broker MQTT Mosquitto no seu servidor:
   ```bash
   sudo apt update
   sudo apt install mosquitto mosquitto-clients
   sudo systemctl enable mosquitto
   ```

### 3. Configurando a Ponte MQTT-Zabbix

1. Instale o Python 3 e os pacotes necessários:
   ```bash
   sudo apt install python3 python3-pip zabbix-sender
   pip3 install paho-mqtt
   ```

2. Copie os scripts da ponte para o seu servidor:
   - mqtt2zabbix.py para dados do acelerômetro
   - mqtt3zabbix.py para dados de vibração

3. Torne os scripts executáveis:
   ```bash
   chmod +x mqtt2zabbix.py mqtt3zabbix.py
   ```

### 4. Configurando o Zabbix

1. Configure um host no Zabbix chamado "SensorHost"
2. Crie itens trapper do Zabbix com as chaves:
   - `sensor.acelerometro` para dados do acelerômetro
   - `sensor.vibration` para dados de vibração
3. Configure gráficos e gatilhos de acordo com suas necessidades de monitoramento

## Configuração

### Configuração do ESP8266/ESP32

Edite os seguintes parâmetros nos sketches do Arduino:

```cpp
// Credenciais WiFi
const char* ssid = "SEU_SSID_WIFI";
const char* password = "SUA_SENHA_WIFI";

// Configurações do broker MQTT
const char* mqtt_server = "IP_DO_SEU_BROKER_MQTT";
const int mqtt_port = 1883;
```

### Configuração da Ponte MQTT-Zabbix

Edite os seguintes parâmetros nos scripts Python:

```python
# Configurações MQTT
MQTT_BROKER = "IP_DO_SEU_BROKER_MQTT"
MQTT_PORT = 1883
MQTT_TOPIC = "sensor/acelerometro" ou "sensor/vibracao"

# Configurações Zabbix
ZABBIX_SERVER = "SEU_SERVIDOR_ZABBIX"
ZABBIX_PORT = "10051"
HOSTNAME = "SensorHost"
```

## Utilização

### Iniciando o Sistema

1. Certifique-se de que suas placas ESP estão ligadas e conectadas ao WiFi
2. Inicie os scripts da ponte MQTT-Zabbix:
   ```bash
   python3 mqtt2zabbix.py &
   python3 mqtt3zabbix.py &
   ```

3. Monitore os dados dos sensores no Zabbix através dos hosts e itens configurados

### Formato dos Dados

#### Dados do Acelerômetro (MPU6050)
```json
{
  "x": 0.12,
  "y": -0.03,
  "z": 1.02,
  "gyro_x": 0.01,
  "gyro_y": 0.02,
  "gyro_z": -0.01,
  "temp": 24.5
}
```

#### Dados de Vibração (SW-420)
```json
{
  "vib": 1,
  "nivel": 845
}
```

## Solução de Problemas

### Problemas com ESP8266/ESP32

- **Falha na Conexão WiFi**: Verifique o SSID e a senha
- **Falha na Conexão MQTT**: Verifique o IP e a porta do broker MQTT
- **Sensor Não Detectado**: Verifique a fiação e os endereços I2C

### Problemas com MQTT

- **Nenhum Dado Publicado**: Use `mosquitto_sub` para verificar se os dados estão sendo publicados:
  ```bash
  mosquitto_sub -t "sensor/acelerometro" -v
  mosquitto_sub -t "sensor/vibracao" -v
  ```

### Problemas com Zabbix

- **Sem Dados no Zabbix**: Verifique se o `zabbix_sender` está funcionando corretamente:
  ```bash
  zabbix_sender -z SEU_SERVIDOR_ZABBIX -p 10051 -s "SensorHost" -k sensor.acelerometro -o '{"x":0,"y":0,"z":0}'
  ```

## Licença

Este projeto é fornecido como está, sem quaisquer garantias expressas ou implícitas.

---

*Última atualização: 5 de maio de 2025*

Similar code found with 2 license types