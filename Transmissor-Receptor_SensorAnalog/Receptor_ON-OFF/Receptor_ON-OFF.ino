#include <SPI.h>
#include <LoRa.h>

// Definir os pinos utilizados pelo RFM95
const uint8_t sck = 13;   // Pino do clock
const uint8_t miso = 12;  // Pino master in, slave out
const uint8_t mosi = 11;  // Pino master out, slave in
const uint8_t cs = 10;    // Pino chip select
const uint8_t reset = 9;  // Pino chip select
const uint8_t irq = 8;    // Pino chip select

uint8_t localAddress = 0xB0;

void setup() {
  // Inicializar o barramento SPI
  Serial.begin(9600);
  while (!Serial)
    ;

  SPI.begin();
  SPI.beginTransaction(SPISettings(8E6, MSBFIRST, SPI_MODE0));

  // Configurar o pino NSS (chip select) para o RFM95
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);  // Desativar o chip select inicialmente

  // Inicializar o módulo LoRa
  LoRa.setPins(cs, reset, irq);  // Substitua 'reset' e 'irq' pelos pinos correspondentes, se necessário

  Serial.println(F("Comunicação LoRa Duplex - Ping&Pong"));
  if (!LoRa.begin(915E6)) {
    Serial.println(F("Erro ao iniciar módulo LoRa. Verifique a conexão dos seus pinos!!"));
    while (true)
      ;
  }

  Serial.println(F("Módulo LoRa iniciado com sucesso!!! 🙂"));

  pinMode(4, OUTPUT);
}

void loop() { 
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    onReceive(packetSize);
  }
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;

  uint8_t recipient = LoRa.read();
  byte sender = LoRa.read();
  byte incomingLength = LoRa.read();

  char incoming[32] = "";  // Array de caracteres para armazenar a mensagem

  LoRa.readBytes(incoming, incomingLength);  // Lê os caracteres do LoRa e armazena no array
  incoming[incomingLength] = '\0';           // Adiciona o caractere nulo ao final do array

  if (incomingLength != strlen(incoming)) {
    Serial.println(F("erro!: o tamanho da mensagem nao condiz com o conteudo!"));
    return;
  }

  if (recipient != localAddress && recipient != 0x01) {
    Serial.println(F("This message is not for me."));
    return;
  }

  Serial.print(F("Recebido do dispositivo: "));
  Serial.println(sender, HEX);
  Serial.print(F("Enviado para: "));
  Serial.println(recipient, HEX);
  Serial.print(F("Tamanho da mensagem: "));
  Serial.println(incomingLength);
  Serial.print(F("Mensagem: "));
  Serial.write(incoming);  // Imprime os caracteres do array
  Serial.println();
  Serial.print(F("RSSI: "));
  Serial.println(LoRa.packetRssi());  // Obtém o RSSI da mensagem
  Serial.print(F("SNR: "));
  Serial.println(LoRa.packetSnr());  // Obtém o RSSI da mensagem
  Serial.println();

  if (strcmp(incoming, "ligar") == 0) {
    digitalWrite(4, HIGH);
    Serial.println(F("Comando para ligar a porta 4 recebido!"));
  } else if (strcmp(incoming, "desligar") == 0) {
    digitalWrite(4, LOW);
    Serial.println(F("Comando para desligar a porta 4 recebido!"));
  }
}
