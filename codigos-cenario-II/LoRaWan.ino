#include "LoRaWan_APP.h"

// Declaração da UART1 para receber dados do Arduino
HardwareSerial mySerial(1); // UART1

// OTAA IDs, onde devEUI e appKey foram gerados utilizando a plataforma TTN
uint8_t devEui[] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x07, 0x1C, 0x0F};
uint8_t appEui[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t appKey[] = {0xF2, 0x88, 0x02, 0xD1, 0x22, 0x94, 0x9A, 0xD7, 0x37, 0xE4, 0x05, 0xAA, 0x66, 0x79, 0xE3, 0xEB};

// ABP (não utilizado)
uint8_t nwkSKey[] = {0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85};
uint8_t appSKey[] = {0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67};
uint32_t devAddr = (uint32_t)0x007e6ae1;

uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t loraWanClass = CLASS_C;

uint32_t appTxDutyCycle = 15000;  // intervalo entre envios (ms)
bool overTheAirActivation = true; // OTAA
bool loraWanAdr = true;
bool isTxConfirmed = true;
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 4;

// Buffer para dados a enviar
#define MAX_PAYLOAD_SIZE 48

static void prepareTxFrame(uint8_t port)
{
  static char message[MAX_PAYLOAD_SIZE]; // buffer para mensagem
  static String serialData = "";

  while (mySerial.available())
  {
    char c = mySerial.read();
    if (c == '\n')
      break;
    serialData += c;
  }

  if (serialData.length() > 0)
  {
    serialData.trim();
    snprintf(message, sizeof(message), "%s", serialData.c_str());

    appDataSize = strlen(message);
    for (int i = 0; i < appDataSize; i++)
    {
      appData[i] = (uint8_t)message[i];
    }

    serialData = "";
  }
  else
  {
    // fallback, caso não tenha dado
    const char *fallback = "0.00 0.00 0.00";
    appDataSize = strlen(fallback);
    for (int i = 0; i < appDataSize; i++)
    {
      appData[i] = (uint8_t)fallback[i];
    }
  }
}

void setup()
{
  Serial.begin(115200);                       // debug serial USB
  mySerial.begin(115200, SERIAL_8N1, 43, -1); // UART1 RX no GPIO43, sem TX conectado
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
}

void loop()
{
  switch (deviceState)
  {
  case DEVICE_STATE_INIT:
  {
#if (LORAWAN_DEVEUI_AUTO)
    LoRaWAN.generateDeveuiByChipID();
#endif
    LoRaWAN.init(loraWanClass, loraWanRegion);
    LoRaWAN.setDefaultDR(3);
    break;
  }
  case DEVICE_STATE_JOIN:
  {
    LoRaWAN.join();
    break;
  }
  case DEVICE_STATE_SEND:
  {
    prepareTxFrame(appPort);
    LoRaWAN.send();
    deviceState = DEVICE_STATE_CYCLE;
    break;
  }
  case DEVICE_STATE_CYCLE:
  {
    txDutyCycleTime = appTxDutyCycle + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
    LoRaWAN.cycle(txDutyCycleTime);
    deviceState = DEVICE_STATE_SLEEP;
    break;
  }
  case DEVICE_STATE_SLEEP:
  {
    LoRaWAN.sleep(loraWanClass);
    break;
  }
  default:
  {
    deviceState = DEVICE_STATE_INIT;
    break;
  }
  }
}