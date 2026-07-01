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

## Configuração por pad

Cada pad possui:

| Campo | Função |
|---|---|
| `threshold` | Valor mínimo para iniciar uma batida |
| `peakMax` | Valor esperado para velocity 127 |
| `volume` | Volume máximo do pad |
| `debounceMs` | Tempo de bloqueio depois de uma batida |
| `scanMs` | Janela máxima para procurar o pico da batida |
| `curve` | Curva de velocity |

## Estados atuais

```text
IDLE
  Espera sinal passar do threshold.

WAIT_PEAK
  Captura o maior valor da batida.
  Dispara quando o pico começa a cair ou quando scanMs expira.

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

## Crosstalk simples

O crosstalk atual usa janela de tempo e razão de pico:

```text
se outro pad disparar muito próximo do anterior
  e o pico for muito menor
    bloqueia como crosstalk
```

Parâmetros atuais:

```text
CROSSTALK_WINDOW_MS = 25
CROSSTALK_RATIO_PERCENT = 45
```

## Próximas melhorias

- Crosstalk Matrix por pares de pads
- Retrigger Cancel por curva de decaimento
- Adaptive Scan baseado no tipo de pad
- Estatísticas de peak mínimo/médio/máximo
- Auto-calibração
