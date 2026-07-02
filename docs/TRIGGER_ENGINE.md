# Trigger Engine

O Trigger Engine é responsável por transformar sinais dos piezos em eventos musicais confiáveis.

## Objetivos

- Detectar batidas reais
- Ignorar ruído elétrico
- Capturar o pico da batida
- Calcular velocity musical
- Evitar disparo duplo
- Reduzir crosstalk entre pads
- Reduzir latência sem perder o pico real da batida
- Medir estatísticas para calibração
- Visualizar o sinal do piezo pelo terminal serial

## Configuração por pad

Cada pad possui:

| Campo | Função |
|---|---|
| `threshold` | Valor mínimo para iniciar uma batida |
| `peakMax` | Valor esperado para velocity 127 |
| `volume` | Volume máximo do pad |
| `debounceMs` | Tempo de bloqueio depois de uma batida |
| `scanMs` | Janela máxima para procurar o pico da batida |
| `retriggerLockMs` | Janela inicial para bloquear picos falsos da mesma vibração |
| `gateMargin` | Pico mínimo acima do threshold para aceitar a batida |
| `curve` | Curva de velocity |

## Estados atuais

```text
IDLE
  Espera sinal passar do threshold.

WAIT_PEAK
  Captura o maior valor da batida.
  Dispara quando o pico começa a cair ou quando scanMs expira.

RETRIGGER_LOCK
  Bloqueia picos fracos que ainda pertencem à mesma vibração do piezo.

MASK_TIME
  Durante debounceMs, ignora novas leituras do mesmo pad.
```

## Peak Tracking Inteligente

Antes o firmware sempre esperava `scanMs` completo antes de disparar.

Agora o Trigger Engine observa se o sinal começou a cair:

```text
600
1200
1800  <- peak
1690  <- queda detectada
```

Quando a queda é detectada depois de uma janela mínima, o sample é disparado imediatamente.

Isso reduz latência principalmente em pads rápidos, sem perder o pico real.

Parâmetros internos atuais:

```text
MIN_PEAK_SCAN_MS = 2
MIN_PEAK_FALL_DELTA = 24
PEAK_FALL_PERCENT = 4
REQUIRED_FALLING_SAMPLES = 1
```

`scanMs` continua existindo como tempo máximo de segurança. Se a queda não for detectada, o disparo acontece quando `scanMs` expira.

## Retrigger Cancel

Depois de uma batida, muitos piezos fazem uma oscilação com novos picos menores:

```text
2100
1600
900
650
820  <- pico falso
500
```

O estado `RETRIGGER_LOCK` reduz esses disparos falsos. Durante esse período, o firmware trata picos fracos como continuação da mesma batida.

O tempo é configurável por pad:

```text
lock kick 35
lock snare 30
lock crash 55
```

## Gate Margin

O filtro de margem evita que pequenos ruídos logo acima do `threshold` virem batidas.

A regra é:

```text
pico minimo aceito = threshold + gateMargin
```

Exemplo:

```text
thr kick 550
margin kick 80
```

Nesse caso, o KICK só dispara se o pico capturado for pelo menos `630`.

Comandos:

```text
margin kick 80
margin snare 70
margin crash 60
```

Se o pico for rejeitado, o contador `noise` em `stats` aumenta.

## Velocity

A velocity usa:

```text
threshold -> ponto mínimo
peakMax   -> ponto máximo
curve     -> resposta musical
```

Curvas disponíveis:

- `LINEAR`
- `SOFT`
- `HARD`

## Crosstalk Matrix

A matriz de crosstalk permite configurar quanto uma peça pode interferir em outra.

Comandos:

```text
xtalk show
xtalk reset
xtalk kick snare 15
xtalk crash hitom 30
```

A regra é direcional:

```text
xtalk crash hitom 30
```

significa que uma batida no CRASH pode bloquear uma vibração fraca no HI-TOM. O contrário só acontece se também existir:

```text
xtalk hitom crash 30
```

Valores:

| Nível | Efeito |
|---:|---|
| 0 | sem bloqueio entre o par |
| 10–30 | bloqueio leve/moderado |
| 40–70 | bloqueio forte |
| 80–100 | muito agressivo, usar com cuidado |

A matriz é salva junto com as configurações pelo comando:

```text
save
```

## Trigger Statistics

As estatísticas ajudam a calibrar os pads com dados reais.

Comandos:

```text
stats
stats kick
stats reset
```

Campos exibidos:

| Campo | Função |
|---|---|
| `hits` | quantidade de disparos aceitos |
| `minPeak` | menor pico aceito |
| `avgPeak` | pico médio aceito |
| `maxPeak` | maior pico aceito |
| `avgVelocity` | velocity média |
| `crosstalk` | disparos bloqueados por crosstalk |
| `retrigger` | picos bloqueados durante retrigger lock |
| `noise` | ruídos descartados pelo filtro de margem |

Esses dados serão usados futuramente pela auto-calibração.

## Osciloscópio Serial

O modo gráfico captura rapidamente 64 amostras do ADC e desenha uma curva simples no terminal.

Comando:

```text
scope graph kick
```

Exemplo de saída:

```text
Scope Graph: KICK
min=0 max=1840
1840 |             *
1686 |            ***
1533 |           *****
1226 |        *********
 613 |   ********
     +----------------------------------------------------------------
```

Esse modo ajuda a enxergar:

- pico real da batida;
- tempo de subida;
- cauda da vibração;
- ruído em repouso;
- necessidade de ajustar `threshold`, `scan`, `lock` e `margin`.

## Próximas melhorias

- Adaptive Scan baseado no tipo de pad
- Auto-calibração
