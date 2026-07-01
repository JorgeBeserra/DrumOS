# DrumOS Roadmap

DrumOS é um módulo de bateria eletrônica open source para ESP32, focado em baixa latência, samples, MIDI e configuração simples.

## Milestone 0 — Infraestrutura

- [x] Migrar para PlatformIO
- [x] Criar GitHub Actions para build
- [x] Publicar firmware como artifact
- [ ] Adicionar releases automáticas por tag
- [ ] Adicionar análise de qualidade de código
- [ ] Documentar arquitetura e decisões técnicas

## Milestone 1 — Trigger Engine

Objetivo: capturar batidas com boa sensibilidade, dinâmica e controle de falsos disparos.

### Sprint 1.1 — Trigger básico calibrável

- [x] Threshold por pad
- [x] Debounce por pad
- [x] Scan window por pad
- [x] Curvas de velocity
- [x] PeakMax por pad
- [x] Comandos de shell para calibração
- [x] Crosstalk simples por janela de tempo e razão de pico

### Sprint 1.2 — Trigger avançado

- [ ] Retrigger Cancel
- [ ] Adaptive Scan
- [ ] Crosstalk Matrix
- [ ] Estatísticas por pad
- [ ] Auto-calibração inicial

## Milestone 2 — Pad Engine

- [ ] Dual zone para caixa e tons
- [ ] Rimshot e rim click
- [ ] Choke de pratos
- [ ] Ride bell/bow/edge
- [ ] Hi-hat contínuo

## Milestone 3 — Audio Engine

- [x] Samples em PROGMEM
- [x] Mixer simples com múltiplas vozes
- [ ] Voice allocator com prioridade
- [ ] Multi-layer samples
- [ ] Round robin
- [ ] Choke groups
- [ ] Limiter simples
- [ ] Pan e volume por instrumento

## Milestone 4 — Configuração

- [ ] Salvar configurações em Preferences/NVS
- [ ] Factory reset
- [ ] Export/import de configurações
- [ ] Kits configuráveis

## Milestone 5 — UI

- [ ] OLED I2C
- [ ] Rotary Encoder
- [ ] Menu de configuração
- [ ] Tela de status dos pads

## Milestone 6 — MIDI

- [ ] MIDI DIN via UART
- [ ] USB MIDI em ESP32-S3
- [ ] Bluetooth MIDI
- [ ] MIDI IN
- [ ] Modo AUDIO / MIDI / BOTH

## Milestone 7 — SD Card

- [ ] Carregar samples do SD
- [ ] Kits no SD
- [ ] Backup e restore de configurações

## Milestone 8 — DrumOS 1.0

- [ ] Trigger estável
- [ ] Samples multicamada
- [ ] Choke
- [ ] Hi-hat funcional
- [ ] MIDI funcional
- [ ] OLED + Encoder
- [ ] Configurações persistentes
- [ ] Documentação de montagem
