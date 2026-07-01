# DrumOS Design Decisions

Este arquivo registra decisões técnicas importantes do projeto.

## Decisão 001 — Usar PlatformIO

### Contexto

O projeto começou com estilo Arduino, mas a complexidade cresceu rapidamente.

### Decisão

Usar PlatformIO como ambiente principal.

### Motivos

- Organização melhor de pastas
- Build reproduzível
- GitHub Actions
- Melhor controle de dependências
- Escala melhor para firmware grande

## Decisão 002 — Separar Drivers, Engine, Modules e UI

### Contexto

O firmware começou a ter áudio, trigger, shell, samples, display e MIDI.

### Decisão

Separar responsabilidades por camada.

### Motivos

- Facilitar manutenção
- Evitar acoplamento forte
- Permitir trocar hardware sem mexer no motor musical
- Preparar o projeto para crescer

## Decisão 003 — Usar samples em vez de síntese principal

### Contexto

A síntese de kick/snare era possível, mas prato/crash ficou artificial e pesado.

### Decisão

Usar samples como base principal de som.

### Motivos

- Som mais real
- Menor complexidade de DSP
- Crash e ride ficam melhores
- Permite multi-layer e round robin no futuro

## Decisão 004 — Samples em PROGMEM no primeiro estágio

### Contexto

O ESP32 tem RAM limitada, mas flash suficiente para os samples iniciais.

### Decisão

Guardar samples iniciais em `const int16_t[] PROGMEM`.

### Motivos

- Evita estouro de DRAM
- Simples de compilar
- Bom para validar engine

### Futuro

Mover banco grande de samples para SD Card.

## Decisão 005 — Usar 22050 Hz como sample rate inicial

### Contexto

Samples 44100 Hz consomem o dobro de espaço e processamento.

### Decisão

Usar 22050 Hz mono 16-bit PCM no estágio inicial.

### Motivos

- Menor uso de flash
- Menor carga no mixer
- Qualidade suficiente para protótipo e estudo

## Decisão 006 — CI obrigatório

### Contexto

O firmware passou a ter muitos arquivos e dependências.

### Decisão

Compilar automaticamente no GitHub Actions a cada push/PR.

### Motivos

- Detectar erros cedo
- Garantir build limpo fora da máquina local
- Publicar firmware como artifact
