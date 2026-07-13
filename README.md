# Sistema de Controle de Nível de Líquido via LoRa 🌊📡

Um sistema robusto de telemetria e controle desenvolvido em **C++**, utilizando comunicação de radiofrequência LoRa (915 MHz) para o monitoramento remoto do nível de líquidos. O projeto evoluiu de uma arquitetura básica ponto-a-ponto para uma rede com **Quality of Service (QoS)**, focando em resiliência, tolerância a falhas e telemetria avançada.

---

## 🚀 Arquitetura e Evolução do Projeto

O repositório está dividido em duas abordagens arquiteturais:

### 📡 Versão 1: Ponto-a-Ponto (Ping & Pong)
Focada em baixo consumo e endereçamento customizado.
* **Transmissor (Sensor):** Lê a tensão em dois pinos analógicos (`A0` e `A1`). Se a tensão for < `0.15V` (nível baixo), envia `"ligar"`. Se > `0.15V` (nível alto), envia `"desligar"`. Após o envio, o módulo entra em `sleep()` para conservação profunda de bateria.
* **Receptor (Atuador):** Fica em escuta ativa. Filtra pacotes pelo endereço de destino (`0xB0`) e aciona um módulo relé (`D4`) conforme o comando recebido.

### 📶 Versão 2: Quality of Service e ESP32 (Pasta `V2_LoRa_QoS_LilygoT3`)
Evolução do sistema utilizando placas **Lilygo T3 (ESP32)**, introduzindo conceitos de redes de alta confiabilidade:
* **Confirmação de Recebimento (ACK):** O transmissor aguarda a confirmação de entrega do pacote pelo receptor. Se falhar, o sistema registra a perda, garantindo previsibilidade da comunicação.
* **Telemetria de Rede Dinâmica:** Cálculo em tempo real da taxa de perda de pacotes (`packetLossRate`), além da extração de **RSSI** e **SNR** para avaliar a saúde do link de rádio.
* **Interface OLED Local:** Integração com display OLED (`SSD1306Wire`) no receptor para monitoramento em tempo real do status da bomba, nível e qualidade do sinal, dispensando o monitor serial.

---

## 🛠️ Tecnologias Utilizadas

* **Linguagem:** C/C++ 
* **Protocolo de Rádio:** LoRa (915 MHz) via módulos RFM95 e Lilygo T3
* **Bibliotecas:** `SPI.h`, `LoRa.h` (Sandeep Mistry), `SSD1306Wire.h` (Display OLED)

---

## 🔌 Pinagem e Esquema de Ligação

Como o projeto suporta duas placas diferentes, as ligações do barramento SPI e dos sensores/atuadores devem seguir o hardware escolhido:

### Hardware V1 (Arduino Uno/Nano + RFM95)
| Função | Pino LoRa (RFM95) | Pino Arduino | Conexões Extras |
| :--- | :--- | :--- | :--- |
| **MISO** | MISO | D12 | **Sensores (TX):** A0 e A1 |
| **MOSI** | MOSI | D11 | **Relé (RX):** D4 |
| **SCK** | SCK  | D13 | |
| **NSS** | NSS/CS | D10 | |
| **RST** | RESET | D9 | |
| **IRQ** | DIO0 | D8 | |

### Hardware V2 (Lilygo T3 - ESP32)
| Função | Pino LoRa Interno | Pino ESP32 | Conexões Extras |
| :--- | :--- | :--- | :--- |
| **MISO** | MISO | GPIO 19 | **Sensores (TX):** GPIO 34 e 35 |
| **MOSI** | MOSI | GPIO 27 | **Relé (RX):** GPIO 12 |
| **SCK** | SCK  | GPIO 5  | **Display OLED:** I2C (SDA/SCL) |
| **NSS** | CS   | GPIO 18 | |
| **RST** | RESET | GPIO 15 (TX) / 23 (RX) | |
| **IRQ** | DIO0 | GPIO 26 | |

---

## 💡 Como executar o projeto

1. Faça o clone deste repositório.
2. Abra a pasta correspondente à versão desejada na [Arduino IDE](https://www.arduino.cc/en/software) ou [PlatformIO](https://platformio.org/).
3. Certifique-se de instalar a biblioteca `LoRa` e, caso utilize a V2, a biblioteca `ESP8266 and ESP32 OLED driver for SSD1306 displays`.
4. Faça o upload do código **Transmissor** para a placa que monitorará o líquido.
5. Faça o upload do código **Receptor** para a placa base que controlará a bomba d'água.
6. Abra o Monitor Serial para inspecionar o tráfego:
   * **Para a V1:** Utilize o *baud rate* de `9600`.
   * **Para a V2:** Utilize o *baud rate* de `115200` para acompanhar o cálculo de perda de pacotes e o recebimento de ACKs.
