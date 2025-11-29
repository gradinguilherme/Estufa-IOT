# 🌱 Estufa Residencial Inteligente

<div align="center">

![Status](https://img.shields.io/badge/status-active-success.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-blue.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)

Sistema IoT para monitoramento e automação de estufa residencial com controle inteligente de temperatura, umidade, irrigação e ventilação.

</div>

---

## 📋 Índice

- [Sobre o Projeto](#sobre-o-projeto)
- [Funcionalidades](#funcionalidades)
- [Arquitetura](#arquitetura)
- [Hardware](#hardware)
- [Tecnologias](#tecnologias)
- [Instalação](#instalação)
- [Configuração](#configuração)
- [Uso](#uso)
- [Monitoramento](#monitoramento)
- [Parâmetros de Controle](#parâmetros-de-controle)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Contribuindo](#contribuindo)
- [Licença](#licença)

---

## 🎯 Sobre o Projeto

A **Estufa Residencial Inteligente** é um sistema IoT desenvolvido para automatizar e otimizar o cultivo de plantas em ambiente controlado. O projeto monitora continuamente os parâmetros ambientais e do solo, acionando sistemas de irrigação, ventilação e climatização de forma autônoma para manter as condições ideais de cultivo.

### Objetivos

- ✅ Monitorar temperatura e umidade do ar em tempo real
- ✅ Controlar umidade do solo com irrigação automática
- ✅ Automatizar ventilação e exaustão
- ✅ Registrar histórico de dados para análise
- ✅ Permitir controle remoto e monitoramento via InfluxDB
- ✅ Gerar alertas sobre condições ambientais

---

## ✨ Funcionalidades

### Monitoramento Contínuo

- **Temperatura**: Leitura a cada 5 segundos com sensor DHT11
- **Umidade do Ar**: Monitoramento constante da umidade relativa
- **Umidade do Solo**: Sensor capacitivo para medição precisa
- **Conectividade WiFi**: Monitoramento da intensidade do sinal (RSSI)

### Automação Inteligente

#### 🌡️ Controle de Temperatura
- **Aquecimento**: Ativa quando temperatura < 18°C
- **Resfriamento**: Liga ventilador quando temperatura > 24°C
- **Exaustão**: Desliga em modo de aquecimento para economia de energia

#### 💧 Sistema de Irrigação
- **Automático**: Aciona bomba quando umidade do solo > 1433 (~35%)
- **Manual**: Botão físico para irrigação sob demanda (1 segundo)
- **Proteção**: Sistema de debounce via interrupção

#### 🌬️ Ventilação e Exaustão
- **Exaustor**: Liga quando umidade do ar > 60%
- **Ventilador**: Ativa em temperaturas elevadas
- **Coordenação**: Sistemas trabalham em conjunto para otimizar clima

### Armazenamento de Dados

- **InfluxDB Cloud**: Registro timestamped de todas as métricas
- **Buffer de Dados**: Média de 2 leituras antes do envio (reduz ruído)
- **Intervalo**: Upload a cada 10 segundos
- **Métricas**: Temperatura, umidade do ar, umidade do solo e RSSI

---

## 🏗️ Arquitetura

```
┌─────────────────────────────────────────────────────────────┐
│                         ESP32 DevKit v1                      │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │  DHT11   │  │  Sensor  │  │  Botão   │  │   WiFi   │    │
│  │ Temp/Hum │  │   Solo   │  │  Manual  │  │  Client  │    │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘    │
│       │             │             │             │           │
│       └─────────────┴─────────────┴─────────────┘           │
│                          │                                   │
│                    ┌─────▼─────┐                            │
│                    │  Control  │                            │
│                    │   Logic   │                            │
│                    └─────┬─────┘                            │
│                          │                                   │
│       ┌──────────────────┼──────────────────┐               │
│       │                  │                  │               │
│  ┌────▼─────┐  ┌────────▼────┐  ┌─────────▼──────┐        │
│  │  Bomba   │  │ Ventiladores │  │   Aquecedor    │        │
│  │  d'Água  │  │ (Exaust/Cool)│  │      LED       │        │
│  └──────────┘  └──────────────┘  └────────────────┘        │
│                                                               │
└───────────────────────────┬───────────────────────────────┘
                            │
                            │ HTTPS
                            ▼
                   ┌─────────────────┐
                   │  InfluxDB Cloud │
                   │   (us-east-1)   │
                   └─────────────────┘
```

---

## 🔧 Hardware

### Componentes Principais

| Componente | Modelo | Função | Pino |
|------------|--------|--------|------|
| **Microcontrolador** | ESP32 DevKit v1 | Processamento e conectividade | - |
| **Sensor Temp/Umidade** | DHT11 | Medição de temperatura e umidade do ar | GPIO 4 |
| **Sensor de Solo** | Capacitivo | Medição de umidade do solo | GPIO 5 |
| **Bomba d'Água** | Relé 5V | Sistema de irrigação | GPIO 18 |
| **Exaustor** | Relé 5V | Ventilação e controle de umidade | GPIO 19 |
| **Ventilador** | Relé 5V | Resfriamento | GPIO 21 |
| **Aquecedor** | LED (representação) | Aquecimento | GPIO 22 |
| **Botão Manual** | Push Button | Irrigação manual | GPIO 23 |

### Especificações Técnicas

- **Alimentação**: 5V via USB ou fonte externa
- **Comunicação**: WiFi 802.11 b/g/n
- **Taxa de Baud**: 115200 bps (Serial)
- **Tensão de Operação**: 3.3V (lógica)

---

## 💻 Tecnologias

### Plataforma e Framework

- **PlatformIO**: Ambiente de desenvolvimento
- **Arduino Framework**: Framework base para ESP32
- **Espressif32**: Plataforma ESP32

### Bibliotecas

```ini
├── DHT sensor library (v1.4.6)      # Leitura do sensor DHT11
├── PubSubClient (v2.8)              # Comunicação MQTT (preparado)
├── NTPClient (v3.2.1)               # Sincronização de tempo
└── ESP8266 Influxdb (v3.13.2)       # Cliente InfluxDB
```

### Serviços Cloud

- **InfluxDB Cloud**: Armazenamento de séries temporais
- **NTP Servers**: Sincronização de tempo (pool.ntp.org)

---

## 📥 Instalação

### Pré-requisitos

- [PlatformIO](https://platformio.org/) instalado
- [Visual Studio Code](https://code.visualstudio.com/) (recomendado)
- Cabo USB para ESP32
- Driver CP2102 ou CH340 (se necessário)

### Passo a Passo

1. **Clone o repositório**
   ```bash
   git clone https://github.com/gustavo-gbarreto/Estufa-IOT.git
   cd Estufa-IOT
   ```

2. **Instale as dependências**
   ```bash
   pio pkg install
   ```

3. **Configure as credenciais** (veja [Configuração](#configuração))

4. **Compile o projeto**
   ```bash
   pio run
   ```

5. **Upload para o ESP32**
   ```bash
   pio run --target upload
   ```

6. **Monitore a execução**
   ```bash
   pio device monitor
   ```

---

## ⚙️ Configuração

### 1. Credenciais WiFi

Edite o arquivo `src/main.cpp`:

```cpp
#define WIFI_SSID         "SEU_SSID"
#define WIFI_PASSWORD     "SUA_SENHA"
```

### 2. Configuração InfluxDB

Crie uma conta no [InfluxDB Cloud](https://cloud2.influxdata.com/) e configure:

```cpp
#define INFLUXDB_URL      "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN    "seu_token_aqui"
#define INFLUXDB_ORG      "sua_organizacao"
#define INFLUXDB_BUCKET   "Estufa"
```

### 3. Timezone

Ajuste o fuso horário conforme sua localização:

```cpp
#define TZ_INFO "UTC-3"  // Brasília
```

### 4. Parâmetros de Controle

Ajuste os limiares conforme necessário:

```cpp
// Umidade do solo (irrigação)
if(Moisture > 1433) { ... }  // ~35% de umidade

// Umidade do ar (exaustão)
if(dht_humidity > 60) { ... }

// Temperatura (resfriamento)
if(dht_temperature > 24) { ... }

// Temperatura (aquecimento)
if(dht_temperature < 18) { ... }
```

---

## 🚀 Uso

### Inicialização

1. Conecte o ESP32 à alimentação
2. O sistema conectará automaticamente ao WiFi
3. Sincronização de tempo via NTP
4. Início do monitoramento e controle

### Operação Normal

O sistema opera de forma totalmente autônoma:

- ✅ Leitura de sensores a cada 5 segundos
- ✅ Decisões de controle em tempo real
- ✅ Upload de dados a cada 10 segundos
- ✅ Irrigação automática baseada em umidade do solo

### Irrigação Manual

Pressione o botão conectado ao GPIO 23 para:
- Acionar a bomba por 1 segundo
- Irrigação de emergência ou complementar

### Monitor Serial

Conecte-se ao monitor serial (115200 baud) para ver:

```
Connecting to wifi
...
Connected to WiFi
Connected to InfluxDB: https://us-east-1-1.aws.cloud2.influxdata.com

Humidity: 65.00%  Temperature: 22.00°C 
Moisture Value: 1250.50
Writing: ESTUFA,device=ESP32,SSID=MinhaRede rssi=-45,temperature=22.00,humidity=65.00,moisture=1250.50
```

---

## 📊 Monitoramento

### InfluxDB Dashboard

Os dados são enviados para o InfluxDB Cloud com as seguintes métricas:

| Campo | Tipo | Descrição | Unidade |
|-------|------|-----------|---------|
| `temperature` | float | Temperatura do ar | °C |
| `humidity` | float | Umidade relativa do ar | % |
| `moisture` | float | Umidade do solo | ADC value |
| `rssi` | int | Intensidade do sinal WiFi | dBm |

### Tags

- `device`: ESP32
- `SSID`: Nome da rede WiFi

### Exemplo de Query (Flux)

```flux
from(bucket: "Estufa")
  |> range(start: -1h)
  |> filter(fn: (r) => r["_measurement"] == "ESTUFA")
  |> filter(fn: (r) => r["_field"] == "temperature" or r["_field"] == "humidity")
```

---

## 🎛️ Parâmetros de Controle

### Limiares de Operação

```cpp
┌─────────────────────────────────────────────────────────┐
│ Parâmetro              │ Limite  │ Ação                 │
├────────────────────────┼─────────┼──────────────────────┤
│ Temperatura Mínima     │ < 18°C  │ Liga aquecedor       │
│ Temperatura Máxima     │ > 24°C  │ Liga ventilador      │
│ Umidade Ar Máxima      │ > 60%   │ Liga exaustor        │
│ Umidade Solo Mínima    │ > 1433  │ Liga irrigação       │
└─────────────────────────────────────────────────────────┘
```

### Intervalos de Atualização

- **Leitura de sensores**: 5000ms (5s)
- **Envio InfluxDB**: 10000ms (10s)
- **Buffer de médias**: 2 amostras

---

## 📁 Estrutura do Projeto

```
Estufa-IOT/
│
├── src/
│   └── main.cpp              # Código principal
│
├── include/
│   └── README                # Headers personalizados (se houver)
│
├── lib/
│   └── README                # Bibliotecas privadas
│
├── test/
│   └── README                # Testes unitários
│
├── platformio.ini            # Configuração do PlatformIO
└── README.md                 # Este arquivo
```

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Para contribuir:

1. Faça um Fork do projeto
2. Crie uma branch para sua feature (`git checkout -b feature/NovaFuncionalidade`)
3. Commit suas mudanças (`git commit -m 'Adiciona nova funcionalidade'`)
4. Push para a branch (`git push origin feature/NovaFuncionalidade`)
5. Abra um Pull Request

### Ideias para Contribuição

- [ ] Implementação de controle de luz/luminosidade
- [ ] Sensor de CO₂
- [ ] Dashboard web local
- [ ] Notificações push
- [ ] Controle via MQTT
- [ ] Modo de economia de energia
- [ ] Calibração automática de sensores

---

## 📄 Licença

Este projeto está sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

---

## 👥 Autores

- **Guilherme Gradin** - [GitHub](https://github.com/gradinguilherme)
- **Gustavo Barreto** - [GitHub](https://github.com/gustavo-gbarreto)
- **Juan Victor Vieira** - [GitHub](https://github.com/juanvvieira)
- **Uriel Ribeiro** - [GitHub](https://github.com/UrielHRO)

---

## 📞 Suporte

Para dúvidas ou problemas:

- Abra uma [Issue](https://github.com/gustavo-gbarreto/Estufa-IOT/issues)
- Entre em contato via email

---
