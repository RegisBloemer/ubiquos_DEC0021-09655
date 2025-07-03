## 🎯 Objetivo

Escutar mensagens MQTT no tópico `sensor/vibracao` e enviar **apenas** o valor do campo `nivel` para dois servidores Zabbix, usando o utilitário `zabbix_sender`.

---

## 📦 Dependências

* **Python 3**
* Biblioteca **paho-mqtt**

  ```bash
  pip install paho-mqtt
  ```
* Utilitário **zabbix\_sender** (do pacote Zabbix Agent)
* Comando `json` (builtin do Python)
* Módulo `subprocess` (builtin do Python)

---

## ⚙️ Configurações

```python
# Broker MQTT
MQTT_BROKER = "localhost"
MQTT_PORT   = 1883
MQTT_TOPIC  = "sensor/vibracao"

# Identificação no Zabbix
HOSTNAME   = "SensorHost"
KEY_NIVEL  = "sensor.vibration"

# Destinos Zabbix (host e porta)
ZABBIX_TARGETS = [
    {"server": "regis-nitro-an515-51", "port": "10051"},
    {"server": "192.168.67.210",  "port": "10051"},  # este host é um Raspberry Pi 3
]
```

* **Obs.**: Um dos servidores Zabbix está rodando em um **Raspberry Pi 3**.
  Para instalar o Zabbix Agent neste dispositivo, você pode seguir este tutorial:
  [https://www.youtube.com/watch?v=8JZXbsIQ\_YU\&ab\_channel=SimplificandoRedes](https://www.youtube.com/watch?v=8JZXbsIQ_YU&ab_channel=SimplificandoRedes).

---

## 🔌 Callbacks MQTT

### `on_connect`

```python
def on_connect(client, userdata, flags, rc):
    print(f"Conectado ao MQTT (rc={rc})")
    client.subscribe(MQTT_TOPIC)
```

* Disparado quando conectar no broker.
* Exibe código de retorno (`rc`) e faz subscribe do tópico.

### `on_message`

```python
def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        if "nivel" in data:
            send_to_zabbix(KEY_NIVEL, data["nivel"])
    except Exception as e:
        print("Erro ao processar a mensagem:", e)
```

* Recebe cada mensagem (`msg.payload`) como **JSON**.
* Se existir o campo `"nivel"`, chama `send_to_zabbix` passando a **chave** e o **valor**.

---

## 🚀 Envio ao Zabbix

```python
def send_to_zabbix(key, value):
    payload = json.dumps({"nivel": value})
    print(f"[ZBX-SEND] key={key} value={payload}")

    for target in ZABBIX_TARGETS:
        cmd = [
            "zabbix_sender",
            "-z", target["server"],
            "-p", target["port"],
            "-s", HOSTNAME,
            "-k", key,
            "-o", payload,
        ]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Falha em {target['server']}:{target['port']} -> {result.stderr.strip()}")
        else:
            print(f"Enviado com sucesso para {target['server']}:{target['port']}")
```

1. Constrói um **JSON** com o campo `nivel`.
2. Para **cada** servidor em `ZABBIX_TARGETS`, executa o comando `zabbix_sender`.
3. Verifica se houve erro e imprime falha ou sucesso.

---

## 🏁 Fluxo Principal (Main)

```python
if __name__ == "__main__":
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_forever()
```

1. Cria instância `mqtt.Client()`.
2. Associa os callbacks `on_connect` e `on_message`.
3. Conecta ao broker MQTT especificado.
4. Inicia o loop infinito de escuta de mensagens com `loop_forever()`.

---

## 📌 Resumo

* **Escuta**: subscreve `sensor/vibracao` via MQTT.
* **Filtra**: processa apenas o campo `nivel` do JSON recebido.
* **Envia**: dispara `zabbix_sender` para múltiplos servidores (incluindo um Raspberry Pi 3), reportando o valor analógico.
* **Instalação no Pi 3**: use o tutorial do Simplificando Redes para configurar o Zabbix Agent no Raspberry Pi 3.
