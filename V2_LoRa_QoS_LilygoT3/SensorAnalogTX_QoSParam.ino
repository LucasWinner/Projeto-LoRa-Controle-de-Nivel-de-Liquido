#include <SPI.h>
#include <LoRa.h>

// Pinos utilizados pelo RFM95
const uint8_t SCK_PIN = 5;
const uint8_t MISO_PIN = 19;
const uint8_t MOSI_PIN = 27;
const uint8_t CS_PIN = 18;
const uint8_t RESET_PIN = 15;
const uint8_t IRQ_PIN = 26;

// Pinos dos sensores
#define SENSOR0_PIN 34
#define SENSOR1_PIN 35

// Variáveis para monitoramento de pacotes
int totalSentPackets = 0;
int totalReceivedPackets = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Inicializando LoRa...");

  // Defina os pinos do módulo LoRa
  LoRa.setPins(CS_PIN, RESET_PIN, IRQ_PIN);

  // Inicialize o SPI manualmente para definir os pinos personalizados
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  // Tente inicializar o rádio LoRa
  if (!LoRa.begin(915E6)) {
    Serial.println("Falha ao inicializar LoRa! Verifique as conexões e a frequência.");
    while (1);
  }

  Serial.println("LoRa inicializado com sucesso!");
}

void loop() {
  // Leia a tensão nos sensores
  float sensor0Voltage = analogRead(SENSOR0_PIN) * (3.3 / 4095.0);
  float sensor1Voltage = analogRead(SENSOR1_PIN) * (3.3 / 4095.0);

  Serial.print("Sensor 0 Voltage: ");
  Serial.println(sensor0Voltage);
  Serial.print("Sensor 1 Voltage: ");
  Serial.println(sensor1Voltage);

  // Verifique as condições antes de definir a mensagem e enviar o pacote
  if (sensor0Voltage > 0.25 && sensor1Voltage <0.25) {
    Serial.println("Nivel adequado, nenhum pacote enviado.");
    delay(1000);
    return;
  }

  String message;

  // Verifique as condições e defina a mensagem
  if (sensor0Voltage < 0.25 && sensor1Voltage < 0.25) {
    message = "low";
  } else if (sensor1Voltage > 0.25) {
    message = "high";
  }

  // Envie a mensagem
  Serial.print("Enviando pacote: ");
  Serial.println(message);

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  totalSentPackets++;

  // Esperar pela confirmação de recebimento (ACK)
  long startTime = millis();
  bool ackReceived = false;

  while (millis() - startTime < 5000) { // Espera por até 5 segundos
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String received = "";
      while (LoRa.available()) {
        received += (char)LoRa.read();
      }

      if (received == "ACK") {
        ackReceived = true;
        totalReceivedPackets++;
        break;
      }
    }
  }

  // Calcule a taxa de perda de pacotes
  int lostPackets = totalSentPackets - totalReceivedPackets;
  float packetLossRate = (float)lostPackets / totalSentPackets * 100;

  Serial.print("Pacotes enviados: ");
  Serial.println(totalSentPackets);
  Serial.print("Pacotes recebidos: ");
  Serial.println(totalReceivedPackets);
  Serial.print("Pacotes perdidos: ");
  Serial.println(lostPackets);
  Serial.print("Taxa de perda de pacotes: ");
  Serial.print(packetLossRate);
  Serial.println("%");

  // Exiba os parâmetros de qualidade do sinal
  int rssi = LoRa.packetRssi();
  float snr = LoRa.packetSnr();
  Serial.print("RSSI: ");
  Serial.println(rssi);
  Serial.print("SNR: ");
  Serial.println(snr);

  if (!ackReceived) {
    Serial.println("Nenhum ACK recebido, retransmissão necessária.");
  } else {
    Serial.println("Confirmação de recebimento (ACK) recebida!");
  }

  delay(1000);
}
