# 🥁 DrumOS

### Open Source Drum Module Firmware for ESP32

[![Release](https://img.shields.io/github/v/release/JorgeBeserra/DrumOS?style=for-the-badge)](https://github.com/JorgeBeserra/DrumOS/releases)
[![License](https://img.shields.io/github/license/JorgeBeserra/DrumOS?style=for-the-badge)](LICENSE)
[![Stars](https://img.shields.io/github/stars/JorgeBeserra/DrumOS?style=for-the-badge)](https://github.com/JorgeBeserra/DrumOS/stargazers)
[![Forks](https://img.shields.io/github/forks/JorgeBeserra/DrumOS?style=for-the-badge)](https://github.com/JorgeBeserra/DrumOS/network)
[![Issues](https://img.shields.io/github/issues/JorgeBeserra/DrumOS?style=for-the-badge)](https://github.com/JorgeBeserra/DrumOS/issues)
[![ESP32](https://img.shields.io/badge/ESP32-Compatible-blue?style=for-the-badge)](https://www.espressif.com/)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge)]()


O **DrumOS** é um firmware Open Source desenvolvido para transformar o **ESP32** em um módulo de bateria eletrônica moderno, oferecendo baixa latência, alta precisão na leitura dos pads e uma arquitetura preparada para expansão.

O projeto foi concebido para servir tanto a quem deseja montar sua própria bateria eletrônica (DIY) quanto a desenvolvedores interessados em criar módulos completos e altamente configuráveis.

Ao invés de ser apenas um projeto Arduino, o DrumOS está sendo desenvolvido como um firmware modular, inspirado na arquitetura utilizada por equipamentos profissionais.


---

# ✨ Recursos

- 🥁 Trigger Engine
  - Peak Detection
  - Velocity
  - Debounce individual
  - Threshold por pad
  - Scan Time
  - Retrigger Cancel

- 🎵 Sintetizador interno
  - Kick
  - Snare
  - Hi-Hat
  - Crash
  - Hi-Tom
  - Low-Tom

- 🔊 Áudio I2S
  - PCM5102A
  - 44.1kHz
  - 16 Bits

- 🎚 Mixer interno
  - Polyphony
  - Volume Master
  - Volume individual por pad

- 🎼 Click (Metrônomo)
  - BPM configurável
  - Forte / Fraco
  - Liga / Desliga

- ⚙️ Shell Serial
  - Configuração em tempo real
  - Scope dos pads
  - Ajuste de Threshold
  - Ajuste de Volume
  - Ajuste do Click

---

# 🚀 Roadmap

## ✅ v0.1

- Estrutura inicial
- Trigger Engine
- Áudio I2S
- Síntese dos instrumentos
- Shell
- Click

## 🚧 v0.2

- Refatoração completa da arquitetura
- Voice Engine
- Trigger Engine modular
- Mixer independente
- Shell modular

## 📦 v0.3

- Sistema de Voices
- Sistema de Drum Kits
- Configuração persistente
- Perfis de usuários

## 💾 v0.4

- Samples WAV
- Cartão SD
- Carregamento dinâmico de Kits
- Streaming de Samples

## 🎹 v0.5

- USB MIDI
- Bluetooth MIDI
- MIDI DIN
- Mapeamento MIDI

## 🥁 v0.6

- Hi-Hat contínuo
- Choke
- Positional Sensing
- Crosstalk Cancel
- Retrigger Cancel avançado

## 🖥 v0.7

- Display OLED
- TFT
- Interface Web
- OTA

## ⭐ v1.0

Primeira versão estável do DrumOS.

---

# 🏗 Arquitetura

```text
                    DrumOS
                       │
                 ┌─────┴─────┐
                 │    Core   │
                 └─────┬─────┘
                       │
        ┌──────────────┼──────────────┐
        │              │              │
     Drivers        Engine         Modules
        │              │              │
        │              │              │
     ADC/I2S       Trigger        Click
     PCM5102       Voice          Kit
     SD Card       Audio          Recorder
     EEPROM        Mixer          MIDI
                       │
                  Output Engine
                       │
                    PCM5102A
```

---

# 📂 Estrutura do projeto

```text
DrumOS
│
├── Core
│   ├── DrumOS.cpp
│   ├── DrumOS.h
│   ├── Config.h
│   ├── Version.h
│   └── Logger.cpp
│
├── Drivers
│   ├── ADC
│   ├── I2S
│   ├── PCM5102
│   ├── SD
│   └── EEPROM
│
├── Engine
│   ├── Audio
│   ├── Trigger
│   ├── Voice
│   ├── Mixer
│   ├── Sample
│   └── Synth
│
├── Modules
│   ├── Click
│   ├── Kit
│   ├── Recorder
│   ├── MIDI
│   ├── Bluetooth
│   └── USB
│
├── UI
│   ├── Shell
│   ├── OLED
│   ├── TFT
│   └── Web
│
└── DrumOS.ino
```

---

# 🎯 Filosofia

O DrumOS foi criado seguindo alguns princípios:

- Arquitetura modular
- Código desacoplado
- Fácil manutenção
- Baixa latência
- Alta escalabilidade
- Hardware acessível
- 100% Open Source

O objetivo não é apenas reproduzir sons de bateria, mas criar uma plataforma completa para módulos eletrônicos modernos.

---

# 🔊 Áudio

- ESP32
- PCM5102A
- 44.100 Hz
- 16 Bits
- Polyphony
- Mixer em tempo real

---

# 🥁 Trigger Engine

Cada pad possui parâmetros independentes:

- Threshold
- Scan Time
- Mask Time
- Velocity Curve
- Volume
- Voice

Isso permite configurar diferentes tipos de pads e piezos com precisão.

---

# 🎼 Voice Engine

O DrumOS utiliza o conceito de **Voices**.

Cada pad dispara uma Voice, e uma Voice pode ser gerada por diferentes provedores:

- Synth Provider
- Sample Provider
- (Futuro) FM Provider
- (Futuro) Physical Modeling

Isso torna o firmware extremamente flexível e facilita a criação de kits personalizados.

---

# 🔮 Futuro

Alguns recursos planejados:

- Samples em WAV
- Drum Kits
- Equalizador
- Compressor
- Reverb
- Multi Outputs
- USB Audio
- Bluetooth MIDI
- USB MIDI
- MIDI DIN
- Atualização OTA
- Interface Web
- Display TFT
- Editor de Kits para PC
- Aplicativo Android/iOS

---

# 🤝 Contribuindo

Contribuições são muito bem-vindas!

Você pode ajudar de diversas formas:

- Reportando bugs
- Sugerindo melhorias
- Criando Pull Requests
- Testando o firmware
- Compartilhando ideias

Toda ajuda é bem-vinda.

---

# ⭐ Apoie o projeto

Se você gostou do DrumOS, deixe uma ⭐ no repositório.

Isso ajuda o projeto a alcançar mais pessoas e incentiva o desenvolvimento contínuo.

---

# 📄 Licença

Este projeto é distribuído sob a licença **MIT**.

---

# 👨‍💻 Autor

**Jorge Augusto Beserra de Souza**

Desenvolvedor do DrumOS.

---

> **DrumOS**
>
> *Building the next generation of Open Source Drum Module Firmware.*
