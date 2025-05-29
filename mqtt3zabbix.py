#!/usr/bin/env python3
"""
Escuta o tópico MQTT 'sensor/vibracao' e envia APENAS o nível analógico
(nivel) para dois servidores Zabbix via zabbix_sender.
"""
import json
import subprocess
import paho.mqtt.client as mqtt

# ---------- CONFIGURAÇÕES MQTT ----------
MQTT_BROKER = "localhost"
MQTT_PORT   = 1883
MQTT_TOPIC  = "sensor/vibracao"

# ---------- CONFIGURAÇÕES ZABBIX ----------
HOSTNAME   = "SensorHost"
KEY_NIVEL  = "sensor.vibration"

# Lista de alvos Zabbix
ZABBIX_TARGETS = [
    {"server": "regis-nitro-an515-51", "port": "10051"},
    {"server": "192.168.66.210",         "port": "10051"},
]

# ---------- CALLBACKS MQTT ----------
def on_connect(client, userdata, flags, rc):
    print(f"Conectado ao MQTT (rc={rc})")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        if "nivel" in data:
            send_to_zabbix(KEY_NIVEL, data["nivel"])
    except Exception as e:
        print("Erro ao processar a mensagem:", e)

# ---------- FUNÇÃO AUXILIAR ZABBIX ----------
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

# ---------- MAIN ----------
if __name__ == "__main__":
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_forever()
