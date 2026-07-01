# DrumOS Architecture

Este documento descreve a arquitetura geral do DrumOS.

## Visão geral

```text
Hardware
  ↓
Drivers
  ↓
Engine
  ↓
Modules
  ↓
UI
```

## Camadas

### Core

Responsável pelo ciclo de vida principal do firmware.

Exemplos:

- Inicialização geral
- Loop principal
- Versão
- Configuração global
- Preferências globais

### Drivers

Responsável por hardware específico.

Exemplos:

- PCM5102
- SD Card
- OLED
- ADC
- EEPROM/Preferences

Regra: driver não deve conhecer regra musical. Driver só fala com hardware.

### Engine

Coração do DrumOS.

Exemplos:

- Audio
- Mixer
- Trigger
- Velocity
- Pads
- Voices
- Samples

Regra: a Engine implementa comportamento de bateria eletrônica, mas não deve cuidar de interface de usuário.

### Modules

Funcionalidades independentes que usam a Engine.

Exemplos:

- Click
- Kits
- MIDI
- Bluetooth MIDI
- Recorder

### UI

Interface de operação.

Exemplos:

- Shell serial
- OLED
- TFT
- Web

Regra: UI deve chamar APIs de módulos/engine; não deve manipular hardware diretamente.

## Fluxo de áudio

```text
Pad ADC
  ↓
Trigger Engine
  ↓
Velocity Engine
  ↓
Voice/Sample Selection
  ↓
Mixer
  ↓
PCM5102 Driver
  ↓
Saída de áudio
```

## Fluxo de trigger

```text
IDLE
  ↓
WAIT_PEAK
  ↓
MASK_TIME
  ↓
IDLE
```

O estado atual é simples e estável. A evolução prevista é adicionar adaptive scan, retrigger cancel e crosstalk matrix.

## Regras de arquitetura

1. Drivers não devem depender de UI.
2. UI não deve acessar hardware diretamente.
3. Configurações globais devem ficar centralizadas.
4. Lógica de pad deve ficar separada de lógica de áudio.
5. Samples grandes devem ficar em flash, SD ou filesystem, nunca em RAM global.
6. Cada commit deve manter o firmware compilável.
