# Hardware

Este documento registra o hardware previsto para o DrumOS.

## Controlador

- ESP32 DevKit no estágio atual
- ESP32-S3 recomendado futuramente para USB MIDI nativo

## Áudio

### DAC atual

PCM5102 via I2S.

Conexões padrão:

| ESP32 | PCM5102 |
|---|---|
| GPIO 26 | BCK |
| GPIO 25 | LRC / WS |
| GPIO 22 | DIN |
| GND | GND |
| 3V3/5V | VCC conforme módulo |

## Pads

Pads usam piezo ligados a entradas ADC do ESP32.

Mapeamento atual:

| Pad | Canal ADC |
|---|---|
| KICK | ADC1_CHANNEL_4 |
| SNARE | ADC1_CHANNEL_5 |
| HI-HAT | ADC1_CHANNEL_6 |
| CRASH | ADC1_CHANNEL_7 |
| HI-TOM | ADC1_CHANNEL_0 |
| LOW-TOM | ADC1_CHANNEL_3 |

## Proteção do piezo

Recomendações:

- resistor em série
- resistor de descarga
- diodos de proteção ou zener conforme circuito final
- aterramento comum bem feito

## Interface planejada

- OLED I2C
- Rotary Encoder
- Botão integrado ao encoder

## Expansões futuras

- SD Card para samples
- MIDI DIN
- USB MIDI com ESP32-S3
- Bluetooth MIDI
- Hi-hat controller contínuo
- Entradas dual zone
