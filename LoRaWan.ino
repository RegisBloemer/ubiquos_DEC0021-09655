#include "LoRaWan_APP.h"
#include <Wire.h>
#include <MPU6050.h>  // Biblioteca para o sensor MPU-6050

// Instancio o acelerômetro (MPU-6050)
MPU6050 mpu;

// Variáveis para armazenar as leituras do acelerômetro
float accX, accY, accZ;

/* OTAA para LoRaWAN */
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x69, 0x1F };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x2F, 0xC3, 0x96, 0xD0, 0x59, 0x5F, 0xD7, 0xC4, 0xF3, 0xD0, 0x57, 0xAA, 0xC0, 0x00, 0x87, 0x7D };

/* ABP para LoRaWAN */
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/* Máscara de canais LoRaWan, padrão 0-7 */
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

/* Região LoRaWan definida nas ferramentas do Arduino IDE */
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/* Classe LoRaWan, pode ser A ou C */
DeviceClass_t loraWanClass = CLASS_C;

/* Intervalo de envio, em ms */
uint32_t appTxDutyCycle = 15000;

/* Defino se vou usar OTAA ou ABP */
bool overTheAirActivation = true;

/* ADR habilitado (Adaptive Data Rate) */
bool loraWanAdr = true;

/* Indica se envio mensagens confirmadas */
bool isTxConfirmed = true;

/* Porta da aplicação */
uint8_t appPort = 2;

/* Número de tentativas para confirmação */
uint8_t confirmedNbTrials = 4;

/*
  Aqui preparo o payload com as leituras do acelerômetro MPU-6050
*/
static void prepareTxFrame( uint8_t port ) {
    le_acelerometro(); // Chamo a função que faz a leitura dos eixos

    // Montagem da mensagem com os valores de aceleração em cada eixo
    char message[50];
    snprintf(message, sizeof(message), "AX:%.2f AY:%.2f AZ:%.2f", accX, accY, accZ);

    // Defino o tamanho do payload
    appDataSize = strlen(message);

    // Copio o payload byte a byte para o buffer de envio
    for (int i = 0; i < appDataSize; i++) {
        appData[i] = (uint8_t)message[i];
    }
}

/*
  Função que faz a leitura do acelerômetro MPU-6050
  Retorna os valores de aceleração nos eixos X, Y, Z em 'g'
*/
void le_acelerometro() {
    mpu.getAcceleration(&accX, &accY, &accZ); // Lê os valores do sensor (valor bruto)
    // Converto para 'g' dividindo por 16384, padrão do MPU-6050 (sensibilidade padrão)
    accX = accX / 16384.0;
    accY = accY / 16384.0;
    accZ = accZ / 16384.0;
}

/*
  Inicialização dos módulos: comunicação serial, LoRa e MPU-6050
*/
void setup() {
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE); // Inicializo a placa Heltec (ou equivalente)
    Wire.begin();   // Início do barramento I2C

    mpu.initialize(); // Inicializo o MPU-6050

    // Testo se o MPU-6050 respondeu corretamente
    if (mpu.testConnection()) {
        Serial.println("MPU6050 conectado com sucesso!");
    } else {
        Serial.println("Erro ao conectar ao MPU6050!");
    }
}

/*
  Máquina de estados para controle do LoRaWAN
*/
void loop() {
    switch( deviceState )
    {
        case DEVICE_STATE_INIT:
        {
    #if(LORAWAN_DEVEUI_AUTO)
          LoRaWAN.generateDeveuiByChipID();
    #endif
          LoRaWAN.init(loraWanClass,loraWanRegion);
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
          prepareTxFrame( appPort ); // Chamo a função que prepara o payload com dados do MPU
          LoRaWAN.send();            // Envio a mensagem pela rede LoRaWAN
          deviceState = DEVICE_STATE_CYCLE; // Prossigo para o próximo ciclo
          break;
        }
        case DEVICE_STATE_CYCLE:
        {
          // Defino o próximo ciclo de envio
          txDutyCycleTime = appTxDutyCycle + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
          LoRaWAN.cycle(txDutyCycleTime);
          deviceState = DEVICE_STATE_SLEEP;
          break;
        }
        case DEVICE_STATE_SLEEP:
        {
          // Coloco o dispositivo para dormir até o próximo ciclo
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
