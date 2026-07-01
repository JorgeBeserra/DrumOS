# Audio Engine

O Audio Engine é responsável por tocar samples, misturar vozes e enviar áudio ao driver do DAC.

## Fluxo atual

```text
TriggerPad
  ↓
Voices::getBuffer()
  ↓
Audio::playBuffer()
  ↓
Mixer interno simples
  ↓
PCM5102::writeStereoBlock()
```

## Decisões atuais

- Samples mono 16-bit
- Sample rate inicial de 22050 Hz
- Samples em PROGMEM
- Saída estéreo duplicando o mesmo sinal nos dois canais
- Mixer por soma simples com volume por voz

## Cuidados

Samples em PROGMEM devem ser lidos com `pgm_read_word()`.

Não usar arrays grandes em RAM:

```cpp
int16_t sample[] = { ... }; // errado para samples grandes
```

Usar:

```cpp
const int16_t sample[] PROGMEM = { ... }; // correto
```

## Próximas melhorias

- Voice allocator com prioridade
- Voice stealing inteligente
- Limiter simples
- Pan por instrumento
- Choke groups
- ADSR por sample
- Multi-layer samples
- Round robin
- Samples via SD Card
