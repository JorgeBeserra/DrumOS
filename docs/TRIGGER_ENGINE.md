# Trigger Engine

O Trigger Engine é responsável por transformar sinais dos piezos em eventos musicais confiáveis.

## Objetivos

- Detectar batidas reais
- Ignorar ruído elétrico
- Capturar o pico da batida
- Calcular velocity musical
- Evitar disparo duplo
- Reduzir crosstalk entre pads

## Configuração por pad

Cada pad possui:

| Campo | Função |
|---|---|
| `threshold` | Valor mínimo para iniciar uma batida |
| `peakMax` | Valor esperado para velocity 127 |
| `volume` | Volume máximo do pad |
| `debounceMs` | Tempo de bloqueio depois de uma batida |
| `scanMs` | Janela para procurar o pico da batida |
| `curve` | Curva de velocity |

## Estados atuais

```text
IDLE
  Espera sinal passar do threshold.

WAIT_PEAK
  Durante scanMs, captura o maior valor da batida.

MASK_TIME
  Durante debounceMs, ignora novas leituras do mesmo pad.
```

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
