# Sistema de Controle de Nível de Líquido via LoRa (RFM95) 🌊📡

Um sistema robusto de telemetria e controle desenvolvido em **C++**, utilizando comunicação de radiofrequência LoRa (915 MHz) para o monitoramento remoto do nível de líquidos. O sistema avalia sensores analógicos e transmite comandos sem fio para acionamento de atuadores (como bombas d'água), com foco em resiliência e baixo consumo de energia.

## 🚀 Arquitetura do Projeto

A rede opera em topologia Ponto-a-Ponto (Ping & Pong) com endereçamento customizado e é dividida em dois nós principais:

### 1. Nó Transmissor (Sensor)
* **Endereço de Rede:** `0xA0`
* **Funcionamento:** Realiza a leitura de tensão de dois sensores conectados aos pinos analógicos `A0` e `A1`. 
* **Lógica de Controle:**
  * Se a tensão em ambos os pinos for menor que `0.15V` (indicando nível baixo), acorda o módulo de rádio e transmite o comando `"ligar"`.
  * Se a tensão no pino `A1` for maior que `0.15V` (indicando nível alto/seguro), transmite o comando `"desligar"`.
* **Eficiência Energética:** Após o envio da mensagem (a cada 5 segundos), o módulo LoRa entra em modo `sleep()` para conservação profunda de bateria.

### 2. Nó Receptor (Atuador)
* **Endereço de Rede:** `0xB0`
* **Funcionamento:** Fica em modo de escuta ativa. Filtra as mensagens pelo endereço de destino para ignorar ruídos ou pacotes de outras redes.
* **Lógica de Acionamento:** Ao receber o comando `"ligar"`, aciona o pino digital `4` (estado `HIGH`), ideal para acionar um módulo relé. Ao receber `"desligar"`, corta a energia do pino `4` (`LOW`).
* **Monitoramento de Sinal:** Extrai e exibe dados críticos da rede, como **RSSI** (Força do Sinal) e **SNR** (Relação Sinal-Ruído), permitindo o mapeamento da qualidade do link de rádio.

---

## 🛠️ Tecnologias e Hardware

* **Linguagem:** C/C++ (Padrão Arduino)
* **Comunicação:** Módulos LoRa RFM95 (Frequência `915E6` - 915 MHz)
* **Microcontrolador:** Compatível com arquiteturas Arduino (Uno, Nano, Mega)
* **Bibliotecas:** 
  * `SPI.h` (Comunicação de barramento padrão)
  * `LoRa.h` (Driver do rádio LoRa)

---

## 🔌 Pinagem e Esquema de Ligação

A comunicação entre o microcontrolador e o transceptor LoRa utiliza o barramento SPI em alta velocidade (`8E6` / MSBFIRST). As ligações devem ser feitas estritamente conforme o esquema abaixo para ambos os nós:

| Pino LoRa (RFM95) | Pino Microcontrolador | Função |
| :--- | :--- | :--- |
| **MISO** | D12 | Master In Slave Out |
| **MOSI** | D11 | Master Out Slave In |
| **SCK** | D13 | Serial Clock |
| **NSS / CS** | D10 | Chip Select |
| **RESET** | D9 | Reset do Módulo |
| **DIO0 / IRQ** | D8 | Interrupção |

**Conexões Extras (Específicas de cada Nó):**
* **Transmissor:** Conectar os sensores de nível (ou boias atuando como divisores de tensão) nas portas analógicas `A0` e `A1`.
* **Receptor:** Conectar o atuador/relé na porta digital `D4`.

---

## 💡 Como executar o projeto

1. Faça o clone deste repositório na sua máquina local.
2. Abra os códigos utilizando a [Arduino IDE](https://www.arduino.cc/en/software) ou [PlatformIO](https://platformio.org/).
3. Certifique-se de instalar a biblioteca `LoRa` gerenciada por Sandeep Mistry via Library Manager.
4. Faça o upload do código `Transmissor` para a placa remota (que ficará no reservatório).
5. Faça o upload do código `Receptor` para a placa base (que controlará a bomba).
6. Abra o Monitor Serial (baud rate: `9600`) para inspecionar os pacotes, voltagens lidas e a qualidade do sinal (RSSI/SNR).
