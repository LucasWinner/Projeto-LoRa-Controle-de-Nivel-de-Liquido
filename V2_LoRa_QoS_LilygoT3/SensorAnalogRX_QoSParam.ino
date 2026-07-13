#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <SSD1306Wire.h>  // Biblioteca para o display OLED

// Defina os pinos para a placa Lilygo T3
#define SS      18
#define RST     23
#define DI0     26

// Pino do relé
#define RELAY_PIN 12

// Endereço I2C do display OLED
#define OLED_ADDR   0x3C

// Inicialize o display OLED
SSD1306Wire display(OLED_ADDR, SDA, SCL);

unsigned long lastPacketTime = 0;
unsigned long displayTimeout = 5000; // Tempo de espera em milissegundos (5 segundos)

void setup() {
  Serial.begin(115200);  // Alterado para 115200 para uma comunicação mais rápida
  while (!Serial);

  // Inicialize o display OLED
  display.init();
  display.flipScreenVertically();  // Ajuste a orientação do display se necessário
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "Inicializando LoRa...");
  display.display();

  Serial.println("Inicializando LoRa...");

  // Inicialize o pino do relé
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Desliga o relé inicialmente

  // Inicialize o SPI manualmente para definir os pinos personalizados
  SPI.begin(5, 19, 27, 18);  // Defina os pinos SCK, MISO, MOSI, e CS

  // Defina os pinos do módulo LoRa
  LoRa.setPins(SS, RST, DI0);

  // Tente inicializar o rádio LoRa
  if (!LoRa.begin(915E6)) {
    display.drawString(0, 10, "Falha ao inicializar LoRa!");
    display.display();
    Serial.println("Falha ao inicializar LoRa! Verifique as conexões e a frequência.");
    while (1);
  }

  display.drawString(0, 20, "LoRa inicializado!");
  display.display();
  Serial.println("LoRa inicializado com sucesso!");

  // Inicialize o tempo da última recepção de pacote
  lastPacketTime = millis();
}

void loop() {
  // Tente receber um pacote
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Leitura do pacote recebido
    String received = "";

    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();

    Serial.print("Pacote recebido: ");
    Serial.println(received);
    Serial.print("RSSI: ");
    Serial.println(rssi);
    Serial.print("SNR: ");
    Serial.println(snr);

    // Envie uma confirmação de recebimento (ACK) de volta ao transmissor
    LoRa.beginPacket();
    LoRa.print("ACK");
    LoRa.endPacket();

    // Atualize o tempo da última recepção de pacote
    lastPacketTime = millis();

    // Atualize o display OLED com os novos dados
    display.clear();
    display.drawString(0, 0, "Pacote status:");
    display.drawString(0, 10, received);
    display.drawString(0, 20, "RSSI: " + String(rssi));
    display.drawString(0, 30, "SNR: " + String(snr));

    // Verifique o conteúdo da mensagem recebida
    if (received == "low") {
      digitalWrite(RELAY_PIN, HIGH); // Liga o relé
      display.drawString(0, 40, "Nivel: Baixo");
      display.drawString(0, 50, "Bomba: Ligada");
    } else if (received == "high") {
      digitalWrite(RELAY_PIN, LOW); // Desliga o relé
      display.drawString(0, 40, "Nivel: Alto");
      display.drawString(0, 50, "Bomba: Desligada");
    }
    display.display();
  }

  // Verifique o tempo decorrido desde o último pacote recebido
  if (millis() - lastPacketTime > displayTimeout) {
    display.drawString(0, 0, "Pacote status:");
    display.drawString(0, 10, "Esperando");
    display.display();
    lastPacketTime = millis(); // Atualize para evitar atualização constante
  }
}
