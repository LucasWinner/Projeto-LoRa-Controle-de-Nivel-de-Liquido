#include <SPI.h>
#include <LoRa.h>

// Definir os pinos utilizados pelo RFM95
const uint8_t sck = 13;   // Pino do clock
const uint8_t miso = 12;  // Pino master in, slave out
const uint8_t mosi = 11;  // Pino master out, slave in
const uint8_t cs = 10;    // Pino chip select
const uint8_t reset = 9;  // Pino chip select
const uint8_t irq = 8;    // Pino chip select

char outgoing[32] = "";  // outgoing message

uint8_t localAddress = 0xA0;  // Endereco deste dispositivo LoRa
uint8_t destination = 0xB0;   // Endereco do dispositivo para enviar a mensagem (0xFF envia para todos devices)
long lastSendTime = 0;        // TimeStamp da ultima mensagem enviada
int interval = 5000;          // Interavalo em ms no envio das mensagens (inicial 5s)

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
}

void loop() {
  // Lê as tensões dos sensores nas portas analógicas A0 e A1
  float tensao0 = analogRead(A0) * 5.0 / 1023.0;
  float tensao1 = analogRead(A1) * 5.0 / 1023.0;
  Serial.print("Tensao do pino A0 em volts: ");
  Serial.print(tensao0);
  Serial.print("\n");
  Serial.print("Tensao do pino A1 em volts: ");
  Serial.print(tensao1);
  Serial.print("\n");
  // Verifica se as tensões são menores que 0.15
  if (tensao0 < 0.15 && tensao1 < 0.15) {
    LoRa.idle();                // Reativa o chip LoRa após o sleep
    strcpy(outgoing, "ligar");  // Copia o comando "ligar" para o array de caracteres
    sendMessage(outgoing);      // Envia o comando "ligar"
    Serial.println(F("Enviando comando para ligar"));
  } else if (tensao1 > 0.15) {
    LoRa.idle();                   // Reativa o chip LoRa após o sleep
    strcpy(outgoing, "desligar");  // Copia o comando "desligar" para o array de caracteres
    sendMessage(outgoing);         // Envia o comando "desligar"
    Serial.println(F("Enviando comando para desligar"));
  }

  lastSendTime = millis();  // Timestamp da última mensagem

  delay(interval);
  LoRa.sleep();  // Coloca o chip LoRa em modo de baixo consumo
}

// Funcao que envia uma mensagem LoRa
void sendMessage(char* outgoing) {
  LoRa.beginPacket();            // Inicia o pacote da mensagem
  LoRa.write(destination);       // Adiciona o endereco de destino
  LoRa.write(localAddress);      // Adiciona o endereco do remetente
  LoRa.write(strlen(outgoing));  // Tamanho da mensagem em bytes
  LoRa.print(outgoing);          // Vetor da mensagem
  LoRa.endPacket();              // Finaliza o pacote e envia
}
