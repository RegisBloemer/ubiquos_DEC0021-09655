#!/usr/bin/env python3
"""
Escuta o tópico MQTT 'sensor/vibracao' e envia APENAS o nível analógico
(nivel) para o Zabbix via zabbix_sender.
"""
import json
import subprocess
import paho.mqtt.client as mqtt

# ---------- CONFIGURAÇÕES ----------
MQTT_BROKER   = "localhost"
MQTT_PORT     = 1883
MQTT_TOPIC    = "sensor/vibracao"

ZABBIX_SERVER = "regis-nitro-an515-51"
ZABBIX_PORT   = "10051"
HOSTNAME      = "SensorHost"

# KEY_NIVEL continua ativa; KEY_VIB mantida só para referência
KEY_VIB   = "sensor.vibration"
KEY_NIVEL = "sensor.vibration.nivel"

# ---------- CALLBACKS ----------
def on_connect(client, userdata, flags, rc):
    print(f"Conectado ao MQTT (rc={rc})")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())

        # ------- ENVIO DESATIVADO (valor digital) -------
        # if "vib" in data:
        #     send_to_zabbix(KEY_VIB, data["vib"])

        # ------- ENVIO ATIVO (valor analógico) -------
        if "nivel" in data:
            send_to_zabbix(KEY_NIVEL, data["nivel"])

    except Exception as e:
        print("Erro ao processar a mensagem:", e)

# ---------- FUNÇÃO AUXILIAR ----------
def send_to_zabbix(key, value):
    cmd = [
        "zabbix_sender",
        "-z", ZABBIX_SERVER,
        "-p", ZABBIX_PORT,
        "-s", HOSTNAME,
        "-k", key,
        "-o", str(value),
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("Falha no zabbix_sender:", result.stderr.strip())

# ---------- MAIN ----------
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_forever()
